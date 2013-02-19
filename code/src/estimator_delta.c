#include "estimator.h"
#include "combinefunction.h"

char *pmf_params[] = {"delta_est_pmf_resolution", "delta_est_combine_start",
		      "delta_est_combine_interval", "delta_est_num_bins",
		      "delta_est_max_delta", "delta_est_step_coarse"};
char *hierarchical_params[] = {"delta_est_step_fine", "delta_est_fine_range"};
char *normaliser_params[] = {"delta_est_combine_start", "delta_est_combine_interval", 
			     "normaliser_est_initial", "normaliser_est_max",
			     "normaliser_est_step", "normaliser_est_subintervals"};
char *area_params[] = {"delta_est_area_resolution", "delta_est_max_delta",
		       "delta_est_step_coarse", "delta_est_area_start",
		       "delta_est_area_interval"};

/*
 * Helper function for pmf delay estimator. Extracts parameters from the paramfile provided and 
 * passes them on to another function to do the actual computation. Also performs hierarchical
 * estimates of the delay if the hierarchical parameter is set to yes. This will perform two passes
 * over the data and attempt to improve an initial coarse estimate with a finer search in a smaller
 * range.
 */
double estimate_delay_pmf(paramlist* params, char* outfile, double_arr* base_events, double_arr* f2_events,
			  void* f1, void* f2, double normaliser, char* hierarchical, char* type, int output_switch)
{
    if (!has_required_params(params, pmf_params, sizeof(pmf_params)/sizeof(char*))) {
	printf("Some parameters required to perform a PMF estimate" \
	       " of delay are missing. Ensure you have them in your parameter" \
	       " file.\n");
	exit(1);
    }
    int hc = strcmp(hierarchical, "yes") == 0;
    // Resolution when combining functions
    double combine_step = get_double_param(params, "delta_est_pmf_resolution");
    // Start of the interval used when combining the functions
    double combine_start = get_double_param(params, "delta_est_combine_start");
    // Length of the interval when combining functions
    double combine_interval = get_double_param(params, "delta_est_combine_interval");
    // Number of bins into which we split the event data
    int num_bins = get_int_param(params, "delta_est_num_bins");
    double max_delay = get_double_param(params, "delta_est_max_delta");

    double delta_step_coarse = get_double_param(params, "delta_est_step_coarse");
    double delta_step_fine = 0;
    double fine_range = 0;

    if (hc){
	if (!has_required_params(params, hierarchical_params,
				 sizeof(hierarchical_params)/sizeof(char*))){
	    printf("Some parameters required to perform a hierarchical estimate"\
		   " of delay are missing. Ensure you have them in your parameter"\
		   " file.\n");
	    exit(1);
	}
	delta_step_fine = get_double_param(params, "delta_est_step_fine");
	fine_range = get_double_param(params, "delta_est_fine_range");
    }

    if (max_delay >= combine_interval){
	printf("The maximum delay (%lf) cannot exceed the length of the"\
	       " interval (%lf).\n", max_delay, combine_interval);
	exit(1);
    }

    /* printf("Estimating delay with pmf method.\n Combine step %lf, Combine interval"\ */
    /* 	   " [%lf %lf], Bins %d, Max delay %lf, Step %lf\n", */
    /* 	   combine_step, combine_start, combine_end, num_bins, max_delay, delta_step_coarse); */

    double result = _estimate_delay_pmf(outfile, base_events, f2_events, f1, f2,
					combine_start, combine_interval, combine_step,
					num_bins, -max_delay, max_delay, max_delay,
					delta_step_coarse, normaliser, type, output_switch);

    if (hc){
	printf("Initial result is %lf. Improving estimate using finer step of %lf"\
	       " in range [%lf, %lf].\n", result, delta_step_fine, result - fine_range,
	       result + fine_range);
	char* hout = malloc(strlen(outfile) + strlen("_hier") + 5);
	sprintf(hout, "%s_hier", outfile);
	double hres = _estimate_delay_pmf(hout, base_events, f2_events, f1, f2,
					  combine_start, combine_interval, combine_step,
					  num_bins, result - fine_range,
					  result + fine_range, max_delay, delta_step_fine,
					  normaliser, type, output_switch);
	printf("Estimate revised to %lf from %lf.\n", hres, result);
	result = hres;
	free(hout);
    }
    
    return result;
}



