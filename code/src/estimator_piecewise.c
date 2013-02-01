#include "estimator.h"

double extend_estimate(char *event_file, est_data *interval_estimate, double start_time,
		       double max_extension, double subinterval_time, double pmf_threshold);
double* interval_pmf(int *bin_counts, double *midpoints, int len, double a, double b);
int pmf_check(double *midpoints, int *bindata, double a, double b, int num_subintervals,
	      double threshold);
int pmf_threshold_check(double *pmfs, int len, double threshold);
int pmf_cumulative_check(double *pmfs, int len, int limit, double threshold);
int pmf_consecutive_check(double *pmfs, int len, int limit, double threshold);



/* int main(int argc, char *argv[]) */
/* { */
/*     double **piece = piecewise_estimate(argv[1], argv[2], 0, 100, 5, 3, 10, 15); */
    
/*     free_pointer_arr((void**)piece, 5); */
                
/*     return 0; */
/* } */

/*
 * Helper function for the piecewise estimator. Takes parameters out of a parameter list and 
 * passes them to the estimator.
 */
est_arr* estimate_piecewise(paramlist* params, char *event_file, char *output_file)
{
    int subint = get_int_param(params, "pc_iwls_subintervals");
    int iterations = get_int_param(params, "pc_iwls_iterations");
    int breakpoints = get_int_param(params, "pc_max_breakpoints");
    double max_extension = get_double_param(params, "pc_max_extension");
    double start = get_double_param(params, "start_time");
    double end = get_double_param(params, "interval_time") + start;
    double min_interval_proportion = get_double_param(params, "pc_min_interval_proportion");
    double pmf_threshold = get_double_param(params, "pc_pmf_threshold");
    double pmf_sum_threshold = get_double_param(params, "pc_pmf_sum_threshold");
    
    return _estimate_piecewise(event_file, output_file, start, end, iterations, 
			       subint, breakpoints, max_extension, min_interval_proportion,
			       pmf_threshold, pmf_sum_threshold);
}

/*
 * Estimate the given data in a piecewise manner. Data is broken up into a 
 * number of subintervals, and each is estimated using the IWLS estimator.
 * These estimates are used as the estimates for each of the subintervals,
 * and the estimate for the whole interval is made up of these subinterval
 * estimates.
 */
est_arr* _estimate_piecewise(char* event_file, char* output_file,
			     double interval_start, double interval_end,
			     int IWLS_iterations, int IWLS_subintervals, 
			     int max_breakpoints, double max_extension,
			     double min_interval_proportion, double pmf_threshold,
			     double pmf_sum_threshold)
{
    if (!interval_valid(interval_start, interval_end)){
	printf("Interval [%lf, %lf] is invalid.\n", interval_start, interval_end);
	return NULL;
    }
    
    int i = 0;
    double default_interval_length;
     // if there are no breakpoints, we work on the whole interval
    if (max_breakpoints == 0)
	default_interval_length = interval_end - interval_start;
    else 
	default_interval_length = (interval_end - interval_start) / max_breakpoints;
    // start time and end time of the subinterval
    double start_time = interval_start;
    double end_time = interval_start + default_interval_length;
    
    // calloc so we don't get uninitialised value errors
    est_data** interval_data = calloc(max_breakpoints + 1, sizeof(est_data*)); 
    est_arr* interval_estimate_array;
    est_data* interval_estimate;
        
    // We want to do this at least once, specifically if max_breakpoints is zero
    do {
	printf("Estimating interval [%lf, %lf].\n", start_time, end_time);
	// If the next interval is too short, make the end time of this interval
	// the end of the overall interval.
	if (interval_end - end_time  < (interval_end-interval_start) * min_interval_proportion){
	    printf("INTERVAL TOO SHORT - RE-ESTIMATING NEW INTERVAL\n");
	    end_time = interval_end;
	    printf("Estimating interval [%lf, %lf].\n", start_time, end_time);
	}

	/* if (interval_end - end_time  < (interval_end-interval_start)*min_interval_proportion){ */
	/*     printf("INTERVAL TOO SHORT - ADDING TO PREVIOUS\n"); */
	/*     interval_data[i-1]->end = end_time; */
	/*     printf("Interval %d is now [%lf, %lf]\n", i-1, interval_data[i-1]->start, interval_data[i-1]->end); */
	/* } */

	interval_estimate_array = _estimate_IWLS(event_file, NULL, start_time, end_time,
					   IWLS_subintervals, IWLS_iterations);
	interval_estimate = interval_estimate_array->estimates[0];
	printf("%lf, %lf, %lf, %lf\n", interval_estimate->start, interval_estimate->end, interval_estimate->est_a, interval_estimate->est_b);
	/* 
	 * Don't bother extending the line if we are at the end of the overall interval,
	 * or if we are estimating the last line segment.
	 */
	if (end_time < interval_end || i < max_breakpoints - 1){
	    // Do not allow the extension time to go over the overall end time
	    double extension_time = max_extension > interval_end - end_time ?interval_end - end_time : max_extension;

	    if (extension_time > 0)
		end_time = extend_estimate(event_file, interval_estimate, end_time,
					   extension_time, 1, pmf_threshold);
	}

	// Put this in a struct
	est_data* this_interval = malloc(sizeof(est_data));
	this_interval->est_a = interval_estimate->est_a; // a estimate
	this_interval->est_b = interval_estimate->est_b; // b estimate
	this_interval->start = start_time;
	this_interval->end = end_time;

	interval_data[i] = this_interval;
		
	++i;
	start_time = end_time; // The start of the next interval is the end of the current
	end_time = start_time + default_interval_length > interval_end ? interval_end : start_time + default_interval_length;
	free_est_arr(interval_estimate_array);
    } while(i <= max_breakpoints && start_time != end_time);

    if (output_file != NULL)
	output_estimates(output_file, interval_data, i);

    est_arr* results = malloc(sizeof(est_arr));
    results->len = i;
    results->estimates = interval_data;
    
    return results;
}

