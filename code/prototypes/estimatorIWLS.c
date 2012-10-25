#include "estimator.h"
#include "math_util.h"
#include "file_util.h"

/* double **intervals; */
/* double *weights; */
/* double *rand_var; */
/* double *lambda_estimate */
/* double alpha, beta, a, b, rand_var_mean, x_mean; */

int main(int argc, char *argv[])
{
    double time = 5.0;
    int subintervals = 2;
        
    double **intervals = get_subintervals(time, subintervals);
    double *weights = initialise_weights(subintervals);
    int *bin_counts = get_bin_counts(argv[1], time/subintervals, subintervals);
    
    int i;
    
    free_pointer_arr((void **) intervals, subintervals);
    free(bin_counts);
    free(weights);
    
    return 0;
}

/*
 * Gets the start, midpoint and end of each time interval, defined
 * by the total time and number of subintervals.
 * 
 * interval = (((k-1)*T)/N, kT/N]
 */
double** get_subintervals(double time, int subintervals)
{
    int i;
    double **subinterval = malloc(subintervals * sizeof(double*));

    for (i = 0; i < subintervals; ++i){
	subinterval[i] = malloc(3 * sizeof(double));
	subinterval[i][0] = (i * time)/subintervals;
	subinterval[i][1] = get_interval_midpoint(i + 1, time, subintervals);
	subinterval[i][2] = ((i + 1) * time) / subintervals;
    }

    return subinterval;
    
}

/*
 * Get the number of events that occurred in each subinterval.
 */
int* get_bin_counts(char *filename, double interval_time, int interval_num)
{
    double *events = get_event_data(filename);
    int num_events = (int) events[0] - 1;

    int *bin_counts = sum_events_in_interval(events + 1, num_events, interval_time, interval_num);
    
    free(events);
        
    return bin_counts;
    
}

/*
 * Get the midpoint of a specified interval. (midpoint(xk)=(k-1/2)*T/N), 1 <= k <= N
 */
double get_interval_midpoint(int interval_number, double time, int subintervals)
{
    return (interval_number - 1.0/2.0) * (time /subintervals);
}

/*
 * Initialise the weights assigned to each subinterval to 1. This is 
 * the simplest way of doing things.
 */
double* initialise_weights(int subintervals)
{
    double *weights = malloc(subintervals * sizeof(double));
    
    int i;
    
    for (i = 0; i < subintervals; ++i){
	weights[i] = 1;
    }
    
    return weights;
    
}

/*
 * Estimates the value of beta
 */ 
double estimate_beta()
{
    return 0.0;
    
}

/*
 * estimates the value of alpha
 */
double estimate_alpha()
{
    return 0.0;
}

/*
 * Calculates the mean of the subinterval midpoints, taking into consideration the
 * weight of each value.
 */
double mean_x(double *weights, double *bin_counts, int size)
{
    int i;
    double sum = 0.0;
    
    for (i = 0; i < size; ++i){
	sum += weights[i] * bin_counts[i];
    }

    return (1.0/size) * sum;
    
}

/*
 * Calculates the mean value of the random variables, given the current weights and 
 * values of alpha and beta.
 */
double mean_Y()
{
    return 0.0;
}

double SSE(double *rand_var, double *weights, double *bin_counts, double alpha, double beta, int size)
{
    double sum = 0.0;
    int i;
    
    for (i = 0; i < size; ++i){
	sum += weights[i] * pow((rand_var[i] - (alpha + beta * bin_counts[i])), 2);
    }

    return sum;
    
}
