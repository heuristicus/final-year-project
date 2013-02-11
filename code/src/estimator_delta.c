#include "estimator.h"
#include "combinefunction.h"

/*
 * Estimates the delay of one function with relation to another by calculating
 * the probability density function at points with a certain distance between them.
 * The minimum of the sum of these probability density functions is taken as the
 * best estimate of the delay. The max_delay parameter specifies the maximum
 * positive or negative delay to consider when attempting to find an estimate.
 * The resolution specifies the step between points on the x-axis which are
 * compared. A high resolution will take longer, but the estimate will be more
 * accurate.
 */
double estimate_delay_pmf(void* f1, void* f2, double_arr* events, double max_delay,
			  double resolution, double step, char* type)
{
    // guess at a time delay
    double best_guess = -INFINITY;
    double best_value = -INFINITY;

    double start_delta = -max_delay, end_delta = max_delay;
//    double start_delta = 0;
    double current_delta = start_delta;
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

    int num_streams = 2;
    double combine_step = 1;
    double combine_start = 0;
    double combine_interval = 100; // should vary according to the delay?
    double combine_end = combine_start + combine_interval;
    double normaliser_start = 1;
    double normaliser_end = 20;
    double normaliser_step = 1;
    int normaliser_subintervals = 100;
    int bin_subintervals = 10;
    double bin_length = (combine_end - combine_start)/bin_subintervals;
    double_arr* time_delay = init_double_arr(2);
    time_delay->data[0] = 0;
    double_multi_arr* combined = NULL;

    double normaliser = find_normaliser(f1, events, combine_start, combine_end,
					normaliser_start, normaliser_end,
					normaliser_step, normaliser_subintervals,
					type);
    printf("Normaliser is %lf\n", normaliser);
    
    int* bin_counts = sum_events_in_interval(events->data, events->len, combine_start,
					     combine_end, bin_subintervals);
    double* midpoints = get_interval_midpoints(combine_start, combine_end,
					       bin_subintervals);
    double* lambda_sums = malloc(sizeof(double) * bin_subintervals);
    
    FILE *fp = fopen("bins", "w");
    int i;
    for (i = 0; i < bin_subintervals; ++i) {
	//	printf("bin %d [%d, %d] has %d events\n", i, i * bin_length, (i + 1) * bin_length, bin_counts[i]);
	fprintf(fp, "%lf %d\n", midpoints[i], bin_counts[i]);
    }

    fclose(fp);

    FILE *fp2 = fopen("pmf_delay_vals", "w");

    while (current_delta <= end_delta){
	// get the combined function with the delay applied
	// compare the combined function to the bin counts of the stream by calculating the pmf for each interval
	// to do this, first get the bin counts. These will remain constant for the function.
	// then, calculate the interval times for each bin, which will be the same for each
	// sum the values of lambda within this interval at each time. This will be one lambda per second, probably,
	// since each lambda represents the number of arrivals per single time step
	// finally, find the poisson pmf of the two values - gsl_ran_poisson_pdf(bin count, lambda sum for bin)
	printf("current delta is %lf\n", current_delta);
	time_delay->data[1] = current_delta;
	if (strcmp(type, "gauss") == 0){
      	    combined = combine_gauss_vectors((gauss_vector**)store, time_delay,
					     combine_start, combine_end, combine_step,
					     num_streams);
    	} else if (strcmp(type, "base") == 0){
    	    combined = combine_functions((est_arr**)store, time_delay,
					 combine_interval, combine_step, num_streams);
	/* if (strcmp(type, "gauss") == 0){ */
      	/*     combined = combine_gauss_vectors_all((gauss_vector**)store, time_delay, */
	/* 				     combine_start, combine_end, combine_step, */
	/* 				     num_streams); */
    	/* } else if (strcmp(type, "base") == 0){ */
    	/*     combined = combine_functions_all((est_arr**)store, time_delay, combine_start, */
	/* 				 combine_interval, combine_step, num_streams); */
    	} else {
	    printf("Unknown type for estimate \"%s\" when estimating delta.\n", type);
	    exit(1);
	}

	for (i = 0; i < bin_subintervals; ++i) {
	    // find the sum of lambda values for each subinterval
//	    printf("bin count %d: %d\n", i, bin_counts[i]);
//	    printf("subinterval start %lf, subinterval end %lf\n", i * bin_length, (i + 1) * bin_length);
	    lambda_sums[i] = sum_array_interval(combined->data[0], combined->data[1],
					     i * bin_length, (i + 1) * bin_length,
					     normaliser, combined->lengths[0]);
//	    printf("sum of lambdas in interval %lf\n", lambda_sums[i]);
	}

	FILE *fp1 = fopen("lsums", "w");
	
	for (i = 0; i < bin_subintervals; ++i) {
	    fprintf(fp1, "%lf %lf\n", midpoints[i], lambda_sums[i]);
	}
	fclose(fp1);
	
	int skip_bins = (int) current_delta/bin_length;

	printf("number of bins that need to be skipped %d\n", skip_bins);

	double total = sum_log_pmfs(bin_counts + skip_bins, lambda_sums + skip_bins, 1, bin_subintervals - 2 * skip_bins);
	fprintf(fp2, "%lf %lf\n", current_delta, total);
	printf("pmf sum is %lf\n", total);
	if (total > best_value){
	    best_value = total;
	    best_guess = current_delta;
	}

	printf("best guess is %lf at time %lf\n\n", best_value, best_guess);

	current_delta += step;
    }
    
    fclose(fp2);

    output_double_multi_arr("pmf_combined", "w", combined);

    return best_guess;
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
 * intervals.
 */
double find_normaliser(void* f1, double_arr* events, double interval_start,
		       double interval_end, double check_start, double check_limit,
		       double step, int subintervals, char* type)
{
    double normaliser = check_start;
    double best = -INFINITY;
    double best_normaliser = -1;
    
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
	//printf("Sum with normaliser %lf: %lf, current best %lf (%lf).\n", normaliser, pmf_sum, best, best_normaliser);
	if (pmf_sum > best) {
	    best = pmf_sum;
	    best_normaliser = normaliser;
	    //printf("New best normaliser is %lf with a sum of %lf\n", best_normaliser, best);
	}
	//	printf("total pmf is %lf, normaliser is %lf\n", pmf_sum, normaliser);
	normaliser += step;
    }

    
    return best_normaliser;
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
double estimate_delay_area(void* f1, void* f2, double max_delay, double resolution,
			   double step, char* type)
{
    double best_guess = 0;
    double best_value = INFINITY;
    double guess;

    double start = -max_delay, end = max_delay;
    double current = start;
    
    FILE *fp = fopen("delaycheck", "w");

    while (current <= end){
	if (strcmp(type, "gauss") == 0){
	    // put these variables into paramfile
	    guess = total_area_estimate((void*)f1, (void*)f2, 0, 100, resolution, current, "gauss");
	} else if (strcmp(type, "base") == 0){
	    guess = total_area_estimate((void*)f1, (void*)f2, 0, 100, resolution, current, "base");
	}
	fprintf(fp, "%lf %lf\n", current, guess);
	if (guess < best_value){
	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current);
	    best_value = guess;
	    best_guess = current;
	} else {
	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess);
	}
	current += step;
    }

    fclose(fp);

    return -best_guess;
}