/*
 * Computes the time delay between two functions f1 and f2. Requires data about the
 * event streams of which each of the functions is an estimate. f1 is the base function,
 * if there are more than two functions. This function is taken as the baseline against
 * which all other functions are compared, and so the time delay will be given relative to
 * it. The function splits the interval specified in the parameter file into a number of
 * bins, and calculates the number of events that fall into each bin. To calculate the time delay,
 * f2 is shifted relative to f1, starting at a negative shift and moving to a positive one.
 * The process is iterative, and the granularity of the estimate is defined by the step
 * specified in the parameter file. The estimate of the time delay is the point at which the
 * sum of the probability mass functions taken at a number of points along the combination of
 * the base function and the shifted function is minimised. In other words, the shift at
 * which the bin counts most closely match the lambda values taken from the functions
 * is the estimate returned. The functions are combined such that for all possible delays
 * in the range [-max_delay, max_delay], the same amount of data is gathered.
 */
double _estimate_delay_pmf(char* outfile, double_arr* base_events, double_arr* f2_events, void* f1, 
			   void* f2, double combine_start, double combine_interval,
			   double combine_step, int num_bins, double start_delta,
			   double end_delta, double max_delay, double delta_step, double normaliser,
			   char* type, int output_switch)
{
    // Always work on two streams
    int num_streams = 2;
    double combine_end = combine_start + combine_interval;

    double bin_length = (combine_end - combine_start)/num_bins;
    double current_delta = start_delta;

    // Set the initial values for our estimate
    double best_delta = -INFINITY;
    double best_value = -INFINITY;
    
    // We don't know which type we will get, so use a void pointer to store
    // it and cast later.
    void** store = NULL;

    if (strcmp(type, "gauss") == 0){
	store = malloc(sizeof(gauss_vector*) * 2);
	store[0] = (gauss_vector*)f1;
	store[1] = (gauss_vector*)f2;
    } else if (strcmp(type, "base") == 0){
	store = malloc(sizeof(est_arr*) * 2);
	store[0] = (est_arr*)f1;
	store[1] = (est_arr*)f2;
    }

    printf("cstart %lf cend %lf cint %lf cstep %lf nbins %d startd %lf endd %lf maxd %lf dstep %lf norm %lf\n", 
	   combine_start, combine_end, combine_interval, combine_step, num_bins, start_delta, end_delta, max_delay, delta_step, normaliser);

    double_arr* time_delay = init_double_arr(2);
    time_delay->data[0] = 0;
    double_multi_arr* combined = NULL;
    double_multi_arr* delay_pmfs = init_multi_array(2, (int)((end_delta - start_delta)/delta_step));

    // PMF sums will be calculated for both streams.
    int* bin_counts1 = sum_events_in_interval(base_events->data, base_events->len, combine_start,
					     combine_end, num_bins);
    int* bin_counts2 = sum_events_in_interval(f2_events->data, f2_events->len, combine_start,
					     combine_end, num_bins);
    
    double* midpoints = get_interval_midpoints(combine_start, combine_end,
					       num_bins);
    double* lambda_sums = malloc(sizeof(double) * num_bins);
    double* best_lambda_sums = malloc(sizeof(double) * num_bins);

    // How many bins need to be skipped to ensure that only the bins that are present in 
    int skip_bins = (int) max_delay/bin_length;
    int i, j = 0;

    printf("binlen %lf skipbin %d\n", bin_length, skip_bins);

    while (current_delta <= end_delta){
	// get the combined function with the delay applied
	// compare the combined function to the bin counts of the stream by calculating the pmf for each interval
	// to do this, first get the bin counts. These will remain constant for the function.
	// then, calculate the interval times for each bin, which will be the same for each
	// sum the values of lambda within this interval at each time. This will be one lambda per second, probably,
	// since each lambda represents the number of arrivals per single time step
	// finally, find the poisson pmf of the two values - gsl_ran_poisson_pdf(bin count, lambda sum for bin)
	printf("current delta %lf\n", current_delta);
	time_delay->data[1] = current_delta;
	if (strcmp(type, "gauss") == 0){
      	    combined = combine_gauss_vectors((gauss_vector**)store, time_delay,
					     combine_start, combine_end, combine_step,
					     num_streams);
    	} else if (strcmp(type, "base") == 0){
    	    combined = combine_functions((est_arr**)store, time_delay, combine_start,
					 combine_interval, combine_step, num_streams);
    	} else {
	    printf("Unknown type for estimate \"%s\" when estimating delta.\n", type);
	    exit(1);
	}

	for (i = 0; i < num_bins; ++i) {
	    // find the sum of lambda values for each subinterval
//	    printf("subinterval start %lf, subinterval end %lf\n", i * bin_length, (i + 1) * bin_length);
	    // The lambda sums must be normalised so that they are on the same scale as the bin counts.
	    // Otherwise, the result will be completely useless.
	    lambda_sums[i] = sum_array_interval(combined->data[0], combined->data[1],
					     i * bin_length, (i + 1) * bin_length,
						normaliser, combined->lengths[0]);
//	    printf("sum of lambdas in interval %lf\n", lambda_sums[i]);
	}

	/* char* cb = malloc(15); */
	/* sprintf(cb, "cfunc%lf", current_delta); */

	/* FILE *fp1 = fopen(cb, "w"); */

	/* for (i = 0; i < combined->lengths[0]; ++i) { */
	/*     fprintf(fp1, "%lf %lf\n", combined->data[0][i], combined->data[1][i]); */
	/* } */

	/* fclose(fp1); */
	
	/* char* ls = malloc(15); */
	/* sprintf(ls, "lsums%.0lf", current_delta); */
	/* FILE *fp3 = fopen(ls, "w"); */
	
	/* for (i = 0; i < num_bins; ++i) { */
	/*     fprintf(fp3, "%lf %lf\n", midpoints[i], lambda_sums[i]); */
	    
	/* } */
	/* fclose(fp3); */
	/* free(ls); */
	/* free(cb); */

	int s2_shift = (int)((max_delay - current_delta)/bin_length);
	printf("s2 shift is %d. Number of bins being checked %d\n", s2_shift, num_bins - 2 * skip_bins);

	/* for (i = 0; i < num_bins - 2 * skip_bins; ++i) { */
	/*     printf("Counts 1 %d counts 2 %d, comparing %lf from 1 with %lf from 2, lambda is %lf\n", */
	/* 	   (bin_counts1+skip_bins)[i], (bin_counts2+s2_shift)[i], */
	/* 	   (midpoints+skip_bins)[i], (midpoints+s2_shift)[i], */
	/* 	   (lambda_sums+skip_bins)[i]); */
	/* } */

//	printf("s1\n");
	double total1 = sum_log_pmfs(bin_counts1 + skip_bins, lambda_sums + skip_bins, 1, num_bins - 2 * skip_bins);
	// need to shift bin_counts2 so that the correct intervals line up for the given delay
//	printf("s2\n");
	double total2 = sum_log_pmfs(bin_counts2 + s2_shift, lambda_sums + skip_bins, 1, num_bins - 2 * skip_bins);
	double cumulative = total1 + total2;
	delay_pmfs->data[0][j] = current_delta;
	delay_pmfs->data[1][j] = cumulative;

//	printf("pmf sum is %lf\n", total);
	if (cumulative > best_value){
	    printf("New value %lf is less than old %lf. guess updated to %lf\n", cumulative, best_value, current_delta);
	    best_value = cumulative;
	    best_delta = current_delta;
	    memcpy(best_lambda_sums, lambda_sums, sizeof(double) * num_bins);
	} else {
	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", cumulative, best_value, best_delta);
	}

	current_delta += delta_step;
	free_double_multi_arr(combined);
	j++;
    }

    if (outfile != NULL && output_switch != 0){
	char* out = malloc(strlen(outfile) + strlen("_pmf_lambda.dat") + 5);
	if (output_switch >= 1){
	    sprintf(out, "%s_pmf_delay.dat", outfile);
	    output_double_multi_arr(out, "w", delay_pmfs);
	}

	if (output_switch >= 2){
	    sprintf(out, "%s_pmf_lambda.dat", outfile);
	    FILE *fp = fopen(out, "w");
		
	    for (i = 0; i < num_bins; ++i) {
		fprintf(fp, "%lf %lf\n", midpoints[i], best_lambda_sums[i]);
	    }
	    fclose(fp);


	    sprintf(out, "%s_pmf_bins.dat", outfile);
	    fp = fopen(out, "w");
	    for (i = 0; i < num_bins; ++i) {
		//	printf("bin %d [%d, %d] has %d events\n", i, i * bin_length, (i + 1) * bin_length, bin_counts[i]);
		/* bin_counts1[i] /= (combine_interval/ num_bins); */
		/* bin_counts2[i] /= (combine_interval/num_bins); */
	    
		fprintf(fp, "%lf %d %d\n", midpoints[i], bin_counts1[i], bin_counts2[i]);
	    }
	    fclose(fp);
	}
	
	free(out);
    }

    printf("number of bins that need to be skipped %d\n", skip_bins);

    free(midpoints);
    free(lambda_sums);
    free(best_lambda_sums);
    free(bin_counts1);
    free(bin_counts2);
    free(store);
    free_double_arr(time_delay);
    free_double_multi_arr(delay_pmfs);
    
    return best_delta;
}

