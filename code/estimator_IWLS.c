#include "estimator.h"
#include "math_util.h"
#include "file_util.h"

int main(int argc, char *argv[])
{
    free(estimate_IWLS(argv[1], argv[2], 25, 75.0, 50, 2));
    return 0;
}

/*
 * Estimates a monotonic linear function with the optimum least squares estimator, which
 * is equivalent to a single iteration of the IWLS method. Returns a, b and the SSE calculated
 * for the estimate.
 */
double* estimate_OLS(char *infile, char *outfile, double start_time, double end_time, int num_subintervals)
{
    return estimate_IWLS(infile, outfile, start_time, end_time, num_subintervals, 1);
}

/*
 * Uses iterative weighted least squares to estimate a monotonic linear function. Returns the estimate of 
 * a, b and the sum squared error calculated for these estimates.
 */
double* estimate_IWLS(char *infile, char *outfile, double start_time, double end_time, int num_subintervals, int iterations)
{
    double interval_time = end_time - start_time;
    
    double **intervals = get_subintervals(start_time, end_time, num_subintervals);
    int *bin_counts = get_bin_counts(infile, start_time, end_time, num_subintervals);

    double *midpoints = get_interval_midpoints(start_time, end_time, num_subintervals);
    double *weights = initialise_weights(num_subintervals);
    double *lambda = NULL;
    
    int i, loop;

    /* for (i = 0; i < num_subintervals; ++i){ */
    /* 	printf("%lf - %lf: %d\n", intervals[i][0], intervals[i][2], bin_counts[i]); */

    /* } */

    double Y_mean, x_mean, est_beta, est_alpha, sse, a, b;
    
    for (loop = 0; loop < iterations; ++loop){
	
	/* for (i = 0; i < num_subintervals; ++i) { */
	/*     printf("Random variable %d is %lf\n", i, random_variables[i]); */
	/* } */

	/* for (i = 0; i < num_subintervals; ++i) { */
	/*     printf("Weight %d is %lf\n", i, weights[i]); */
	/* } */

	Y_mean = mean_Y(bin_counts, weights, num_subintervals);
	x_mean = mean_x(midpoints, weights, num_subintervals);

	printf("mean x %lf\n", x_mean);
	printf("mean randvar %lf\n", Y_mean);


	est_beta = beta_estimate(weights, midpoints, bin_counts, x_mean, num_subintervals);
	est_alpha = alpha_estimate(Y_mean, x_mean, est_beta);

	printf("alpha estimate: %lf\n", est_alpha);
	printf("beta estimate: %lf\n", est_beta);

	sse = SSE(weights, midpoints, bin_counts, est_alpha, est_beta, num_subintervals);

	printf("SSE: %lf\n", sse);
    
	a = a_estimate(est_alpha, interval_time, num_subintervals);
	b = b_estimate(est_beta, interval_time, num_subintervals);

	printf("a estimate: %lf\nb estimate %lf\n", a, b);

	if (loop == 0) {
	    if (a < 0){
		printf("Estimate for a is not positive (%lf) - setting a to 0 and recalculating b.(OLS)\n", a);
		a = 0;
		b = constraint_a_OLS(weights, midpoints, bin_counts, interval_time, num_subintervals);
		printf("New a: %lf\nNew b: %lf\n", a, b);
	    } else if (a > 0 && b < -a/interval_time){
		printf("Estimate for b is not positive (%lf) - setting a=-bT, b=N*beta/T. (OLS)\n", b);
		a = -b * interval_time;
		b = constraint_b_OLS(weights, midpoints, bin_counts, interval_time, num_subintervals);
		printf("New a: %lf\nNew b: %lf\n", a, b);
	    }
	} else {
	    if (a < 0){
		printf("Estimate for a is not within constraints (%lf) - setting a to 0 and recalculating b. (IWLS)\n", a);
		a = 0;
		b = constraint_b_IWLS(bin_counts, interval_time, num_subintervals);
		printf("New a: %lf\nNew b: %lf\n", a, b);
	    } else if (a > 0 && b < -a/interval_time){
		printf("Estimate for b is not within constraints (%lf). Recalculating (IWLS)\n", b);
		a = -b * interval_time;
		b = constraint_b_IWLS(bin_counts, interval_time, num_subintervals);
		printf("New a: %lf\nNew b: %lf\n", a, b);
	    }
	}
	

	lambda = lambda_estimate(lambda, midpoints, a, b, interval_time, num_subintervals);
    
	/* for (i = 0; i < num_subintervals; ++i) { */
	/*     printf("New lambda estimate for random variable %d: %lf\n", i, lambda[i]); */
	/* } */

	weight_estimate(weights, lambda, num_subintervals);
    
	/* for (i = 0; i < num_subintervals; ++i) { */
	/*     printf("New weight %d is %lf\n", i, weights[i]); */
	/* } */

	sse = SSE(weights, midpoints, bin_counts, est_alpha, est_beta, num_subintervals);

	printf("new SSE: %lf\n", sse);

	printf("Estimates after %d iterations:\na = %lf\nb = %lf\n\n\n", loop + 1, a , b);
    
    }

    printf("Final estimates:\na = %lf\nb = %lf\n", a, b);

    FILE *fp = fopen(outfile, "w");
    
    
    for (i = 0; i < num_subintervals; ++i) {
	fprintf(fp, "%lf, %lf, %d, %lf\n", intervals[i][1], a + b * intervals[i][1], bin_counts[i], lambda[i]);
    }

    fclose(fp);
    
    free_pointer_arr((void **) intervals, num_subintervals);
    free(bin_counts);
    free(midpoints);
    free(lambda);
    free(weights);
    
    double *retval = malloc(3 * sizeof(double));
    
    retval[0] = a;
    retval[1] = b;
    retval[2] = sse;
        
    return retval;
}

