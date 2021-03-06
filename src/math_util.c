#include "math_util.h"

#define ZERO_EPSILON 0.000000000000000001


int rand_initialised = 0;
gsl_rng* r;

// Cleanup any allocated memory.
void cleanup()
{
    gsl_rng_free(r);
}

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
int* sum_events_in_interval(double* event_times, int num_events, double start_time, double end_time, int num_subintervals)
{
    if (interval_valid(start_time, end_time) != 1 || event_times == NULL || num_events < 1 || num_subintervals < 1)
	return NULL;
#ifdef VERBOSE 
    printf("num subintervals %d num events %d\n", num_subintervals, num_events);
#endif
    int i = 0, current_interval = 0;
    double subinterval_time = (end_time - start_time) / num_subintervals;

    int *bins = calloc(num_subintervals, sizeof(int));
     for (; i < num_events; ++i){
	for (; event_times[i] < start_time; i++); // get to the start of the interval that we are checking.
		
	// Just in case we go over the end of the array that we receive. This shouldn't really happen
	if (event_times[i] < 0.0 + ZERO_EPSILON){
	    printf("SOMETHING IS VERY ODD - WE PROBABLY WENT OVER THE END OF THE ARRAY RECEIVED BY SUM_EVENTS_IN_INTERVAL\n");
	    return bins;
	}

	while (event_times[i] > (start_time + (subinterval_time * (current_interval + 1)))) {
#ifdef VERBOSE
	    printf("event time: %lf, end of interval: %lf\n", event_times[i], start_time + (subinterval_time * (current_interval + 1)));
	    printf("%lf is greater than %lf, interval time: %lf, interval incremented to %d\n", event_times[i], start_time + (subinterval_time * (current_interval + 1)), subinterval_time, current_interval + 1);
#endif
	    current_interval++;
	}

#ifdef VERBOSE
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

	r = gsl_rng_alloc(gsl_rng_rand48);
	gsl_rng_set(r, seed);
	printf("Seed for this run: %lf\n", seed);

	rand_initialised = 1;
	atexit(cleanup); // make sure we free the random number generator on exit
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

    if (dbl_equal(a, b, 0.0000001))
	return a;
    
    double diff = fabs(a - b);

    if (a < b)
	return a + diff/2.0;
    else
	return b + diff/2.0;
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
 * Calculates the contribution of the specified gaussian at the given point.
 * The weight specified is applied to the gaussian.
 */
double gaussian_contribution_at_point(double x, gaussian* g, double weight)
{
    if (weight == 0)
	return 0;

    return weight * exp(-pow(x - g->mean, 2)/(2 * pow(g->stdev, 2)));
}

/*
 * Calculates the contribution of the given gaussian in the interval [start, end]
 * with the specified step between each contribution check.
 */
double** gaussian_contribution(gaussian* g, double start, double end, double step, double weight)
{
    printf("contrib %lf %lf\n", start, end);
    if (g == NULL || step <= 0 || start >= end)
	return NULL;
    
    int len = (end - start)/step;
    int i;
    double current;
    double** cont = malloc(2 * sizeof(double*));
    cont[0] = malloc(len * sizeof(double));
    cont[1] = malloc(len * sizeof(double));

    for (i = 0, current = start; current <= end && i <= len; current+= step, ++i) {
	cont[0][i] = current;
	cont[1][i] = gaussian_contribution_at_point(current, g, weight);
    }

    return cont;
}

/*
 * Calculates the contribution of gaussians in the given vector at the given
 * point along the x-axis.
 */
double sum_gaussians_at_point(double x, gauss_vector* G)
{
    if (G == NULL)
	return 0;
    
    int i;
    double sum = 0;

    for (i = 0; i < G->len; ++i) {
	sum += gaussian_contribution_at_point(x, G->gaussians[i], G->w[i]);
    }
    return sum;
}

/*
 * Performs a discrete gaussian transform on the given vector of gaussians, in the
 * interval [start, end], with the given data resolution. Returns a 2-d
 * vector of the values and points at which the values were sampled.
 */
double_multi_arr* gauss_transform(gauss_vector* G, double start, double end, double resolution)
{
    if (start >= end || resolution <= 0 || G == NULL)
	return NULL;

    double current;
    int i;
    
    int memsize = ((end - start)/resolution) + 1;
    double_multi_arr* ret = init_multi_array(2, memsize);
    
    for (i = 0, current = start; dbl_leq(current, end, 0.0001) && i < memsize; current += resolution, i++) {
	ret->data[0][i] = current;
	ret->data[1][i] = sum_gaussians_at_point(current, G);
    }

    return ret;
}

/*
 * Returns a gaussian transform which is shifted so that all points are >= 0 on the
 * y axis
 */
double_multi_arr* shifted_transform(gauss_vector* V, double start, double interval,
				   double step, double resolution)
{
    if (step <= 0 || resolution <= 0 || start >= start + interval)
	return NULL;
    
    double_multi_arr* func = gauss_transform(V, start, start + interval, resolution);

    double* shifted_func = shift_above_zero(func->data[1], func->lengths[1]);
    free(func->data[1]);
    func->data[1] = shifted_func;
    
    return func;
}

/*
 * Generates a vector of specified length with each point p ~ [-1,1]
 * random_vector(int len, double multiplier)
 */
double* random_vector(int len, double multiplier)
{
    if (len <= 0)
	return NULL;

    if (!rand_initialised)
	init_rand(0.0);
    
    double* V = malloc(len * sizeof(double));
    
    int i;
    
    for (i = 0; i < len; ++i) {
	V[i] = (gsl_rng_uniform(r) * 2 - 1) * multiplier;
    }

    return V;
}

/*
 * Generate a vector of length len, with all values set to the specified 
 * weight.
 */
double* weight_vector(double weight, int len)
{
    if (len <= 0)
	return NULL;
    
    double* V = malloc(len * sizeof(double));
    
    int i;
    
    for (i = 0; i < len; ++i) {
	V[i] = weight;
    }

    return V;
}

/*
 * Generates gaussians with the given stdev spaced according to the step parameter
 * within the interval [start, start + interval_time]. A gaussian is always placed at the start
 * of the interval. Returns a null pointer if the interval is invalid or the 
 * step or stdev are <= 0
 */
gauss_vector* gen_gaussian_vector_uniform(double stdev, double start,
					  double interval_time, double step,
					  double multiplier)
{
    double end = start + interval_time;
    if (start >= end || step <= 0 || stdev <= 0)
	return NULL;
    
    gauss_vector* G = malloc(sizeof(gauss_vector));
    
    int num = interval_time/step + 1;

    G->gaussians = malloc(sizeof(gaussian*) * num);
    G->w = random_vector(num, multiplier);
    G->len = num;
    double current;
    
    int i;
    
    for (i = 0, current = start; current <= end; ++i, current += step) {
	G->gaussians[i] = make_gaussian(current, stdev);
    }

    return G;
}

/*
 * Generates a vector of gaussians whose means are centred on the values
 * in the given array and have the specified standard deviation. Returns null if
 * the stdev or len <= 0, or a null pointer is passed in. The random_weights
 * parameter specifies whether to use randomised weights or not. Anything other
 * than 0 will use randomised weights, 0 initialises the vector with all 1s
 */
gauss_vector* gen_gaussian_vector_from_array(double* means, int len, double stdev,
					     double multiplier, int random_weights)
{
    if (stdev <= 0 || len <= 0 || means == NULL)
	return NULL;
    
    gauss_vector* G = malloc(sizeof(gauss_vector));

    G->gaussians = malloc(sizeof(gaussian*) * len);
    if (random_weights == 0)
	G->w = weight_vector(1, len);
    else
	G->w = random_vector(len, multiplier);
    G->len = len;

    int i;

    for (i = 0; i < len; ++i) {
	G->gaussians[i] = make_gaussian(means[i], stdev);
    }
    
    return G;
}

/*
 * Adds the specified value to all elements in the array. This is non-destructive.
 */
double* add_to_arr(double* data, int len, double add)
{
    if (data == NULL || len <= 0){
	return NULL;
    }
    
    double* new = malloc(len * sizeof(double));
    
    int i;
    
    for (i = 0; i < len; ++i) {
	new[i] = data[i] + add;
    }

    return new;
}

/*
 * Finds the minimum value in a double array. Zero can be returned either if it
 * is the minimum value, if the data is NULL, or if the length is invalid
 */
double find_min_value(double* data, int len)
{
    if (data == NULL || len <= 0){
	return 0;
    }
    
    int i;
    double min = INFINITY;
    
    for (i = 0; i < len; ++i) {
	if (data[i] < min){
	    min = data[i];
	}
	
    }
    return min;
}

int find_min_value_int(int* data, int len)
{
    if (data == NULL || len <= 0){
	return 0;
    }

    int i;
    int min = INT_MAX;
    
    for (i = 0; i < len; ++i) {
	if (data[i] < min){
	    min = data[i];
	}
	
    }
    return min;
}

double find_max_value(double* data, int len)
{
    if (data == NULL || len <= 0){
	return 0;
    }
    
    int i;
    double max = -INFINITY;
    
    for (i = 0; i < len; ++i) {
	if (data[i] > max){
	    max = data[i];
	}
	
    }
    return max;
}

double* multiply_arr(double* data, int len, double multiplier)
{
    if (data == NULL || len <= 0){
	return NULL;
    }
    
    double* new = malloc(len * sizeof(double));
    
    int i;
    
    for (i = 0; i < len; ++i) {
	new[i] = data[i] * multiplier;
    }

    return new;
}

double log_pdf(int count, double lambda, double normaliser)
{
    if (lambda == 0)
	return log(gsl_ran_poisson_pdf(count, 0.00001/normaliser));
    else 
	return log(gsl_ran_poisson_pdf(count, lambda/normaliser));
}

/*
 * Computes the log of the probability mass function for each count-lambda pair.
 */
double sum_log_pdfs(int* counts, double* lambdas, double normaliser, int len)
{
    if (counts == NULL || lambdas == NULL || normaliser == 0 || len <= 0)
	return -INFINITY;
    
    int i;
    double sum = 0;
    
    for (i = 0; i < len; ++i) {
	double res = log_pdf(counts[i], lambdas[i], normaliser);
	sum += res;
	/* printf("count %d, normalised lambda %lf, pmf %lf, sum now %lf\n", */
        /*       counts[i], lambdas[i]/normaliser, res, sum); */
    }

    return sum;
}

/*
 * Computes the sum of values in the left-open, right-closed interval (start, end].
 * This means that values which fall at exactly start or end are not considered
 * in the sum. Each value in the times array should correspond to a value in the
 * values array. All values in the value array are divided by the normaliser provided.
 */
double sum_array_interval(double* times, double* values, double start, double end,
			  double normaliser, int len)
{
    if (times == NULL || values == NULL || end <= start || len <= 0)
	return -INFINITY;
    
//    printf("summing interval from %lf to %lf, normaliser %lf with len %d\n", start, end, normaliser, len);
    
    int i;
    i = 0;
    int ln = len;
    double current;
    double en;
    en = end;
    current = times[0];
    double sum;
    sum = 0;

    while (current < en && i < ln){//dbl_less_than(current, end, 0.0001) && i < ln){
	//	printf("current is %.30lf, end is %.30lf, i is %d, value is %lf\n", current, end, i, values[i]);
	if (dbl_less_than(current, start, 0.0001)){
	    //  printf("continuing\n");
	    i++;
	    current = times[i];
	    continue;
	}
	
	//	printf("value here is %lf\n", values[i]);
	sum += values[i];
	//		printf("sum now %.30lf\n", sum);
	i++;
	current = times[i];
    }

	
    
/*     for (i = 0; dbl_less_than(current, end, 0.0001) && i < len; ++i, current = times[i]) { */
/* 	if (dbl_less_than(current, start, 0.0001)) */
/* 	    continue; */
/* //	printf("current is %.30lf, end is %.30lf, i is %d, value is %lf\n", current, end, i, values[i]); */
/* 	sum += values[i]; */
	
/*     } */

//    printf("sum is %lf\n", sum);
//    printf("normalised sum is %lf\n", sum/normaliser);

    return sum / normaliser;
}

/*
 * Sums the given gaussian vector at all points provided in the array.
 */
double_arr* sum_gaussians_at_points(gauss_vector* G, double* points, int len)
{
    if (G == NULL || points == NULL || len <= 0)
	return NULL;

    double_arr* ret = init_double_arr(len);

    int i;

    for (i = 0; i < len; ++i) {
	ret->data[i] = sum_gaussians_at_point(points[i], G);
    }
    
    return ret;
}

/*
 * Finds the largest positive or negative value in the given array. Returns the
 * absolute value of the largest element.
 */
double largest_value_in_arr(double* data, int len)
{
    if (data == NULL || len <= 0){
	return 0;
    }
    
    int i;
    double max = -INFINITY;
    
    for (i = 0; i < len; ++i) {
	
	if (fabs(data[i]) > max){
	    max = fabs(data[i]);
	}
	
    }
    return max;
}

/*
 * Return the absolute value of the parameter with the largest absolute value
 */
double abs_max(double a, double b)
{
    double br = fabs(b), ar = fabs(a);
    return ar > br ? ar : br;
}

/*
 * Checks whether a is less than b, to some number of decimal places.
 */
int dbl_less_than(double a, double b, double precision)
{
    double diff;
    diff = b - a;
    double pr = precision;
    int p1 = diff > pr;
    int p2 = diff > 0;
    int ret = p1 && p2;

    /* printf("diff is %.30lf\n", diff); */
    /* printf("returning %d\n", diff > precision && diff > 0); */
    return ret;
}

int dbl_equal(double a, double b, double precision)
{
    double diff = fabs(b - a);

    return diff < precision;
}

/*
 * Checks a <= b with a certain precision.
 */
int dbl_leq(double a, double b, double precision)
{
    return dbl_less_than(a, b, precision) || dbl_equal(a, b, precision);
}

/*
 * Shift the values of numbers in the given array such that none of them are < 0
 */
double* shift_above_zero(double* arr, int len)
{
    double min = find_min_value(arr, len);
    double shift = 0;
    if (min <= 0){
    	shift = -min;
    }

    double* rep = add_to_arr(arr, len, shift);

    return rep;
}

/*
 * Calculates values of the original function given in a gauss vector which correspond
 * to the sampling times of the estimated function. The values received from the gaussian
 * vector are shifted so that none of them are < 0. The estimated functions never have
 * values below zero since the rate parameter can never go below zero.
 */
double* get_corresponding_funcvals(gauss_vector* original, double_multi_arr* estimated)
{
    int i;

    double* func_orig = malloc(sizeof(double) * estimated->lengths[0]);
    for (i = 0; i < estimated->lengths[0]; ++i) {
	func_orig[i] = sum_gaussians_at_point(estimated->data[0][i], original);
    }
    
    double* shifted_orig = shift_above_zero(func_orig, estimated->lengths[0]);
    free(func_orig);

    return shifted_orig;
}

double get_twofunction_RSS(gauss_vector* original, double_multi_arr* estimated)
{

    double* func_orig = get_corresponding_funcvals(original, estimated);

    double rss = RSS(func_orig, estimated->data[1], estimated->lengths[0]);
    free(func_orig);
    
    return rss;
}

double get_twofunction_TSS(gauss_vector* original, double_multi_arr* estimated)
{

    double* func_orig = get_corresponding_funcvals(original, estimated);

    double tss = TSS(func_orig, estimated->lengths[0]);
    free(func_orig);
    
    return tss;
}

double get_twofunction_ESS(gauss_vector* original, double_multi_arr* estimated)
{

    double* func_orig = get_corresponding_funcvals(original, estimated);

    double ess = ESS(estimated->data[1], func_orig, estimated->lengths[0]);
    free(func_orig);
    
    return ess;
}

double get_twofunction_RMS(gauss_vector* original, double_multi_arr* estimated)
{

    double* func_orig = get_corresponding_funcvals(original, estimated);
    
    return RMS(estimated->data[1], func_orig, estimated->lengths[1]);
}

/*
 * Calculates the total sum of squares for a set of dependent variables
 */
double TSS(double* dependent_variables, int len)
{
    double mean = avg(dependent_variables, len);

    int i;
    double sum = 0;
    for (i = 0; i < len; ++i) {
	sum += pow(dependent_variables[i] - mean, 2);
    }

    return sum;
}

/*
 * Calculates explained sum of squares for a set of estimates and dependent variables
 */
double ESS(double* estimates, double* dependent_variables, int len)
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
 */
double RSS(double* dependent_variables, double* independent_variables, int len)
{
    int i;
    int sum = 0;
    for (i = 0; i < len; ++i) {
//	printf("dep %lf ind %lf\n", dependent_variables[i], independent_variables[i]);
	sum += pow(dependent_variables[i] - independent_variables[i], 2);
    }

    return sum;
}

/*
 * Calculates the root mean square error for the given values.
 */
double RMS(double* estimate, double* original, int len)
{
    return sqrt(MSE(estimate, original, len));
}

/*
 * Calculates the mean squared error for a given set of predictions and true
 * values.
 */
double MSE(double* estimate, double* original, int len)
{
    int i;
    int sum = 0;
    
    for (i = 0; i < len; ++i) {
	sum += pow(estimate[i] - original[i], 2);
    }

    return sum/len;
}

/*
 * Calculates the standard deviation for a set of data
 */
double stdev(double* data, int len)
{
    double mean = avg(data, len);
    
    int i;
    
    double sum = 0;
    for (i = 0; i < len; ++i) {
	sum += pow(data[i] - mean, 2);
    }

    return sqrt(sum/len);
}
