#include "math_util.h"
#include "general_util.h"

//#define DEBUG
#define ZERO_EPSILON 0.000000000000000001

int rand_initialised = 0;

/* int main(int argc, char *argv[]) */
/* { */

/*     double lambda = atof(argv[1]); */
/*     int i; */
    
/*     for (i = 1; i < 100; ++i) { */
/* 	printf("probability of %d events given lambda %lf: %.15lf\n", i, lambda, poisson_PMF(lambda, i)); */
/*     } */
    
/*     return 0; */
    
/* } */


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
    if (interval_check(start_time, end_time) != 1 || lambda <= 0 || k < 0)
	return -1;
    double tau = end_time - start_time;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}

/* 
 * Outputs the number of events in each interval to the array provided. 
 */
int* sum_events_in_interval(double *event_times, int num_events, double start_time, double end_time, int num_subintervals)
{
    if (interval_check(start_time, end_time) != 1 || event_times == NULL || num_events < 1 || num_subintervals < 1)
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

void init_rand(double seed)
{
    if (! rand_initialised) {
	if (seed == 0.0)
	    seed = time(NULL);

	srand48(seed);
	printf("Seed for this run: %lf\n", seed);

	rand_initialised = 1;
    }
}

/*
 * From "The Art of Computer Programming", Vol.3. Generates a gaussian random
 * value with mean of 0 and standard deviation of 1. http://c-faq.com/lib/gaussian.html
 */
double rand_gauss()
{

    if(!rand_initialised)
	init_rand(0.0);
        
    static double V1, V2, S;
    static int phase = 0;
    double X;

    if(phase == 0) {
	do {
	    double U1 = (double)rand() / RAND_MAX;
	    double U2 = (double)rand() / RAND_MAX;

	    V1 = 2 * U1 - 1;
	    V2 = 2 * U2 - 1;
	    S = V1 * V1 + V2 * V2;
	} while(S >= 1 || S == 0);

	X = V1 * sqrt(-2 * log(S) / S);
    } else
	X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    return X;
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
    return (rand_gauss() * std_dev) + mean;
    
}

/*
 * Get midpoints for an interval with the given number of subintervals.
 */
double* get_interval_midpoints(double start_time, double end_time, int subintervals)
{
    if (interval_check(start_time, end_time) != 1 || subintervals < 1)
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
    if (interval_check(start_time, end_time) != 1 || subintervals < 1 || subinterval_number < 1 || subinterval_number > subintervals)
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
