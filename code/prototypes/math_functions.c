#include "poisson_random.h"

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

int rolling_window(double *event_times, int num_events, double timespan, int *output_array)
{
    int i, spanmult = 0;
    double time;
    
    printf("events %d, timespan %lf, array locs: %lf\n", num_events, timespan, num_events / timespan);
    for (i = 0; i < num_events; ++i, time = event_times[i]){
	printf("time/timespan %lf\n", time / (timespan * (spanmult + 1)));
	if (time < timespan * (spanmult + 1)){
	    printf("time %lf, timespan %lf, spanmult %d\n", time, timespan * spanmult, spanmult);
	    printf("incrementing output %d\n", spanmult);
	} else {
	    while (time > timespan * (spanmult + 1)){
		spanmult++;
	    }
	    printf("incremented spanmult\n");
	    printf("time %lf, timespan %lf, spanmult %d\n", time, timespan * (spanmult + 1), spanmult);
	    printf("incrementing output %d\n", spanmult);
	}

    }

    return i;
    
}