/*
 * Gets the start, midpoint and end of each time interval, defined
 * by the total time and number of subintervals.
 * 
 * interval = (((k-1)*T)/N, kT/N]
 */
double** get_subintervals(double start_time, double end_time, int num_subintervals)
{

    int i;
    double interval_time = end_time - start_time;
    double **subinterval = malloc(num_subintervals * sizeof(double*));

    for (i = 0; i < num_subintervals; ++i){
	subinterval[i] = malloc(3 * sizeof(double));
	subinterval[i][0] = start_time + (i * interval_time)/num_subintervals;
	subinterval[i][1] = get_interval_midpoint(i + 1, start_time, end_time, num_subintervals);
	subinterval[i][2] = start_time + ((i + 1) * interval_time) / num_subintervals;
    }
    

    return subinterval;
    
}

/*
 * Get the number of events that occurred in each subinterval.
 */
int* get_bin_counts(char *filename, double start_time, double end_time, int num_subintervals)
{

    double *events = get_event_data_interval(start_time, end_time, filename);
    int num_events = (int) events[0] - 1;

    int *bin_counts = sum_events_in_interval(events + 1, num_events, start_time, end_time, num_subintervals);
    
    free(events);
        
    return bin_counts;
    
}


/*
 * Initialise the weights assigned to each subinterval to 1.
 */
double* initialise_weights(int num_subintervals)
{
    double *weights = malloc(num_subintervals * sizeof(double));
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i){
	weights[i] = 1;
    }
    
    return weights;
    
}

/*
 * Initialises each random variable Yk to the number of events in the corresponding subinterval k
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
double beta_estimate(double* weights, double *midpoints, int *bin_counts, double mean_x, int num_subintervals)
{
    double xdiffsum = 0;
    double squarediffsum = 0;
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i) {
	xdiffsum += weights[i] * (midpoints[i] - mean_x) * bin_counts[i];
	squarediffsum += weights[i] * pow(midpoints[i] - mean_x, 2);
    }

    printf("xdiff %lf, sqdiff %lf\n", xdiffsum, squarediffsum);

    return xdiffsum/squarediffsum;
        
}

/*
 * Estimates the value of a
 */
double a_estimate(double alpha, double interval_time, int num_subintervals)
{
    return alpha * (num_subintervals/interval_time);
}

/*
 * Estimates the value of b
 */
double b_estimate(double beta, double interval_time, int num_subintervals)
{
    return beta * (num_subintervals/interval_time);
}

/*
 * Recalculates b in cases where a violates the constraint a >= 0 (i.e. a < 0). This function
 * is used only for OLS.
 */
