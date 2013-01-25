#include "math_util.h"
#include "general_util.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

//#define DEBUG
#define ZERO_EPSILON 0.000000000000000001


int rand_initialised = 0;
gsl_rng* r;

/*
 * Recursive factorial function.
 */
long double fact(int i)
{
    if (i == 0)
	return 1;
    else if (i < 0)
	return 0;
    else
	return i * fact(i - 1);
}

/* calculates the probabilty of there being k events between time t_start and t_end. (for homogenous processes)
 */
double prob_num_events_in_time_span(double start_time, double end_time, double lambda, int k)
{
    if (interval_valid(start_time, end_time) != 1 || lambda <= 0 || k < 0)
	return -1;
    double tau = end_time - start_time;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}

/* 
 * Outputs the number of events in each interval to the array provided. 
 */
int* sum_events_in_interval(double *event_times, int num_events, double start_time, double end_time, int num_subintervals)
{
    if (interval_valid(start_time, end_time) != 1 || event_times == NULL || num_events < 1 || num_subintervals < 1)
	return NULL;
    
    int i = 0, current_interval = 0;
    double subinterval_time = (end_time - start_time) / num_subintervals;

    int *bins = calloc(num_subintervals, sizeof(int));
    
    for (; i < num_events; ++i){
	for (; event_times[i] < start_time; i++); // get to the start of the interval that we are checking.
		
	// Just in case we go over the end of the array that we receive. This shouldn't really happen
	if (event_times[i] < 0.0 + ZERO_EPSILON){
	    printf("SOMETHING IS VERY ODD - WE WENT OVER THE END OF THE ARRAY RECEIVED BY SUM_EVENTS_IN_INTERVAL\n");
	    return bins;
	}

	while (event_times[i] > (start_time + (subinterval_time * (current_interval + 1)))) {
#ifdef DEBUG
	    printf("event time: %lf, end of interval: %lf\n", event_times[i], start_time + (subinterval_time * (current_interval + 1)));
	    printf("%lf is greater than %lf, interval time: %lf, interval incremented to %d\n", event_times[i], start_time + (subinterval_time * (current_interval + 1)), subinterval_time, current_interval + 1);
#endif
	    current_interval++;
	}

#ifdef DEBUG
	printf("current time: %lf, interval end: %lf. Adding to loc %d\n", event_times[i], start_time + (subinterval_time * (current_interval + 1)), current_interval);
#endif

	bins[current_interval]++;
    }

    return bins;
}


/*
 * Initialise the random number generator.
 */
void init_rand(double seed)
{
    if (! rand_initialised) {
	if (seed == 0.0)
	    seed = time(NULL);
	

	const gsl_rng_type* T;
	gsl_rng_env_setup();

	r = gsl_rng_alloc(gsl_rng_rand48); // need to free this somewhere
	gsl_rng_set(r, seed);
	printf("Seed for this run: %lf\n", seed);

	rand_initialised = 1;
    }
}

/*
 * Gets the random number generator that is currently in use.
 */
double get_uniform_rand()
{
    if (!rand_initialised)
	init_rand(0.0);

    return gsl_rng_uniform(r);
}

double get_rand_gaussian()
{
    if (!rand_initialised)
	init_rand(0.0);

    return gsl_ran_ugaussian(r);
}

/*
 * Gets noise based on a poisson distribution centred around the mean
 * provided. Since the mean and variance of a poisson random variable
 * are the same, the larger the mean, the larger the variance.
 */
double get_poisson_noise(double mean)
{

    return get_gaussian_noise(mean, mean);
    
}

/*
 * Gets gaussian noise with the given mean and standard deviation
 */
double get_gaussian_noise(double mean, double std_dev)
{
    if (!rand_initialised)
	init_rand(0.0);

    return (get_rand_gaussian() * std_dev) + mean;
    
}

/*
 * Get midpoints for an interval with the given number of subintervals.
 */
double* get_interval_midpoints(double start_time, double end_time, int subintervals)
{
    if (interval_valid(start_time, end_time) != 1 || subintervals < 1)
	return NULL;
    
    double *midpoints = malloc(subintervals * sizeof(double));
        
    int i;
    
    for (i = 0; i < subintervals; ++i){
	midpoints[i] = get_interval_midpoint(i + 1, start_time, end_time, subintervals);
    }

    return midpoints;
}