/*
 * Helper function for finding the normalisation constant. Extracts parameters from the
 * parameter file and then calls the sub-function to do the actual calculation.
 */
double find_normaliser(paramlist* params, void* f1, double_arr* events, char* type)
{

    if (!has_required_params(params, normaliser_params,
			     sizeof(normaliser_params)/sizeof(char*))) {
	printf("Some parameters required to find a normalisation constant are missing."\
	       " Ensure you have them in your parameter file.\n");
	exit(1);
    }

    double combine_start = get_double_param(params, "delta_est_combine_start");
    // Length of the interval when combining functions
    double combine_interval = get_double_param(params, "delta_est_combine_interval");
    double combine_end = combine_start + combine_interval;
    // Start value for finding the normalisation constant
    double normaliser_start = get_double_param(params, "normaliser_est_initial");
    // Maximum value to allow the normalisation function to look at
    double normaliser_end = get_double_param(params, "normaliser_est_max");
    // Increase the normalisation constant value by this each iteration
    double normaliser_step = get_double_param(params, "normaliser_est_step");
    // Number of subintervals to use when checking normalisation
    int normaliser_subintervals = get_int_param(params, "normaliser_est_subintervals");

    // Find a normalisation constant. This is needed to make the estimated function
    // correctly line up with the original. For baseline estimates, this is usually 1.
    return _find_normaliser(f1, events, combine_start, combine_end,
				 normaliser_start, normaliser_end,
				 normaliser_step, normaliser_subintervals,
				 type);
}

