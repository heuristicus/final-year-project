#include "estimator.h"
#include "math_util.h"
#include "file_util.h"
#include "general_util.h"

/* double **intervals; */
/* double *weights; */
/* double *rand_var; */
/* double *lambda_estimate */
/* double alpha, beta, a, b, rand_var_mean, x_mean; */

int main(int argc, char *argv[])
{
    double interval_time = 10.0;
    int num_subintervals = 10;
        
    double **subinterval_data = get_subinterval_data(interval_time, num_subintervals);
    int *bin_counts = get_bin_counts(argv[1], interval_time/num_subintervals, num_subintervals);
    
    double *midpoints = get_interval_midpoints(interval_time, num_subintervals);
    double *weights = initialise_weights(num_subintervals);
    double *random_variables = initialise_random_variables(midpoints, num_subintervals);
    
    int i, loop;
    
    for (i = 0; i < num_subintervals; ++i){
	printf("%lf - %lf: %d\n", subinterval_data[i][0], subinterval_data[i][2], bin_counts[i]);

    }

    double Y_mean, x_mean, est_beta, est_alpha, sse, a, b;
    
    for (loop = 0; loop < 10; ++loop){
	
	/* for (i = 0; i < num_subintervals; ++i) { */
	/*     printf("Random variable %d is %lf\n", i, random_variables[i]); */
	/* } */

	/* for (i = 0; i < num_subintervals; ++i) { */
	/*     printf("Weight %d is %lf\n", i, weights[i]); */
	/* } */

	Y_mean = mean_Y(random_variables, weights, num_subintervals);
	x_mean = mean_x(midpoints, weights, num_subintervals);

	printf("mean x %lf\n", x_mean);
	printf("mean randvar %lf\n", Y_mean);


	est_beta = beta_estimate(weights, midpoints, random_variables, x_mean, num_subintervals);
	est_alpha = alpha_estimate(Y_mean, x_mean, est_beta);

	printf("alpha estimate: %lf\n", est_alpha);
	printf("beta estimate: %lf\n", est_beta);

	sse = SSE(random_variables, weights, midpoints, est_alpha, est_beta, num_subintervals);

	printf("SSE: %lf\n", sse);
    
	a = a_estimate(est_alpha, interval_time, num_subintervals);
	b = b_estimate(est_beta, interval_time, num_subintervals);

	printf("a estimate: %lf\nb estimate %lf\n", a, b);

	if (a < 0){
	    printf("Estimate for a is not positive (%lf) - setting a to 0 and recalculating b.\n", a);
	    a = 0;
	    b = constraint_a(weights, midpoints, random_variables, interval_time, num_subintervals);
	} else if (a > 0 && b < -a/interval_time){
	    printf("Estimate for b is not withing constraints (%lf) - setting a=-bT, b=N*beta/T\n", b);
	    a = -b * interval_time;
	    b = constraint_b(weights, midpoints, random_variables, interval_time, num_subintervals);
	}

	randvar_estimate(random_variables, midpoints, a, b, interval_time, num_subintervals);
    
	for (i = 0; i < num_subintervals; ++i) {
	    printf("New lambda estimate for random variable %d: %lf\n", i, random_variables[i]);
	}

	weight_estimate(weights, random_variables, num_subintervals);
    
    
	for (i = 0; i < num_subintervals; ++i) {
	    printf("New weight %d is %lf\n", i, weights[i]);
	}


	sse = SSE(random_variables, weights, midpoints, est_alpha, est_beta, num_subintervals);

	printf("new SSE: %lf\n\n\n\n", sse);
    
    }

    FILE *fp = fopen(argv[2], "w");
    
    
    for (i = 0; i < num_subintervals; ++i) {
	fprintf(fp, "%lf, %lf\n", subinterval_data[i][1], a + b * subinterval_data[i][1]);
    }

    free_pointer_arr((void **) subinterval_data, num_subintervals);
    free(midpoints);
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
double** get_subinterval_data(double interval_time, int num_subintervals)
{
    int i;
    double **subinterval = malloc(num_subintervals * sizeof(double*));

    for (i = 0; i < num_subintervals; ++i){
	subinterval[i] = malloc(3 * sizeof(double));
	subinterval[i][0] = (i * interval_time)/num_subintervals;
	subinterval[i][1] = get_interval_midpoint(i + 1, interval_time, num_subintervals);
	subinterval[i][2] = ((i + 1) * interval_time) / num_subintervals;
    }

    return subinterval;
    
}

/*
 * Get the number of events that occurred in each subinterval.
 */
int* get_bin_counts(char *filename, double interval_time, int num_subintervals)
{
    double *events = get_event_data(filename);
    int num_events = (int) events[0] - 1;

    int *bin_counts = sum_events_in_interval(events + 1, num_events, interval_time, num_subintervals);
    
    free(events);
        
    return bin_counts;
    
}


/*
 * Initialise the weights assigned to each subinterval to 1. This is 
 * the simplest way of doing things.
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
 * Initialises each random variable Yk to the event number in the corresponding subinterval
 * (should noise be added here?)
 */
double* initialise_random_variables(double *midpoints, int num_subintervals)
{
    double* rand_var = malloc(num_subintervals * sizeof(double));
    
    int i;
        
    for (i = 0; i < num_subintervals; ++i){
	rand_var[i] = midpoints[i] + get_poisson_noise(midpoints[i]);
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
double beta_estimate(double* weights, double *midpoints, double *rand_var, double mean_x, int num_subintervals)
{
    double xdiffsum = 0;
    double squarediffsum = 0;
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i) {
	xdiffsum += weights[i] * (midpoints[i] - mean_x) * rand_var[i];
	squarediffsum += weights[i] * pow(midpoints[i] - mean_x, 2);
    }

    printf("xdiff %lf, sqdiff %lf\n", xdiffsum, squarediffsum);

    return xdiffsum/squarediffsum;
        
}

double a_estimate(double alpha, double interval_time, int num_subintervals)
{
    return alpha * (interval_time/num_subintervals);
}

double b_estimate(double beta, double interval_time, int num_subintervals)
{
    return beta * (interval_time/num_subintervals);
}

double constraint_a(double *weights, double *midpoints, double *random_variables, double interval_time, int num_subintervals)
{
    double rvsum = 0;
    double sqsum = 0;
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i) {
	rvsum = weights[i] * midpoints[i] * random_variables[i];
	sqsum = weights[i] * pow(midpoints[i], 2);
    }

    return (interval_time/num_subintervals) * (rvsum/sqsum);
    
}

double constraint_b(double *weights, double *midpoints, double *random_variables, double interval_time, int num_subintervals)
{
    double rvsum = 0;
    double sqsum = 0;
    
    int i;
    
    for (i = 0; i < num_subintervals; ++i) {
	rvsum = weights[i] * (interval_time - midpoints[i]) * random_variables[i];
	sqsum = weights[i] * pow(interval_time - midpoints[i], 2);
    }

    return -(interval_time/num_subintervals) * (rvsum/sqsum);
    
}

/*
 * Updates the estimates for the means of the random variables.
 */
void randvar_estimate(double *random_variables, double *midpoints, double alpha, double beta, double interval_time, int num_subintervals)
{
    int i;

    for (i = 0; i < num_subintervals; ++i) {
	random_variables[i] = alpha + beta * midpoints[i] + get_poisson_noise(random_variables[i]);
    }

}

void weight_estimate(double *weights, double *random_variables, int num_subintervals)
{
    int i;
    double randvar_total = 0;
        
    for (i = 0; i < num_subintervals; ++i) {
	randvar_total += 1/random_variables[i];
    }

    for (i = 0; i < num_subintervals; ++i) {
	weights[i] = num_subintervals * ((1/random_variables[i]) / randvar_total);
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
double mean_Y(double *random_variables, double *weights, int num_subintervals)
{
    int i;
    double sum = 0;
    
    for (i = 0; i < num_subintervals; ++i) {
	sum += weights[i] * random_variables[i];
    }
    
    return (1.0/num_subintervals) * sum;
    
}

double SSE(double *rand_var, double *weights, double *midpoints, double alpha, double beta, int num_subintervals)
{
    double sum = 0.0;
    int i;
    
    for (i = 0; i < num_subintervals; ++i){
	sum += weights[i] * pow((rand_var[i] - (alpha + beta * midpoints[i])), 2);
    }

    return sum;
    
}