double constraint_a_OLS(double *weights, double *midpoints, int *bin_counts, double interval_time, int num_subintervals)
{
    double rvsum = 0;
    double sqsum = 0;
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i) {
	rvsum = weights[i] * midpoints[i] * bin_counts[i];
	sqsum = weights[i] * pow(midpoints[i], 2);
    }

    return (num_subintervals/interval_time) * (rvsum/sqsum);
    
}

/*
 * Recalculates b in cases where it violates the constraint b >= -a/T (i.e. a >= 0, but b < -a/T).
 * This function is used only for OLS.
 */
double constraint_b_OLS(double *weights, double *midpoints, int *bin_counts, double interval_time, int num_subintervals)
{
    double rvsum = 0;
    double sqsum = 0;
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i) {
	rvsum = weights[i] * (interval_time - midpoints[i]) * bin_counts[i];
	sqsum = weights[i] * pow(interval_time - midpoints[i], 2);
    }

    return -(num_subintervals/interval_time) * (rvsum/sqsum);
    
}

/*
 * Recalculates b in cases where it violates the constraints imposed. This function is used
 * when IWLS is being used.
 */
double constraint_b_IWLS(int *bin_counts, double interval_time, int num_subintervals)
{
    int i;
    double sum = 0;
        
    for (i = 0; i < num_subintervals; ++i) {
	sum += bin_counts[i] / pow(interval_time, 2);
    }
    
    //return 2 * sum; According to Massey et al 1996, there should be a factor of 2 applied to the sum,
    // but this generates incorrect estimates.
    return sum;
}

/*
 * Original function for IWLS b estimate recalculation. The function constraint_b_IWLS should have a factor
 * of 2 in it according to Massey et al 1996, but it appears that this factor is unnecessary. That function is
 * a simplified version of this function, but appears to have an error. This function correctly recalculates
 * b when it violates the constraints.
 */
double constraint_b_IWLS_2(int *bin_counts, double *midpoints, double interval_time, int num_subintervals)
{
    int i;
    double ysum = 0;
    double xsum = 0;
    
    for (i = 0; i < num_subintervals; ++i) {
	ysum += bin_counts[i];
	xsum += midpoints[i];
    }
    
    return (num_subintervals / interval_time) * (ysum / xsum);
    
}

/*
 * Updates the estimates for the means of the random variables.
 */
double* lambda_estimate(double *lambda, double *midpoints, double a, double b, double interval_time, int num_subintervals)
{
    int i;
    
    if (lambda == NULL){
	lambda = malloc(num_subintervals * sizeof(double));
    }
        
    for (i = 0; i < num_subintervals; ++i) {
	lambda[i] = (interval_time/num_subintervals) * (a + b * midpoints[i]);
    }

    return lambda;

}

/*
 * Updates the estimates for the weights
 */
void weight_estimate(double *weights, double *random_variables, int num_subintervals)
{
    int i;
    double randvar_total = 0;
        
    for (i = 0; i < num_subintervals; ++i) {
	randvar_total += 1/random_variables[i];
    }

    for (i = 0; i < num_subintervals; ++i) {
	weights[i] = (num_subintervals/random_variables[i]) / randvar_total;
    }

}

/*
 * Calculates the mean of the subinterval midpoints, taking into consideration the
 * weight of each value.
 */
double mean_x(double *midpoints, double *weights, int num_subintervals)
{
    int i;
    double sum = 0.0;
    
    for (i = 0; i < num_subintervals; ++i){
	sum += weights[i] * midpoints[i];
    }

    return (1.0/num_subintervals) * sum;
    
}

/*
 * Calculates the mean value of the random variables, given the current weights
 */
double mean_Y(int *bin_counts, double *weights, int num_subintervals)
{
    int i;
    double sum = 0;
    
    for (i = 0; i < num_subintervals; ++i) {
	sum += weights[i] * bin_counts[i];
    }
    
    return (1.0/num_subintervals) * sum;
    
}

/*
 * Find the sum squared error for a set of estimates
 */
double SSE(double *weights, double *midpoints, int *bin_counts, double alpha, double beta, int num_subintervals)
{
    double sum = 0.0;
    int i;
    
    for (i = 0; i < num_subintervals; ++i){
	sum += weights[i] * pow((bin_counts[i] - (alpha + beta * midpoints[i])), 2);
    }

    return sum;
    
}