/*
 * Finds the constant required to normalise the function to the given set of event
 * data. The check limit specifies the range of values that will be checked to find
 * the constant. Starts from 1 and goes up to check_limit with the given step.
 * Returns a negative value if something goes wrong. To get a good normaliser for
 * the actual function, it is probably best to use the number of subintervals equivalent
 * to the length of the interval divided by the time steps that lambda represents.
 * This is usually one time step. As an example, if lambda represents the number of 
 * events in one second, and your interval time is 100 seconds, you should use 100
 * intervals. A normaliser is required for functions estimated using the gaussian
 * kernel method due to the way that gaussians are summed. Dividing the values of
 * the estimated function by the return value of this function will give a function
 * that is on the same scale as the generating function.
 */
double _find_normaliser(void* f1, double_arr* events, double interval_start,
		       double interval_end, double check_start, double check_limit,
		       double step, int subintervals, char* type)
{
    double normaliser = check_start;
    double best = -INFINITY;
    double best_normaliser = -INFINITY;

    int* bin_counts = sum_events_in_interval(events->data, events->len, interval_start,
					     interval_end, subintervals);

    double* midpoints = get_interval_midpoints(interval_start, interval_end, subintervals);

    double_arr* function_values;

    if (strcmp(type, "gauss") == 0){
	function_values = sum_gaussians_at_points((gauss_vector*) f1, midpoints, subintervals);
    } else if (strcmp(type, "base") == 0){
	function_values = estimate_at_points((est_arr*) f1, midpoints, subintervals);
    } else {
	printf("Unknown type \"%s\"when finding normalisation constant.\n", type);
	exit(1);
    }

    while(normaliser <= check_limit){
	double pmf_sum = sum_log_pmfs(bin_counts, function_values->data, normaliser, subintervals);
//	printf("Sum with normaliser %lf: %lf, current best %lf (%lf).\n", normaliser, pmf_sum, best, best_normaliser);
	if (pmf_sum > best) {
	    best = pmf_sum;
	    best_normaliser = normaliser;
//	    printf("New best normaliser is %lf with a sum of %lf\n", best_normaliser, best);
	}
//		printf("total pmf is %lf, normaliser is %lf\n", pmf_sum, normaliser);
	normaliser += step;
    }

    free(midpoints);
    free(bin_counts);
    free_double_arr(function_values);
    return best_normaliser;
}


