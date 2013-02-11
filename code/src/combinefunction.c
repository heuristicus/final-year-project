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
				    double interval_time, double step, int num_estimates)
{
//    printf("estimates %p, time delay %p, interval_time %lf, step %lf, num estimates %d\n", estimates, time_delay, interval_time, step, num_estimates);
    if (estimates == NULL || time_delay == NULL || step <= 0 || num_estimates <= 0 ||
	interval_time <= 0)
	return NULL;

    double time;
    int estimate_num;
    double max_delay = largest_value_in_arr(time_delay->data, num_estimates);
    double end = interval_time - max_delay;

    printf("Combining function between %lf and %lf.\n", time, end);

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

    for (i = 0, time = max_delay; time <= end; time += step, ++i) {
	double total = 0;
	for (estimate_num = 0; estimate_num < num_estimates; ++estimate_num) {
		total += estimate_at_point(estimates[estimate_num], time - time_delay->data[estimate_num]);
	}
	combination->data[0][i] = time;
	combination->data[1][i] = total/num_estimates;
    }

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
					double start, double interval_time, double step, int num_vectors)
{
//    printf("start %lf, time %lf, step %lf, nvec %d\n", start, interval_time, step, num_vectors);
//    printf("time delay for function 0 is %lf, 1 is %lf\n", time_delay->data[0], time_delay->data[1]);

    double max_delay = largest_value_in_arr(time_delay->data, num_vectors);
//    printf("max delay %lf\n", max_delay);
    int memsize = (int)(interval_time - start - 2 * max_delay) / step + 1;
//    printf("memsize is %d\n", memsize);
    double_multi_arr* res = init_multi_array(2, memsize);
    
    double current = max_delay, end = interval_time - start - max_delay, sum;
    printf("Combining function between %lf and %lf.\n", current, end);
    int i, vector_num;
    for (i = 0; current <= end; ++i, current += step) {
//	printf("current is %lf\n", current);
	sum = 0;
	for (vector_num = 0; vector_num < num_vectors; ++vector_num) {
		sum += sum_gaussians_at_point(current - time_delay->data[vector_num], V[vector_num]);
//	    printf("summing vector %d at %lf\n", vector_num, current - time_delay->data[vector_num]);
//	    printf("sum is now %lf\n", sum);
	}
	res->data[0][i] = current;
	// must apply the same normalisation that is being applied to the gaussians in the estimate?
	// does not apply if the normalisation is calculated later and then applied.
	res->data[1][i] = sum / num_vectors;
    }

    return res;
}
/*
 * Combines two or more function estimates to produce a single estimate.
 * Takes an array of function estimates from different streams, and the
 * time delay between each. The average value of these functions is taken.
 * Functions are summed at all points in the interval, even if no value
 * for the function is specified at that point.
 */
double_multi_arr* combine_functions_all(est_arr** estimates, double_arr* time_delay, 
					double start, double interval_time, double step, int num_estimates)
{
    if (estimates == NULL || time_delay == NULL || step <= 0 || num_estimates <= 0 ||
	interval_time <= 0)
	return NULL;

    double time;
    int estimate_num;
    double end = start + interval_time;

    // need to fix this to work with combinations at any point and
    // any interval. This currently only works for stuff which starts
    // at zero and when the delay is non-negative.
    int memsize = (int)(interval_time / step + 1);
    double_multi_arr* combination = init_multi_array(2, memsize);

    /*
     * Only have data for all functions in the intervals in which they
     * are all active, so we must limit the times being checked to this interval.
     * The first point at which we have data for all functions in the same place is
     * where the maximum delay is. The last time we have data for all functions is 
     * where the least delayed function ends (interval_time - max_delay).
     */
    int i;

    for (i = 0, time = start; time <= end; time += step, ++i) {
	double total = 0;
	for (estimate_num = 0; estimate_num < num_estimates; ++estimate_num) {
	    total += estimate_at_point(estimates[estimate_num], time - time_delay->data[estimate_num]);
	}
	combination->data[0][i] = time;
	combination->data[1][i] = total/num_estimates;
    }

    return combination;
}

/*
 * Combines a set of vectors of gaussians into a single set of values representing
 * the average value of the sums of the gaussians. Samples are taken with the 
 * given resolution. The return array of the function will contain values for the
 * whole interval given, even if one of the functions does not have a value specified
 * for some part of the interval.
 */
double_multi_arr* combine_gauss_vectors_all(gauss_vector** V, double_arr* time_delay,
					double start, double interval_time, double step, int num_vectors)
{
//    printf("start %lf, time %lf, step %lf, nvec %d\n", start, interval_time, step, num_vectors);
//    printf("time delay for function 0 is %lf\n", time_delay->data[0]);

    int memsize = (int)(interval_time / step + 1);
//    printf("memsize is %d\n", memsize);
    double_multi_arr* res = init_multi_array(2, memsize);
    
    double current = start, end = start + interval_time, sum;
//    printf("Combining function between %lf and %lf.\n", current, end);
    int i, vector_num;
    for (i = 0; current <= end; ++i, current += step) {
//	printf("current is %lf\n", current);
	sum = 0;
	for (vector_num = 0; vector_num < num_vectors; ++vector_num) {
	    sum += sum_gaussians_at_point(current - time_delay->data[vector_num], V[vector_num]);
//	    printf("summing vector %d at %lf\n", vector_num, current - time_delay->data[vector_num]);
//	    printf("sum is now %lf\n", sum);
	}
	res->data[0][i] = current;
	// must apply the same normalisation that is being applied to the gaussians in the estimate?
	// does not apply if the normalisation is calculated later and then applied.
	res->data[1][i] = sum / num_vectors;
    }

    return res;
}