/*
 * Get the midpoint of a specified interval. (midpoint(xk)=(k-1/2)*T/N), 1 <= k <= N
 */
double get_interval_midpoint(int subinterval_number, double start_time, double end_time, int subintervals)
{
    if (interval_valid(start_time, end_time) != 1 || subintervals < 1 || subinterval_number < 1 || subinterval_number > subintervals)
	return -1;
    
    return start_time + ((subinterval_number - 1.0/2.0) * ((end_time - start_time)/subintervals));
}

/*
 * Simple average
 */
double avg(double *arr, int len)
{
    int i;
    double sum = 0;
    
    for (i = 0; i < len; ++i) {
	sum += arr[i];
    }
    return sum/len;
}

/*
 * Calculates the total sum of squares for a set of dependent variables
 */
double TSS(double *dependent_variables, int len)
{
    double grand_mean = avg(dependent_variables, len);

    int i;
    double sum = 0;
    for (i = 0; i < len; ++i) {
	sum += pow(dependent_variables[i] - grand_mean, 2);
    }

    return sum;
}

/*
 * Calculates explained sum of squares for a set of estimates and dependent variables
 */
double ESS(double *estimates, double *dependent_variables, int len)
{
    double dep_var_mean = avg(dependent_variables, len);
    
    int i;
    double sum = 0;
    for (i = 0; i < len; ++i){
	sum += pow(estimates[i] - dep_var_mean, 2);
    }

    return sum;
}

/*
 * Calculates the residual sum of squares for a specified set of dependent and independent variables.
 * est_func should be a pointer to a function that will return an estimated value of the dependent variable,
 * given a specific value of the independent variable.
 */
double RSS(double *dependent_variables, double *independent_variables, double (*est_func)(double), int len)
{
    int i;
    int sum = 0;
    for (i = 0; i < len; ++i) {
	sum += pow(dependent_variables[i] - est_func(independent_variables[i]), 2);
    }

    return sum;
}

/*
 * Returns the sum of a double array.
 */
double sum_double_arr(double *arr, int len)
{
    int i;
    double sum = 0;
    
    for (i = 0; i < len; ++i) {
	sum += arr[i];
    }

    return sum;
}

int sum_int_arr(int *arr, int len)
{
    int i;
    int sum = 0;
    
    for (i = 0; i < len; ++i) {
	sum += arr[i];
    }

    return sum;    
}

/*
 * Probability mass function for poisson random variables.
 */
double poisson_PMF(double lambda, int k)
{
    mpf_t res;

    mpf_t top;
    mpf_t lk;
    mpf_t el;
    mpf_t kfacf;
    mpz_t kfacz;
        
    mpz_init(kfacz); // integer factorial
    
    mpf_init(top);
    mpf_init(lk);
    mpf_init(el);
    mpf_init(kfacf); // float storage for factorial
    mpf_init(res);
                    
    // This could cause problems if k is too large
    mpf_set_d(lk, pow(lambda, k));
    mpf_set_d(el, pow(M_E, -lambda));
    mpf_mul(top, lk, el);

    //gmp_printf("pow(lambda, k) * pow(e, -lambda) = %Ff\n", top);

    mpz_fac_ui(kfacz, k);
    
    mpf_set_z(kfacf, kfacz);

    //gmp_printf("%d factorial = %Ff\n", k, kfacf);
        
    mpf_div(res, top, kfacf);

    double result = mpf_get_d(res);

    mpf_clear(kfacf);
    mpf_clear(res);
    mpf_clear(top);
    mpf_clear(lk);
    mpf_clear(el);
    mpz_clear(kfacz);
        
    return result;
}

/*
 * Calculates the gradient of a line given two points on the line.
 */
double get_gradient(double a_x, double a_y, double b_x, double b_y)
{
    return (b_y-a_y)/(b_x-a_x);
}

/*
 * Calculates the intercept of a line given a point on the line and the
 * line's gradient
 */
double get_intercept(double x, double y, double gradient)
{
    return y - gradient * x;
}

