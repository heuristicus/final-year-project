#include "estimator.h"
#include "file_util.h"
#include "math_util.h"
#include "general_util.h"
#include <string.h>

#define INTERVAL_EPSILON 0.5
#define MIN_INTERVAL_LENGTH_PROPORTION 0.1
#define PMF_INSTANCE_THRESHOLD 0.02
#define PMF_SUM_THRESHOLD 0.85

void estimate_to_file(char *filename, double *estimate, char *mode);
void output_estimates(char *filename, double **estimates, int len);
double** piecewise_estimate(char *event_file, char *output_file, double interval_start, 
			    double interval_end, double max_breakpoints, double IWLS_iterations, 
			    double IWLS_subintervals, double max_extension);
double extend_estimate(char *event_file, double *interval_estimate, double start_time, 
		       double max_extension, double subinterval_time);
double* interval_pmf(int *bin_counts, double *midpoints, int len, double a, double b);
int pmf_check(double *midpoints, int *bindata, double a, double b, int num_subintervals);
int pmf_threshold_check(double *pmfs, int len);
int pmf_cumulative_check(double *pmfs, int len, int limit);
int pmf_consecutive_check(double *pmfs, int len, int limit);


int main(int argc, char *argv[])
{
    double **piece = piecewise_estimate(argv[1], argv[2], 0, 100, 5, 3, 10, 15);
    
    free_pointer_arr((void**)piece, 5);
                
    return 0;
}

double** piecewise_estimate(char *event_file, char *output_file, double interval_start, 
			    double interval_end, double max_breakpoints, double IWLS_iterations, 
			    double IWLS_subintervals, double max_extension)
{
    if (!interval_check(interval_start, interval_end)){
	printf("Interval [%lf, %lf] is invalid.\n", interval_start, interval_end);
	return NULL;
    }
    
    int i = 0;
    double default_interval_length;
    if (max_breakpoints == 0) // if there are no breakpoints, we work on the whole interval
	default_interval_length = interval_end - interval_start;
    else 
	default_interval_length = (interval_end - interval_start) / max_breakpoints;

    double start_time = interval_start, end_time = interval_start + default_interval_length; // start time and end time of the subinterval
    
    double **interval_data = calloc(max_breakpoints, sizeof(double*));
    double *interval_estimate;
        
    // We want to do this at least once, specifically if max_breakpoints is zero
    do {
	printf("Estimating interval [%lf, %lf].\n", start_time, end_time);
	interval_estimate = estimate_IWLS(event_file, NULL, start_time, end_time, IWLS_subintervals, IWLS_iterations);
	
	// Don't bother extending the line if we are at the end of the overall interval,
	// or if we are estimating the last line segment.
	if (end_time < interval_end || i < max_breakpoints - 1){
	    double extension_time = max_extension > interval_end - end_time ? interval_end - end_time : max_extension;

	    if (extension_time > 0)
		end_time = extend_estimate(event_file, interval_estimate, end_time, extension_time, 1);
	}

	double *this_interval = malloc(4 * sizeof(double));
	this_interval[0] = interval_estimate[0]; // a estimate
	this_interval[1] = interval_estimate[1]; // b estimate
	this_interval[2] = start_time;
	this_interval[3] = end_time;

	interval_data[i] = this_interval;

	printf("Final interval is [%lf, %lf].\n", start_time, end_time);
				
	++i;
	start_time = end_time; // The start of the next interval is the end of the current
	end_time = start_time + default_interval_length > interval_end ? interval_end : start_time + default_interval_length;
	free(interval_estimate);
    } while(i < max_breakpoints && start_time != end_time);

    output_estimates(output_file, interval_data, i);
    
    return interval_data;
}

/*
 * Check whether the provided estimates for a and b (contained within the interval estimate) 
 * can also be used as good estimators for a short interval following the end of the 
 * interval estimated with IWLS. Returns the end time of the extended interval if it
 * is possible to extend the line, otherwise returns start_time.
 * 
 * *********NUM SUBINTERVALS should be relative - using the same number on each means
 * that the shorter the interval the fewer events there end up being in each interval.
 */
