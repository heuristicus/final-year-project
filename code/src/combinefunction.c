#include "combinefunction.h"

/*
 * Combines two or more function estimates to produce a single estimate.
 * Takes an array of function estimates from different streams, and the
 * time delay between each. The average value of these functions is taken.
 */
double_multi_arr* combine_functions(est_arr** estimates, double* time_delay, 
			   double interval_time, int num_estimates, double step)
{
    if (estimates == NULL || time_delay == NULL || step <= 0 || num_estimates <= 0 ||
	interval_time <= 0)
	return NULL;
    
    double time;
    int estimate_num;
    double max_delay = find_max_value(time_delay, num_estimates);
        
//    double_multi_arr* combination = malloc(sizeof(double_multi_arr));
    // need to fix this to work with combinations at any point and
    // any interval. This currently only works for stuff which starts
    // at zero and when the delay is non-negative.
    int memsize = (int)(((interval_time - 2 * max_delay) / step + 1));
    double_multi_arr* combination = init_multi_array(2, memsize);

    /*
     * Only have data for all functions in the intervals in which they
     * are all active, so we must limit the times being checked to this interval.
     * The first point at which we have data for all functions in the same place is
     * where the maximum delay is. The last time we have data for all functions is 
     * where the least delayed function ends (interval_time - max_delay).
     */
    int i;
    
    for (i = 0, time = max_delay; time <= interval_time - max_delay; time += step, ++i) {
	double total = 0;
	for (estimate_num = 0; estimate_num < num_estimates; ++estimate_num) {
	    total += estimate_at_point(estimates[estimate_num], time - time_delay[estimate_num]);
	}
	combination->data[0][i] = time;
	combination->data[1][i] = total/num_estimates;
    }

    return combination;
}

/*
 * Combines a set of vectors of gaussians into a single set of values representing
 * the average value of the sums of the gaussians. Samples are taken with the 
 * given resolution
 */
double_multi_arr* combine_gauss_vectors(gauss_vector** V, double_arr* time_delay,
					double start, double interval_time, double step, int num_vectors)
{

    printf("start %lf, time %lf, step %lf, nvec %d\n", start, interval_time, step, num_vectors);
    printf("time delay len %d\n", time_delay->len);
    double max_delay = find_max_value(time_delay->data, num_vectors);
    int memsize = (int)(interval_time - start - 2 * max_delay) / step + 1;
    printf("memsize is %d\n", memsize);
    double_multi_arr* res = init_multi_array(2, memsize);
    
    double current = max_delay, end = interval_time - start - max_delay, sum;
    printf("Combining function between %lf and %lf.\n", start, end);
    int i, vector_num;
    for (i = 0; current <= end; ++i, current += step) {
	printf("current is %lf\n", current);
	sum = 0;
	for (vector_num = 0; vector_num < num_vectors; ++vector_num) {
	    sum += sum_gaussians_at_point(current - time_delay->data[vector_num], V[vector_num]);
	    printf("summing vector %d at %lf\n", vector_num, current - time_delay->data[vector_num]);
	    printf("sum is now %lf\n", sum);
	}
	res->data[0][i] = current;
	// must apply the same normalisation that is being applied to the gaussians in the estimate?
	// does not apply if the normalisation is calculated later and then applied.
	res->data[1][i] = sum / num_vectors;
	
	printf("i is %d\n", i);
    }

    output_double_multi_arr("combgauss", "w", res);
    
    return NULL;
}
