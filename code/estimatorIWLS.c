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
    double time = 10;
    int subintervals = 10;
        
    double **intervals = get_subintervals(time, subintervals);
    double *weights = initialise_weights(subintervals);
    int *bin_counts = get_bin_counts(argv[1], time/subintervals, subintervals);
    double *random_variables = initialise_random_variables(bin_counts, subintervals);
    
    int i;
    
    for (i = 0; i < subintervals; ++i){
	printf("%lf - %lf: %d\n", intervals[i][0], intervals[i][2], bin_counts[i]);
	printf("Random variable %d is %lf\n", i, random_variables[i]);
    }

    double Y_mean = mean_Y(random_variables, weights, subintervals);
    double x_mean = mean_x(bin_counts, weights, subintervals);

    double est_beta = beta_estimate(weights, bin_counts, random_variables, x_mean, subintervals);
    double est_alpha = alpha_estimate(Y_mean, x_mean, est_beta);

    double sse = SSE(random_variables, weights, bin_counts, est_alpha, est_beta, subintervals);
    
    printf("mean x %lf\n", x_mean);
    printf("mean randvar %lf\n", Y_mean);
    
    printf("beta estimate: %lf\n", est_beta);
    printf("alpha estimate: %lf\n", est_alpha);
    
    printf("SSE: %lf\n", sse);
    
    free_pointer_arr((void **) intervals, subintervals);
    free(bin_counts);
    free(random_variables);
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
 * Initialises each random variable Yk to the event number in the corresponding subinterval
 * (should noise be added here?)
 */
double* initialise_random_variables(int *bin_counts, int length)
{
    double* rand_var = malloc(length * sizeof(double));
    
    int i;
        
    for (i = 0; i < length; ++i){
	rand_var[i] = bin_counts[i] + get_poisson_noise(bin_counts[i]);
    }

    return rand_var;
}

/*
 * Estimates the value of beta
 */ 
double beta_estimate(double* weights, int* bin_counts, double *rand_var, double mean_x, int subintervals)
{
    double xdiffsum = 0;
    double squarediffsum = 0;
    
    int i;
    
    for (i = 0; i < subintervals; ++i) {
	xdiffsum += weights[i] * (bin_counts[i] - mean_x) * rand_var[i];
	squarediffsum += weights[i] * pow(bin_counts[i] - mean_x, 2);
    }

    printf("xdiff %lf, sqdiff %lf\n", xdiffsum, squarediffsum);

    return xdiffsum/squarediffsum;
        
}

/*
 * estimates the value of alpha
 */
double alpha_estimate(double mean_y, double mean_x, double beta_estimate)
{
    return mean_y - beta_estimate * mean_x;
}

double weight_estimate()
{
    return 0.0;
}

double lambda_estimate()
{
    return 0.0;
}

/*
 * Calculates the mean of the subinterval midpoints, taking into consideration the
 * weight of each value.
 */
double mean_x(int *bin_counts, double *weights, int size)
{
    int i;
    double sum = 0.0;
    
    for (i = 0; i < size; ++i){
	sum += weights[i] * bin_counts[i];
    }

    return (1.0/size) * sum;
    
}

/*
 * Calculates the mean value of the random variables, given the current weights
 */
double mean_Y(double *random_variables, double *weights, int size)
{
    int i;
    double sum = 0;
    
    for (i = 0; i < size; ++i) {
	sum += weights[i] * random_variables[i];
    }
    
    return (1.0/size) * sum;
    
}

double SSE(double *rand_var, double *weights, int *bin_counts, double alpha, double beta, int size)
{
    double sum = 0.0;
    int i;
    
    for (i = 0; i < size; ++i){
	sum += weights[i] * pow((rand_var[i] - (alpha + beta * bin_counts[i])), 2);
    }

    return sum;
    
}
