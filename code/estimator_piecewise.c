#include "estimator.h"
#include "file_util.h"
#include "math_util.h"
#include "general_util.h"
#include <string.h>

#define INTERVAL_EPSILON 0.5
#define MIN_INTERVAL_LENGTH_PROPORTION 0.1
#define PMF_INSTANCE_THRESHOLD 0.02
#define PMF_SUM_THRESHOLD 0.85

int check_next_time(char *infile, double *estimate, double start_time, double limit);
void estimate_to_file(char *filename, double *estimate, char *mode);
void output_estimates(char *filename, double **estimates, int len);
void piecewise_estimate(char *infile, char *outfile, double start_time, double end_time, int break_points);
double* interval_pmf(int *bin_counts, double *midpoints, int len, double a, double b);
int pmf_check(char *infile, double start_time, double end_time, double a, double b, int num_subintervals);
int pmf_threshold_check(double *pmfs, int len);
int pmf_cumulative_check(double *pmfs, int len, int limit);
int pmf_consecutive_check(double *pmfs, int len, int limit);

void piecewise_estimate(char *infile, char *outfile, double start_time, double end_time, int break_points)
{
    
    char *out_separate = malloc(strlen(outfile) + 5); // separate outfiles for each piece
    double **interval_estimates = malloc((break_points + 1) * sizeof(double*)); // estimates for each piece
    interval_estimates[0] = calloc(1, sizeof(double)); // we will store the size of the array in the first memory location
    interval_estimates += 1;
    
    int i, j;
    double interval_time = end_time - start_time;
    double step = interval_time/break_points;
    //double min_length = interval_time * MIN_INTERVAL_LENGTH_PROPORTION;
        
    double **estimates = malloc(break_points * sizeof(double*));

    double check_forwards = 15; // time to look forwards and see if we can use the same parameters
                
    double interval_start = start_time;
    double interval_end = 0;
    double prev_interval_end = 0;

    FILE *fp = fopen("spline.gnu", "w");

    for (i = 0; i < break_points && prev_interval_end <= end_time; ++i) {
	int accept_estimate = 0;
	interval_start = start_time + prev_interval_end;
	interval_end = interval_start + step;
	
	// Stop the end interval exceeding the total time of the data.
	interval_end = interval_end > end_time ? end_time : interval_end;
	if (i - break_points - 1 == 0) // If this is the last interval, end it at the total time.
	    interval_end = interval_time;
			
	if (abs(interval_start - interval_end) < INTERVAL_EPSILON){
	    
	    printf("Interval start and end are equal\n");
	    break;
	}
	
	sprintf(out_separate, "%s_%d", outfile, i);
	printf("--------RUN %d STARTING:--------\n", i);
	printf("interval start: %lf, interval end: %lf\n", interval_start, interval_end);
	estimates[i] = estimate_IWLS(infile, out_separate, interval_start, interval_end, 10, 3);
	/* if (i > interval_estimates_size) { */
	/*     // Play with the pointer so that the reallocation works. */
	/*     interval_estimates -= 1; */
	/*     interval_estimates = realloc(interval_estimates, (interval_estimates_size *= 2) * sizeof(double*)); */
	/*     interval_estimates += 1; */
	/* } */

	// Save data for this run
	interval_estimates[i] = malloc(4 * sizeof(double));
	interval_estimates[i][0] = estimates[i][0];
	interval_estimates[i][1] = estimates[i][1];
	interval_estimates[i][2] = interval_start;

	if (interval_start <= end_time && interval_end < end_time) { // don't try and estimate past the end of the data that we have
	    printf("Checking if the estimates can be extended for the next time period.\n\n");
	    for (j = 1; j < 4; ++j) {
		printf("Checking estimates against estimate for time period [%lf, %lf]\n", interval_end, interval_end + check_forwards/j);
		// Check if we can use the estimate we got above to estimate the next lot of data.
		// If we can't estimate a long interval, try with a few shorter intervals.
		//accept_estimate = check_next_time(infile, estimates[i], interval_end, check_forwards/j);
		accept_estimate = pmf_check(infile, interval_end, interval_end + check_forwards/j, interval_estimates[i][0], interval_estimates[i][1], check_forwards/j);
		if (accept_estimate == 1){
		    printf("Estimates for next time period are similar. Extending the line.\n");
		    break;
		} else {
		    printf("Estimates are not similar enough.\n\n");
		}
	    }
	}

	if (accept_estimate) {
	    interval_estimates[i][3] = interval_end + check_forwards/j;
	} else { // can't estimate next block using the current estimates
	    interval_estimates[i][3] = interval_end;
	}
	printf("Estimates for interval [%lf, %lf]: a = %lf, b = %lf\n\n\n", interval_start, interval_end, interval_estimates[i][0], interval_estimates[i][1]);
	prev_interval_end = interval_estimates[i][3];

	
	/* double mid = interval_estimates[i][2] + (1.0/2.0) * (interval_estimates[i][3] - interval_estimates[i][2]); */
	
	/* fprintf(fp, "%lf %lf\n", mid, interval_estimates[i][0] + interval_estimates[i][1] * mid); */
	
	
	
//	printf("updating interval estimate count\n");
	**(interval_estimates - 1) += 1; // store the size of the array before the data
    }

    fclose(fp);
    
    for (i = 0; i < **(interval_estimates - 1); ++i) {
	printf("Interval start: %lf, interval end: %lf, a estimate: %lf, b estimate: %lf\n", interval_estimates[i][2], interval_estimates[i][3], interval_estimates[i][0], interval_estimates[i][1]);
    }

    output_estimates(outfile, interval_estimates, **(interval_estimates - 1));
    
    free(out_separate);
    interval_estimates -= 1;
    // We may end up with fewer estimates than break points due to extension
    free_pointer_arr((void**)estimates, (**interval_estimates));
    free_pointer_arr((void**)interval_estimates, (**interval_estimates) + 1); // free everything 
    
}