/*
 * Estimates the total area between the curves f1 and f2 in the interval
 * [start, end], with samples taken at the given resolution and the given
 * delay applied to f2. The higher the resolution given, the higher better the
 * estimate will be, but the computation will also take longer.
 */
double total_area_estimate(void* f1, void* f2, double start,double end,
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
    double f2sum = sum_gaussians_at_point(x + delta, f2);

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
    double f2val = estimate_at_point(f2, x + delay);

    return pow(f2val - f1val, 2);
}

/* /\* */
/*  * Estimates the time delay of one function with relation to another by finding */
/*  *  the minimum value of the area between their curves. This is done in a  */
/*  * discrete manner by guessing a time delay and calculating the value, */
/*  * and continuing to make guesses until the resulting area goes below */
/*  * a threshold value. The max_delay parameter specifies the maximum */
/*  * positive or negative delay to consider when attempting to find an estimate. */
/*  * The resolution specifies the step between points on the x-axis which are */
/*  * compared. A high resolution will take longer, but the estimate will be more */
/*  * accurate. The returned value is the estimate of how much the second function */
/*  * is shifted. The value calculated by the function is inverted upon return */
/*  * to give the amount that f2 must be shifted to line it up with f1. */
/*  *\/ */
/* double estimate_delay_area_gauss(gauss_vector* f1, gauss_vector* f2,  */
/* 				 double max_delay, double resolution) */
/* { */
/*     double best_guess = 0; */
/*     double best_value = INFINITY; */
/*     double guess; */

