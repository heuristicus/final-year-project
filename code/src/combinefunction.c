#include "combinefunction.h"

/*
 * Combines two or more function estimates to produce a single estimate.
 * Takes an array of function estimates from different streams, and the
 * time delay between each.
 */
est_arr* combine_function(est_arr** estimates, double* time_delay)
{
    return NULL;
}

/*
 * Returns the estimate data for the given estimate at a specific time.
 */
est_data* estimate_at_point(est_arr* estimate, double check_time)
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