/*
 * Calculates the intercept and gradient of a line starting at point a and ending
 * at point b.
 */
double* get_intercept_and_gradient(double a_x, double a_y, double b_x, double b_y)
{
    double *line = malloc(2 * sizeof(double));
    
    line[1] = get_gradient(a_x, a_y, b_x, b_y);
    if (isnan(line[1])){
	free(line);
	return NULL;
    }
    line[0] = get_intercept(a_x, a_y, line[1]);
    
    return line;
}

/*
 * Finds the value of y in a linear function of the form y = a + bx.
 */
double evaluate_function(double a, double b, double x)
{
    return a + b * x;
}

/*
 * Calculate the number halfway between two others.
 */
double get_midpoint(double a, double b)
{

    if (a == b)
	return a;
    
    double diff = abs(a - b);
    
    if (a < b)
	return a + diff/2;
    else
	return b + diff/2;
}



/*
 * Creates a gaussian with the given mean and standard deviation. Returns NULL if
 * the standard deviation is <= 0.
 */
gaussian* make_gaussian(double mean, double stdev)
{
    if (stdev <= 0)
	return NULL;

    gaussian* g = malloc(sizeof(gaussian));
    
    g->mean = mean;
    g->stdev = stdev;
    
    return g;
}

/*
 * Calculates the contribution of gaussians in the given vector at the given
 * point along the x-axis.
 */
double sum_gaussians_at_point(double x, gauss_vector* G)
{
    int i;
    double sum = 0;

    for (i = 0; i < G->len; ++i) {
	sum += G->w[i] * exp(-pow(x - G->gaussians[i]->mean, 2)/G->gaussians[i]->stdev);
    }
    return sum;
}

/*
 * Performs a discrete gaussian transform on the given vector of gaussians, in the
 * interval [start, end], with distance step between sample points. Returns a 2-d
 * vector of the values and points at which the values were sampled.
 */
double** gauss_transform(gauss_vector* G, double start, double end, double step)
{
    if (!interval_valid(start, end))
	return NULL;

    double current;
    int i;
    double** T = malloc(2 * sizeof(double*));
    T[0] = malloc(sizeof(double) * ((end - start)/step));
    T[1] = malloc(sizeof(double) * ((end - start)/step));
    
    for (i = 0, current = start; current < end; current += step, i++) {
	T[1][i] = current;
	T[0][i] = sum_gaussians_at_point(current, G);
    }
    
    return T;
}

/*
 * Generates a vector of specified length with each point p ~ N(0,1)
 */
double* random_vector(int len)
{
    if (len <= 0)
	return NULL;

    init_rand(0.0);
    
    double* V = malloc(len * sizeof(double));
    
    int i;
    
    for (i = 0; i < len; ++i) {
	V[i] = gsl_ran_ugaussian(r);
	printf("weight is %lf\n", V[i]);
    }

    return V;
}

/*
 * Generates the given number of gaussians with evenly distributed means, and the 
 * given stdev, within the specified interval.
 */
gauss_vector* gen_gaussian_vector_uniform(double stdev, double start, double end, int num)
{
    if (!interval_valid(start, end))
	return NULL;
    
    gauss_vector* G = malloc(sizeof(gauss_vector));

    G->gaussians = malloc(sizeof(gaussian*) * num);
    G->w = random_vector(num);
    G->len = num;

    double step = (end - start)/num;
    double current;
    
    int i;
    
    for (i = 0, current = start; i < num && current < end; ++i, current += step) {
	G->gaussians[i] = make_gaussian(current, stdev);
    }

    return G;
}

/*
 * Generates a vector of gaussians whose means are centred on the values
 * in the given array and have the specified standard deviation.
 */
gauss_vector* gen_gaussian_vector_from_array(double* means, int len, double stdev)
{
    gauss_vector* G = malloc(sizeof(gauss_vector));

    G->gaussians = malloc(sizeof(gaussian*) * len);
    G->w = random_vector(len);
    G->len = len;

    int i;
    
    for (i = 0; i < len; ++i) {
	G->gaussians[i] = make_gaussian(means[i], stdev);
    }
    
    return G;
}
