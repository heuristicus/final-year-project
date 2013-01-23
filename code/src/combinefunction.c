#include "combinefunction.h"
#include <stdio.h>

/*
 * Combines two or more function estimates to produce a single estimate.
 * Takes an array of function estimates from different streams, and the
 * time delay between each. The average value of the functions at some
 * points is calculated, and this 
 */
est_arr* combine_function(est_arr** estimates, double* time_delay, double interval_time, int num_estimates)
{
    FILE* fp = fopen("comb.txt", "w");

    int time;
    int estimate_num;
    double max_delay = 0;
    
    int i;

    for (i = 0; i < num_estimates; ++i) {
	max_delay = time_delay[i] > max_delay ? time_delay[i] : max_delay;
    }

    /*
     * Only have data for all three functions in the intervals in which they
     * are all active, so we must limit the times being checked to this interval.
     * The first point at which we have data for all functions in the same place is
     * where the maximum delay is. The last time we have data for all functions is 
     * where the least delayed function ends (interval_time - max_delay).
     */
    for (time = max_delay; time < interval_time - max_delay; ++time) {
	printf("time is %d\n", time);
	double total = 0;
	for (estimate_num = 0; estimate_num < num_estimates; ++estimate_num) {
	    printf("time delay %lf\n", time_delay[estimate_num]);
	    total += estimate_at_point(estimates[estimate_num], time - time_delay[estimate_num]);
	    printf("total is %lf\n", total);
	}
	fprintf(fp, "%d %lf\n", time, total/num_estimates);
    }

    fclose(fp);
    
    return NULL;
}

/*
 * calculates the value of the estimate at a given point in time.
 */
double estimate_at_point(est_arr* estimate, double time)
{
    est_data* idata = data_at_point(estimate, time);
    
    return idata->est_a + idata->est_b * time;
}

/*
 * Returns the data from the given estimate that can be used to calculate
 * the function value at the specified time.
 */
est_data* data_at_point(est_arr* estimate, double check_time)
{
    est_data* current = NULL;
    est_data* ret = NULL;
        
    int i;
    
    for (i = 0; i < estimate->len; ++i) {
	current = estimate->estimates[i];
	if (current->start <= check_time && check_time <= current->end){
	    ret = current;
	}
    }
    return ret;
}
