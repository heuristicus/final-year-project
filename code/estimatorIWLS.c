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
    double time = 100;
    int subintervals = 100;
        
    double **intervals = get_subintervals(time, subintervals);
    int *bin_counts = get_bin_counts(argv[1], time/subintervals, subintervals);

    double *weights = initialise_weights(subintervals);
    double *random_variables = initialise_random_variables(bin_counts, subintervals);
    
    int i, loop;
    
    for (i = 0; i < subintervals; ++i){
	printf("%lf - %lf: %d\n", intervals[i][0], intervals[i][2], bin_counts[i]);

    }

    double Y_mean, x_mean, est_beta, est_alpha, sse, a, b;
    
    for (loop = 0; loop < 5; ++loop){
	
	/* for (i = 0; i < subintervals; ++i) { */
	/*     printf("Random variable %d is %lf\n", i, random_variables[i]); */
	/* } */

	/* for (i = 0; i < subintervals; ++i) { */
	/*     printf("Weight %d is %lf\n", i, weights[i]); */
	/* } */

	Y_mean = mean_Y(random_variables, weights, subintervals);
	x_mean = mean_x(bin_counts, weights, subintervals);

	printf("mean x %lf\n", x_mean);
	printf("mean randvar %lf\n", Y_mean);


	est_beta = beta_estimate(weights, bin_counts, random_variables, x_mean, subintervals);
	est_alpha = alpha_estimate(Y_mean, x_mean, est_beta);

	printf("alpha estimate: %lf\n", est_alpha);
	printf("beta estimate: %lf\n", est_beta);

	sse = SSE(random_variables, weights, bin_counts, est_alpha, est_beta, subintervals);

	printf("SSE: %lf\n", sse);
    
	a = a_estimate(est_alpha, time, subintervals);
	b = b_estimate(est_beta, time, subintervals);

	printf("a estimate: %lf\nb estimate %lf\n", a, b);

	if (a < 0){
	    printf("Estimate for a is not positive (%lf) - setting a to 0 and recalculating b.\n", a);
	    a = 0;
	    b = constraint_a(weights, bin_counts, random_variables, time, subintervals);
	} else if (a > 0 && b < -a/time){
	    printf("Estimate for b is not positive (%lf) - setting a=-bT, b=N*beta/T\n", b);
	    a = -b * time;
	    b = constraint_b(weights, bin_counts, random_variables, time, subintervals);
	}

	lambda_estimate(random_variables, bin_counts, a, b, time, subintervals);
    
	/* for (i = 0; i < subintervals; ++i) { */
	/*     printf("New lambda estimate for random variable %d: %lf\n", i, random_variables[i]); */
	/* } */

	weight_estimate(weights, random_variables, subintervals);
    
    
	/* for (i = 0; i < subintervals; ++i) { */
	/*     printf("New weight %d is %lf\n", i, weights[i]); */
	/* } */


	sse = SSE(random_variables, weights, bin_counts, est_alpha, est_beta, subintervals);

	printf("new SSE: %lf\n\n\n\n", sse);
    
    }

    FILE *fp = fopen(argv[2], "w");
    
    
    for (i = 0; i < subintervals; ++i) {
	fprintf(fp, "%lf, %lf\n", intervals[i][1], a + b * intervals[i][1]);
    }

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
 * estimates the value of alpha
 */
double alpha_estimate(double mean_y, double mean_x, double beta_estimate)
{
    return mean_y - beta_estimate * mean_x;
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

double a_estimate(double alpha, double total_time, int intervals)
{
    return alpha * (total_time/intervals);
}

double b_estimate(double beta, double total_time, int intervals)
{
    return beta * (total_time/intervals);
}

double constraint_a(double *weights, int *bin_counts, double *random_variables, double time, int intervals)
{
    double rvsum = 0;
    double sqsum = 0;
    
    int i;
    
    for (i = 0; i < intervals; ++i) {
	rvsum = weights[i] * bin_counts[i] * random_variables[i];
	sqsum = weights[i] * pow(bin_counts[i], 2);
    }

    return (time/intervals) * (rvsum/sqsum);
    
}

double constraint_b(double *weights, int *bin_counts, double *random_variables, double time, int intervals)
{
    double rvsum = 0;
    double sqsum = 0;
    
    int i;
    
    for (i = 0; i < intervals; ++i) {
	rvsum = weights[i] * (time - bin_counts[i]) * random_variables[i];
	sqsum = weights[i] * pow(time - bin_counts[i], 2);
    }

    return -(time/intervals) * (rvsum/sqsum);
    
}

/*
 * Updates the estimates for the means of the random variables.
 */
void lambda_estimate(double *random_variables, int *bin_counts, double a, double b, double time, int intervals)
{
    int i;
    
    for (i = 0; i < intervals; ++i) {
	random_variables[i] = (time/intervals) * (a + b * bin_counts[i]);
    }

}

void weight_estimate(double *weights, double *random_variables, int intervals)
{
    int i;
    double randvar_total = 0;
        
    for (i = 0; i < intervals; ++i) {
	randvar_total += 1/random_variables[i];
    }

    for (i = 0; i < intervals; ++i) {
	weights[i] = intervals * ((1/random_variables[i]) / randvar_total);
    }

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