double extend_estimate(char *event_file, double *interval_estimate, double start_time, 
		       double max_extension, double subinterval_time)
{
    int i;
    double retval = start_time;
        
    double *events = get_event_data_interval(start_time, start_time + max_extension, event_file);
    double *lastevents = NULL;
        
    for (i = 0; i < 5; ++i) {
	double end_time = start_time + max_extension / (i + 1);	

	printf("checking interval [%lf, %lf]\n", start_time, end_time);

	if (lastevents != NULL){
	    events = get_event_subinterval(lastevents, start_time, end_time);
	    free(lastevents);
	}

	int event_num = events[0] - 1;
		
	int num_subintervals = (end_time - start_time)/subinterval_time;

	int *bin_counts = sum_events_in_interval((events + 1), event_num, start_time, end_time, num_subintervals);
	double *midpoints = get_interval_midpoints(start_time, end_time, num_subintervals);
    
	int res = pmf_check(midpoints, bin_counts, interval_estimate[0], interval_estimate[1], num_subintervals);

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
 * Performs a check on the given interval to see how well values estimated for a previous time interval
 * match. This is done by calculating the poisson probability mass function for each bin count which corresponds
 * to a time interval. The lambda value for the middle of that time interval is determined, and then this
 * value of lambda is used in the PMF calculation.
 */
int pmf_check(double *midpoints, int *bindata, double a, double b, int num_subintervals)
{
    double *pmfs = interval_pmf(bindata, midpoints, num_subintervals, a, b);

    double sum = sum_double_arr(pmfs, num_subintervals);
    printf("Cumulative probability: %lf\n", sum);
    printf("Mean probability: %lf\n", sum/num_subintervals);

    int ret = pmf_threshold_check(pmfs, num_subintervals);

    free(pmfs);
    
    return ret;
}

/*
 * Performs a threshold check on the probability mass functions. If one
 * of the values exceeds the threshold, returns 0.
 */
int pmf_threshold_check(double *pmfs, int len)
{
    int i;
    int ret = 1;
    
    for (i = 0; i < len; ++i) {
	if (pmfs[i] < PMF_INSTANCE_THRESHOLD){
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
int pmf_sum_check(double *pmfs, int len)
{
    int i;
    int sum = 0;
        
    for (i = 0; i < len; ++i) {
	sum += pmfs[i];
	if (sum > PMF_SUM_THRESHOLD)
	    return 0;
    }

    return 1;
}

/*
 * Performs a check on the number of probability mass functions which exceed
 * the threshold. If the number of functions exceeding the threshold goes
 * over the limit, 0 is returned.
 */
int pmf_cumulative_check(double *pmfs, int len, int limit)
{
    int i;
    int count = 0;
        
    for (i = 0; i < len; ++i) {
	if (pmfs[i] < PMF_INSTANCE_THRESHOLD){
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
int pmf_consecutive_check(double *pmfs, int len, int limit)

{
    int i;
    int ret;
    int count = 0;
        
    for (i = 0; (ret = count <= limit) && i < len; ++i) {
	if (pmfs[i] < PMF_INSTANCE_THRESHOLD){
	    count++;
	} else {
	    count = 0;
	}
    }

    return count <= limit;
}

/*
 * Calculates the poisson probability mass function for each bin count.
 * The function passed in should return the value of lambda at a given
 * point in time. This only works for functions which are of the form 
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
		
	pmf[i] = poisson_PMF(lambda, counts[i]);
	printf("time: %lf, lambda: %lf, bin count %d, pmf %lf\n", midpoints[i], lambda, counts[i], pmf[i]);
    }

    return pmf;
}

/*
 * Outputs a series of estimates to separate files. The filename provided
 * will have the interval number appended to it - the first will be
 * "filename_0"
 */
void output_estimates(char *filename, double **estimates, int len)
{
    int i;
    //char *f;
        
    for (i = 0; i < len; ++i) {
	//f = malloc(strlen(filename) + 4);
	//sprintf(f, "%s_%d", filename, i);
	//estimate_to_file(filename, estimates[i], "w");
	// Write to file if it's the first run, otherwise append
	if (estimates[i] == NULL){
	    printf("null, breaking %d\n", i);
	    break;
	}
	estimate_to_file(filename, estimates[i], i > 0 ? "a" : "w");
    }

}

/*
 * Prints a single set of estimates to an output file. This will calculate
 * the value of the function for each second within the interval.
 */
void estimate_to_file(char *filename, double *estimate, char *mode)
{

    if (estimate == NULL)
	return;
        
    double a = estimate[0];
    double b = estimate[1];
    double start = estimate[2];
    double end = estimate[3];
    
    // We will subtract from this value to make sure we cover the whole
    // length of the interval.
    double counter = start;
    
    printf("int end %lf, int start %lf\n", end, start);
    
    FILE *fp = fopen(filename, mode);
        
    while(counter < end){
	fprintf(fp, "%lf %lf\n", counter, a + counter * b);
#ifdef DEBUG
	printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
#endif
	if (end - counter <= 1){
	    counter += end - counter;
#ifdef DEBUG
	    printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
#endif
	    fprintf(fp, "%lf %lf\n", counter, a + counter * b);
	} else {
	    counter += 1;
	}
#ifdef DEBUG
	printf("counter: %lf, end %lf\n", counter, end);
#endif
    }

    fprintf(fp, "\n\n");
    
    fclose(fp);
    
}
