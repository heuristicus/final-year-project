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
    double max_delay = 0;
        
    int i;

    for (i = 0; i < num_estimates; ++i) {
	max_delay = time_delay[i] > max_delay ? time_delay[i] : max_delay;
    }

    double_multi_arr* combination = malloc(sizeof(double_multi_arr));
    int memsize = (int)(((interval_time - max_delay) - max_delay)/ step + 1);
    combination->data = malloc(sizeof(double*) * 2);
    combination->data[0] = malloc(sizeof(double) * memsize);
    combination->data[1] = malloc(sizeof(double) * memsize);
    combination->len = 2;
    combination->lengths = malloc(sizeof(int) * 2);
    combination->lengths[0] = memsize;
    combination->lengths[1] = memsize;

    /*
     * Only have data for all functions in the intervals in which they
     * are all active, so we must limit the times being checked to this interval.
     * The first point at which we have data for all functions in the same place is
     * where the maximum delay is. The last time we have data for all functions is 
     * where the least delayed function ends (interval_time - max_delay).
     */

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
double_arr* combine_gauss_vectors(gauss_vector** V, int num)
{
    return NULL;
}
