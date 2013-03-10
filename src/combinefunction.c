#include "combinefunction.h"

/*
 * Combines two or more function estimates to produce a single estimate.
 * Takes an array of function estimates from different streams, and the
 * time delay between each. The average value of these functions is taken.
 * This function will return only points for which all functions being combined
 * have values. The overlap between functions depends
 * on the maximum time delay between any two functions. For example, if there are
 * three functions, and the delay between 0 and 1 is 10, and 0 and 2 is 20, and you
 * specify an interval time of 100, then you will only receive data for times 20 to 80,
 * since the functions only overlap there.
 */
double_multi_arr* combine_functions(est_arr** estimates, double_arr* time_delay, 
				    double start, double interval_time, double step,
				    int num_estimates)
{
//    printf("estimates %p, time delay %p, interval_time %lf, step %lf, num estimates %d\n", estimates, time_delay, interval_time, step, num_estimates);
    if (estimates == NULL || time_delay == NULL || step <= 0 || num_estimates <= 0 ||
	interval_time <= 0)
	return NULL;

    double time;
    int estimate_num;
    double max_delay = largest_value_in_arr(time_delay->data, num_estimates);
    double end = interval_time - start - max_delay;

//    printf("Combining function between %lf and %lf.\n", time, end);

    // need to fix this to work with combinations at any point and
    // any interval. This currently only works for stuff which starts
    // at zero and when the delay is non-negative(?).
    int memsize = ((interval_time - start - 2 * max_delay) / step) + 10;


    double* times = malloc((memsize + 10) * sizeof(double));
    double* sums = malloc((memsize + 10) * sizeof(double));
    
    /*
     * Only have data for all functions in the intervals in which they
     * are all active, so we must limit the times being checked to this interval.
     * The first point at which we have data for all functions in the same place is
     * where the maximum delay is. The last time we have data for all functions is 
     * where the least delayed function ends (interval_time - max_delay).
     */
    int i;

    for (i = 0, time = max_delay; dbl_leq(time, end, 0.0001); time += step, ++i) {
//	printf("time is %.30lf\n", time);
	double total = 0;
	for (estimate_num = 0; estimate_num < num_estimates; ++estimate_num) {
	    total += estimate_at_point(estimates[estimate_num], time - time_delay->data[estimate_num]);
	}
	
	times[i] = time;
	sums[i] = total/num_estimates;
//	printf("sum at time %lf is %lf\n", times[i], sums[i]);
    }
//    printf("time is %.30lf\n", time);

    double_multi_arr* combination = malloc(sizeof(double_multi_arr));
    combination->len = 2;
    combination->lengths = malloc(2 * sizeof(int));
    combination->lengths[0] = i;
    combination->lengths[1] = i;
    combination->data = malloc(2 * sizeof(double*));
    combination->data[0] = times;
    combination->data[1] = sums;

    return combination;
}

/*
 * Combines a set of vectors of gaussians into a single set of values representing
 * the average value of the sums of the gaussians. Samples are taken with the 
 * given resolution. This version of the function will return only points for which
 * all functions being combined have values. The overlap between functions depends
 * on the maximum time delay between any two functions. For example, if there are
 * three functions, and the delay between 0 and 1 is 10, and 0 and 2 is 20, and you
 * specify an interval time of 100, then you will only receive data for times 20 to 80,
 * since the functions only overlap there.
 */
double_multi_arr* combine_gauss_vectors(gauss_vector** V, double_arr* time_delay,
					double start, double interval_time, double step,
					double normaliser, int num_vectors)
{

    if (V == NULL || time_delay == NULL || interval_time <= 0 || step <= 0 || num_vectors <= 0)
	return NULL;
    
//    printf("start %lf, time %lf, step %lf, nvec %d\n", start, interval_time, step, num_vectors);
//    printf("time delay for function 0 is %lf, 1 is %lf\n", time_delay->data[0], time_delay->data[1]);
    

    double max_delay = largest_value_in_arr(time_delay->data, num_vectors);
//    printf("max delay %lf\n", max_delay);


    double current = max_delay, end = interval_time - start - max_delay, sum;
//    printf("Combining function between %lf and %lf.\n", current, end);

    int memsize = ((interval_time - start - 2 * max_delay) / step) + 10;
//    printf("memsize is %d\n", memsize);

    double* times = malloc((memsize + 10) * sizeof(double));
    double* sums = malloc((memsize + 10) * sizeof(double));


    int i, vector_num;
    for (i = 0; dbl_leq(current, end, 0.0001); ++i, current += step) {
//	printf("current is %lf\n", current);
	sum = 0;
	for (vector_num = 0; vector_num < num_vectors; ++vector_num) {
	    sum += sum_gaussians_at_point(current - time_delay->data[vector_num], V[vector_num]);
//		printf("summing vector %d at %lf\n", vector_num, current - time_delay->data[vector_num]);
//		printf("sum is now %lf\n", sum);
		
	}
	times[i] = current;
	// must apply the same normalisation that is being applied to the gaussians in the estimate?
	// does not apply if the normalisation is calculated later and then applied.
	sums[i] = (sum / num_vectors) / normaliser;
    }

    double_multi_arr* res = malloc(sizeof(double_multi_arr));
    res->len = 2;
    res->lengths = malloc(2 * sizeof(int));
    res->lengths[0] = i;
    res->lengths[1] = i;
    res->data = malloc(2 * sizeof(double*));
    res->data[0] = times;
    res->data[1] = sums;
    

    return res;
}
