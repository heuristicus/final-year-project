#include "poisson.h"

long double fact(int i)
{
    if (i == 0)
	return 1;
    else
	return i * fact(i - 1);
}

/* calculates the probabilty of there being k events between time t_start and t_end. (for homogenous processes)*/
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k)
{
    double tau = t_end - t_start;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}

/* Outputs the sum of events in a specific time period to the array provided. The contents of the array will
 * be wiped. Returns the last array location that contains a value.
 */
int rolling_window(double *event_times, int num_events, double start_time, double timespan, int *output_array)
{
    int i, spanmult = 0;
    double time;

    for (i = 0, time = event_times[i]; i < num_events; ++i, time = event_times[i]){
	while (time > timespan * (spanmult + 1)){
	    spanmult++;
	}
	
	//printf("span %d, val %lf\n", spanmult, event_times[i]);
	//printf("output arr %d\n", output_array[spanmult]);
	output_array[spanmult]++;
	
    }

    return spanmult;
    
}