/*
 * Helper function for the area delay calculation. Extracts parameters from 
 * the parameter list and then calls the function which does the computation.
 * Also does hierarchical search if the parameter is set to yes, calling the 
 * estimation function again to make a finer pass over the data.
 */
double estimate_delay_area(paramlist* params, char* outfile, void* f1, void* f2,
			   char* hierarchical, char* type, int output_switch)
{
    if (!has_required_params(params, area_params, sizeof(area_params)/sizeof(char*))){
	printf("Some parameters required to perform an area estimate of the"\
	       " time delay are missing. Ensure you have them in your parameter file.\n");
	exit(1);
    }
    int hc = strcmp(hierarchical, "yes") == 0;
    double max_delay = get_double_param(params, "delta_est_max_delta");
    double step = get_double_param(params, "delta_est_step_coarse");
    double resolution = get_double_param(params, "delta_est_area_resolution");
    double comp_start = get_double_param(params, "delta_est_area_start");
    double comp_end = comp_start + get_double_param(params, "delta_est_area_interval");

    printf("Estimating delay using area method.\n Max delay: %lf, Step %lf,"\
	   " Resolution %lf, Interval [%lf %lf]\n", max_delay, step, resolution,
	   comp_start, comp_end);

    double step_fine = 0;
    double fine_range = 0;
    
    if (hc){
    	step_fine = get_double_param(params, "delta_est_step_fine");
    	fine_range = get_double_param(params, "delta_est_fine_range");
    }

    double result = _estimate_delay_area(outfile, f1, f2, comp_start, comp_end, 
					 -max_delay, max_delay, max_delay,
					 resolution, step, type, output_switch);

    if (hc){
    	printf("Initial result is %lf. Improving estimate using finer step of %lf" \
    	       " in range [%lf, %lf].\n", result, step_fine, result - fine_range,
    	       result + fine_range);
    	double hres = _estimate_delay_area(outfile, f1, f2, comp_start, comp_end,
    					   result - fine_range, result + fine_range,
    					   max_delay, resolution, step_fine, type,
					   output_switch);
    	printf("Estimate revised to %lf from %lf.\n", hres, result);
    	result = hres;
    }
    
    return result;

}