/*
 * Check whether the provided estimates for a and b (contained within the interval estimate) 
 * can also be used as good estimators for a short interval following the end of the 
 * interval estimated with IWLS. Returns the end time of the extended interval if it
 * is possible to extend the line, otherwise returns start_time.
 */
double extend_estimate(char *event_file, est_data *interval_estimate, double start_time, 
		       double max_extension, double subinterval_time, double pmf_threshold)
{
    int i;
    double retval = start_time;
        
    double *events = get_event_data_interval(start_time, start_time + max_extension,
					     event_file);
    double *lastevents = NULL;
        
    for (i = 0; i < 5; ++i) {
	double end_time = start_time + max_extension / (i + 1);	

	printf("checking interval [%lf, %lf]\n", start_time, end_time);

	if (lastevents != NULL){
	    events = get_event_subinterval(lastevents, start_time, end_time);
	    free(lastevents);
	}

	int event_num = events[0] - 1;

	if (event_num == 0 || end_time - start_time < subinterval_time)
	    break;
		
	int num_subintervals = (end_time - start_time)/subinterval_time;

	int *bin_counts = sum_events_in_interval((events + 1), event_num, 
						 start_time, end_time, 
						 num_subintervals);
	double *midpoints = get_interval_midpoints(start_time, end_time,
						   num_subintervals);
    
	int res = pmf_check(midpoints, bin_counts, interval_estimate->est_a, 
			    interval_estimate->est_b, num_subintervals, pmf_threshold);

	free(bin_counts);
	free(midpoints);
		
	if (res){
	    retval = end_time;
	    break;
	}

	printf("Could not extend estimate to end time %lf\n", end_time);
	lastevents = events;
    }

    free(events);
        
    return retval;
}

/*
 * Performs a check on the given interval to see how well values estimated for a
 * previous time interval match. This is done by calculating the poisson 
 * probability mass function for each bin count which corresponds to a time interval.
 * The lambda value for the middle of that time interval is determined, and then this
 * value of lambda is used in the PMF calculation.
 */
int pmf_check(double *midpoints, int *bindata, double a, double b, int num_subintervals,
	      double threshold)
{
    double *pmfs = interval_pmf(bindata, midpoints, num_subintervals, a, b);

    double sum = sum_double_arr(pmfs, num_subintervals);
    printf("Cumulative probability: %lf\n", sum);
    printf("Mean probability: %lf\n", sum/num_subintervals);

    int ret = pmf_threshold_check(pmfs, num_subintervals, threshold);

    /* int ret = pmf_sum_check(pmfs, num_subintervals, sum_threshold); */
    /* int ret = pmf_cumulative_check(pmfs, num_subintervals, 3, sum_threshold); */
    /* int ret = pmf_consecutive_check(pmfs, num_subintervals, 3, threshold); */

    free(pmfs);
    
    return ret;
}

/*
 * Performs a threshold check on the probability mass functions. If one
 * of the values exceeds the threshold, returns 0.
 */
int pmf_threshold_check(double *pmfs, int len, double threshold)
{
    int i;
    int ret = 1;
    
    for (i = 0; i < len; ++i) {
	if (pmfs[i] < threshold){
	    ret = 0;
	    break;
	}
    }

    return ret;
}

/*
 * Performs a check on the sum of the given probability mass functions.
 * If the sum exceeds the threshold, 0 is returned.
 */
int pmf_sum_check(double *pmfs, int len, double threshold)
{
    int i;
    int sum = 0;
        
    for (i = 0; i < len; ++i) {
	sum += pmfs[i];
	if (sum > threshold)
	    return 0;
    }

    return 1;
}

/*
 * Performs a check on the number of probability mass functions which exceed
 * the threshold. If the number of functions exceeding the threshold goes
 * over the limit, 0 is returned.
 */
int pmf_cumulative_check(double *pmfs, int len, int limit, double threshold)
{
    int i;
    int count = 0;
        
    for (i = 0; i < len; ++i) {
	if (pmfs[i] < threshold){
	    count++;
	    if (count > limit)
		return 0;
	}
    }

    return 1;
}

/*
 * Performs a check on the number of consecutive probability functions which exceed
 * the threshold. For example, if there are 4 consecutive values in the array
 * which exceed the threshold, and the limit is 3, then 0 is returned.
 */
int pmf_consecutive_check(double *pmfs, int len, int limit, double threshold)

{
    int i;
    int ret;
    int count = 0;
        
    for (i = 0; (ret = count <= limit) && i < len; ++i) {
	if (pmfs[i] < threshold){
	    count++;
	} else {
	    count = 0;
	}
    }

    return count <= limit;
}

/*
 * Calculates the poisson probability mass function for each bin count.
 * This only works for functions which are of the form 
 * lambda = a + b * x
 * The probabilities in the array returned by this function do not sum
 * to one, because each subinterval has a PMF of its own, and we are taking
 * a single value out of each of these PMFs.
 */
double* interval_pmf(int *counts, double *midpoints, int len, double a, double b)
{
    int i;
    double *pmf = malloc(len * sizeof(double));
        
    for (i = 0; i < len; ++i) {
	double lambda = a + b * midpoints[i];
	// lambda can't go below zero
	lambda = lambda < 0 ? 0 : lambda;
		
	pmf[i] = gsl_ran_poisson_pdf(counts[i], lambda);
	printf("time: %lf, lambda: %lf, bin count %d, pmf %lf\n", midpoints[i],
	       lambda, counts[i], pmf[i]);
    }

    return pmf;
}