int main(int argc, char *argv[])
{

    piecewise_estimate(argv[1], argv[2], 0, 100, 5);
        
    return 0;
}

/*
 * Performs a check on the given interval to see how well values estimated for a previous time interval
 * match. This is done by calculating the poisson probability mass function for each bin count which corresponds
 * to a time interval. The lambda value for the middle of that time interval is determined, and then this
 * value of lambda is used in the PMF calculation.
 */
int pmf_check(char *infile, double start_time, double end_time, double a, double b, int num_subintervals)
{
    
    double *evdata = get_event_data_interval(start_time, end_time, infile);
    int num_events = evdata[0] - 1;
    evdata += 1;
        
    int *bindata = sum_events_in_interval(evdata, num_events, start_time, end_time, num_subintervals);
    
    double *midpoints = get_interval_midpoints(start_time, end_time, num_subintervals);
    
    double *pmfs = interval_pmf(bindata, midpoints, num_subintervals, a, b);

    double sum = sum_double_arr(pmfs, num_subintervals);
    printf("Cumulative probability: %lf\n", sum);
    printf("Mean probability: %lf\n", sum/num_subintervals);

    int ret = pmf_threshold_check(pmfs, num_subintervals);
    /* int ret = pmf_sum_check(pmfs, num_subintervals); */
    /* int ret = pmf_cumulative_check(pmfs, num_subintervals, 3); */
    /* int ret = pmf_consecutive_check(pmfs, num_subintervals, 3); */
    
    free(pmfs);
    free(midpoints);
    free(bindata);
    evdata -= 1;
    free(evdata);
    
    return ret;
}

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

int pmf_sum_check(double *pmfs, int len)
{
    int i;
    int sum = 0;
        
    for (i = 0; i < len; ++i) {
	sum += pmfs[i];
    }

    return sum <= PMF_SUM_THRESHOLD;
}

int pmf_cumulative_check(double *pmfs, int len, int limit)
{
    int i;
    int count = 0;
        
    for (i = 0; i < len; ++i) {
	if (pmfs[i] < PMF_INSTANCE_THRESHOLD){
	    count++;
	}
    }

    return count <= limit;
}

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
	estimate_to_file(filename, estimates[i], i > 0 ? "a" : "w");
    }

}

/*
 * Prints a single set of estimates to an output file. This will calculate
 * the value of the function for each second within the interval.
 */
void estimate_to_file(char *filename, double *estimate, char *mode)
{
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
