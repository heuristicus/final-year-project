#include "poisson_random.h"

#define NUM_TRIALS 6
#define TIME_SPAN 10.0

int main(int argc, char *argv[])
{
    srand48(time(NULL));
    double lambda = 10;
    
    int times_to_run[NUM_TRIALS] = {10, 100, 1000, 10000, 100000, 1000000};
            
    int events_total, events_run, i, j;
    double run_time;
    for (i = 0, events_total = 0; i < NUM_TRIALS; ++i, events_total = 0){
	for (j = 0, events_run = 0; j < times_to_run[i]; ++j, events_run = 0){
	    for (run_time = 0; run_time < TIME_SPAN;){
		run_time += generate_event_time(lambda);
		events_run++;
	    }
	    events_total += events_run;
	}
	printf("Running %d times, average number of events: %.3lf\n", times_to_run[i], (double) events_total / (double) times_to_run[i]);
    }
    
    return 0;
}

double generate_event_time(double lambda)
{
    return -log(drand48()) / lambda;
}