/*     double start = -max_delay, end = max_delay; */
/*     double current = start; */
    
/*     FILE *fp = fopen("delaycheck", "w"); */

/*     while (current <= end){ */
/* 	printf("shifting by %lf\n", current); */
/* 	guess = total_area_estimate_gauss(f1, f2, 0, 100, resolution, current); */
/* 	fprintf(fp, "%lf %lf\n", current, guess); */
/* 	if (guess < best_value){ */
/* 	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current); */
/* 	    best_value = guess; */
/* 	    best_guess = current; */
/* 	} else { */
/* 	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess); */
/* 	} */
/* 	current += 1; // change this to a paramfile value */
/*     } */

/*     fclose(fp); */

/*     return -best_guess; */
/* } */

/* double estimate_delay_area_baseline(est_arr* f1, est_arr* f2, */
/* 				    double max_delay, double resolution) */
/* { */
/*     double best_guess = 0; */
/*     double best_value = INFINITY; */
/*     double guess; */

/*     double start = -max_delay, end = max_delay; */
/*     double current = start; */
    
/*     FILE *fp = fopen("delaycheck", "w"); */

/*     while (current <= end){ */
/* 	guess = total_area_estimate_base(f1, f2, 0, 100, resolution, current); */
/* 	fprintf(fp, "%lf %lf\n", current, guess); */
/* 	if (guess < best_value){ */
/* 	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current); */
/* 	    best_value = guess; */
/* 	    best_guess = current; */
/* 	} else { */
/* 	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess); */
/* 	} */
/* 	current += 1; // change this to a paramfile value */
/*     } */

/*     fclose(fp); */

/*     return -best_guess; */
/* } */

/* double total_area_estimate_base(est_arr* f1, est_arr* f2, double start, */
/* 				double end, double resolution, double delay) */
/* { */
/*     if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0) */
/* 	return -1; */

/*     double current = start; */
/*     printf("start %lf end %lf delay %lf interval overlap %lf\n", start, end, delay, end - start - fabs(delay)); */
/*     double sum = 0; */
/*     int i = 0; */

/*     while(current <= end){ */
/* 	double pt = area_at_point_base(f1, f2, current, delay); */
/* 	if (pt != -1){ */
/* 	    sum += pt; */
/* 	    ++i; */
/* 	} */
/* 	current += resolution; */
/*     } */

/*     return sum/i; */
/* } */


/*  /\* */
/*   * Estimates the total area between the curves f1 and f2 in the interval */
/*   * [start, end], with samples taken at the given resolution and the given */
/*   * delay applied to f2. The higher the resolution given, the higher better the */
/*   * estimate will be, but the computation will also take longer. */
/*   *\/ */
/* double total_area_estimate_gauss(gauss_vector* f1, gauss_vector* f2, double start, double end, */
/* 		   double resolution, double delay) */
/*  { */
/*      if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0) */
/* 	 return -1; */

/*      double current = start; */
/*      double sum = 0; */
/*      int i = 0; */

/*      while(current <= end){ */
/* 	 double pt = area_at_point_gauss(f1, f2, current, delay); */
/* 	 if (pt != -1){ */
/* 	     sum += pt; */
/* 	     ++i; */
/* 	 } */
/* 	current += resolution; */
/*     } */

/*     return sum/i; */
/* } */


/* /\* */
/*  * Estimates the time delay of one function with relation to another by finding */
/*  *  the minimum value of the area between their curves. This is done in a  */
/*  * discrete manner by guessing a time delay and calculating the value, */
/*  * and continuing to make guesses until the resulting area goes below */
/*  * a threshold value. The max_delay parameter specifies the maximum */
/*  * positive or negative delay to consider when attempting to find an estimate. */
/*  *\/ */
/* double estimate_delay_discrete_area(double_multi_arr* f1, double_multi_arr* f2, */
/* 				    double max_delay) */
/* { */
/*     double best_guess = 0; */
/*     double best_value = -INFINITY; */
/*     double guess; */