/*
 * Estimates the time delay of one function with relation to another by finding
 *  the minimum value of the area between their curves. This is done in a 
 * discrete manner by guessing a time delay and calculating the value,
 * and continuing to make guesses until the resulting area goes below
 * a threshold value. The max_delay parameter specifies the maximum
 * positive or negative delay to consider when attempting to find an estimate.
 * The resolution specifies the step between points on the x-axis which are
 * compared. A high resolution will take longer, but the estimate will be more
 * accurate. The returned value is the estimate of how much the second function
 * is shifted. The value calculated by the function is inverted upon return
 * to give the amount that f2 must be shifted to line it up with f1.
 */
double _estimate_delay_area(char* outfile, void* f1, void* f2, double comp_start, double comp_end,
			    double start_delta, double end_delta, double max_delay,
			    double resolution, double step, char* type, int output_switch)
{

    double best_delta = 0;
    double best_value = INFINITY;
    double guess;

    double current = start_delta;
    double norm_start = comp_start + max_delay, norm_end = comp_end - max_delay;
    FILE *fp = NULL;
    char* out = NULL;
    if (outfile != NULL && output_switch >= 1){
	out = malloc(strlen(outfile) + strlen("_area_delay.dat") + 5);
	sprintf(out, "%s_area_delay.dat", outfile);
	fp = fopen(out, "w");
    }

    while (current <= end_delta){
	if (strcmp(type, "gauss") == 0){
	    guess = total_area_estimate((void*)f1, (void*)f2, norm_start, norm_end, resolution, current, "gauss");
	} else if (strcmp(type, "base") == 0){
	    guess = total_area_estimate((void*)f1, (void*)f2, norm_start, norm_end, resolution, current, "base");
	}
	if (outfile != NULL)
	    fprintf(fp, "%lf %lf\n", current, guess);
	if (guess < best_value){
	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current);
	    best_value = guess;
	    best_delta = current;
	} else {
	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_delta);
	}
	current += step;
    }

    if (outfile != NULL){
	fclose(fp);
	free(out);
    }
    
    return best_delta;
}



/*
 * Estimates the total area between the curves f1 and f2 in the interval
 * [start, end], with samples taken at the given resolution and the given
 * delay applied to f2. The higher the resolution given, the higher better the
 * estimate will be, but the computation will also take longer.
 */
double total_area_estimate(void* f1, void* f2, double start, double end,
			   double resolution, double delay, char* type)
{
    if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0)
	return -1;

    double current = start, pt = 0;
    double sum = 0;
    int i = 0;

    while(current <= end){
	if (strcmp(type, "gauss") == 0){
	    pt = area_at_point_gauss((gauss_vector*)f1, (gauss_vector*)f2, current, delay);
	} else if (strcmp(type, "base") == 0){
	    pt = area_at_point_base((est_arr*)f1, (est_arr*)f2, current, delay);
	}
	if (pt != -1){
	    sum += pt;
	    ++i;
	}
	current += resolution;
    }

    return sum/i;
}

/*
 * Estimates the area between points at a given point on the x-axis, where function
 * f2 is delayed by delta. This version is used for calculation with structs
 * produced by the gaussing estimator.
 */
double area_at_point_gauss(gauss_vector* f1, gauss_vector* f2, double x, double delta)
{
    if (f1 == NULL || f2 == NULL)
	return -1;

    double f1sum = sum_gaussians_at_point(x, f1);
    double f2sum = sum_gaussians_at_point(x - delta, f2);

    return pow(f1sum - f2sum, 2);
}

/*
 * Estimates the area at a given point on the x-axis, where function f2 is delayed
 * by delta. This version of the function is used for calculation with
 * arrays produced by the baseline estimator
 */
double area_at_point_base(est_arr* f1, est_arr* f2, double x, double delay)
{
    if (f1 == NULL || f2 == NULL)
	return -1;

    double f1val = estimate_at_point(f1, x);
    double f2val = estimate_at_point(f2, x - delay);

    return pow(f2val - f1val, 2);
}