/*     double start = -max_delay, end = max_delay; */
/*     double current = start; */
    
/*     while (current <= end){ */
/* 	printf("shifting by %lf\n", current); */
/* 	guess = total_area_estimate(f1, f2, current); */
/* 	if (guess > best_value){ */
/* 	    best_value = guess; */
/* 	    best_guess = current; */
/* 	} */
/* 	current += 5; // change this to a paramfile value */
/*     } */

/*     return best_guess; */
/* } */


/* /\* */
/*  * Estimates the total area between the curves f1 and f2 in the interval */
/*  * [start, end], with the given delay applied to f2. The given double arrays */
/*  * are assumed to contain the values of the two functions at the same points */
/*  * on the x-axis. The area is estimated by summing the differences between the */
/*  * two function values for each point. The calculation is only done for points */
/*  * at which both functions have values. */
/*  *\/ */
/* double total_area_estimate(double_multi_arr* f1, double_multi_arr* f2, double delay) */
/* { */
/*     if (f1 == NULL || f2 == NULL) */
/* 	return -1; */
    
/*     double sum = 0; */

/*     double* shift = add_to_arr(f2->data[0], f2->lengths[0], delay); */

/*     int i; */
    
/*     for (i = 0; i < f2->lengths[0]; ++i) { */
/* 	printf("f1 data %lf, f2 data %lf\n", f1->data[0][i], shift[i]); */
/*     } */

/*     return sum/i; */
/* } */


    /* double_arr* time_delay = malloc(sizeof(double_arr)); */
    /* double_multi_arr* combined = NULL; */
    /* time_delay->len = 1; */
    /* time_delay->data = malloc(sizeof(double)); */
    
    /* FILE *fp = fopen("delaycheck_pmf", "w"); */
    /* printf("events length %d\n", events->len); */
    /* double ev_start = 0; */
    /* double interval_time = 100; */
    /* double ev_end = ev_start + interval_time; */

    /* printf("ev end is %lf\n", ev_end); */
    /* int intervals = 100; */
    /* int* event_sum = sum_events_in_interval(events->data, events->len, ev_start, ev_end, intervals); // put these params in paramfile */
    /* double* midpoints = get_interval_midpoints(ev_start, ev_end, intervals); // these should be the same as above */
    /* FILE *fp1 = fopen("counts", "w"); */

    /* int i; */
    
    /* for (i = 0; i < intervals; ++i) { */
    /* 	fprintf(fp1, "%lf %d\n", midpoints[i], event_sum[i]); */
    /* } */

    /* fclose(fp1); */

    /* while (current <= end){ */
    /* 	time_delay->data[0] = current; */

    /* 	// need to calculate only times at which the data intersects? */
    /* 	if (strcmp(type, "gauss") == 0){ */
    /* 	    printf("gass\n"); */
    /* 	    combined = combine_gauss_vectors((gauss_vector**)store, time_delay, 0, 100, 5, 2); */
    /* 	} else if (strcmp(type, "base") == 0){ */
    /* 	    printf("base\n"); */
    /* 	    combined = combine_functions((est_arr**)store, time_delay, 100, 5, 2); */
    /* 	} */
    /* 	combined->data[1] = multiply_arr(combined->data[1], combined->lengths[1], intervals/interval_time); */
    /* 	guess = sum_log_pmfs(event_sum, combined->data[1], combined->lengths[1]); */
    /* 	// compare combined with the estimate bins */
    /* 	fprintf(fp, "%lf %lf\n", current, guess); */
    /* 	if (guess < best_value){ */
    /* 	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current); */
    /* 	    best_value = guess; */
    /* 	    best_guess = current; */
    /* 	} else { */
    /* 	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess); */
    /* 	} */
    /* 	current += step; */
    /* } */

    /* fclose(fp); */

    /* return -best_guess; */

    /* return 0; */
