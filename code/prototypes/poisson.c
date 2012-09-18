#include "poisson_random.h"

#define MAX_TIME 10
#define MAX_EVENTS 1000

double run_time = 0;
int events = 0;

int main(int argc, char *argv[])
{
    srand48(time(NULL));
    double lambda = 5.0;
    extern double run_time;
    extern int events;
    
    int events_in_sec[MAX_TIME] = {0};
    double *e_times, event_time;
    if ((e_times = calloc(MAX_EVENTS, sizeof(double))) == NULL){
	printf("Could not allocate memory for event time array. Exiting.\n");
	exit(1);
    }
    
    while (run_time < MAX_TIME){
	e_times[events] = event_time = generate_event_time(lambda);
	//printf("event time arrloc: %d\n", (int) event_time);
	events_in_sec[(int) event_time]++;
    }

    int i, j, num_events_so_far = 0;
        

    // Display detailed data on a run
    for (i = 0; i < MAX_TIME; ++i){
    	printf("events in second %d: %d\n", i, events_in_sec[i]);
    	printf("times of events this second:\n");
    	for (j = 0; j < events_in_sec[i]; ++j){
    	    printf("%lf\n", e_times[num_events_so_far + j]);
    	}
    	num_events_so_far += events_in_sec[i];
    	printf("events so far: %d\n", num_events_so_far);
    	putchar('\n');
    }

    printf("Total events: %d\n", events);
    
    // Display only the second and the number of events in that second
    for (i = 0, num_events_so_far = 0; i < MAX_TIME; ++i){
    	num_events_so_far += events_in_sec[i];
    	printf("%d %d\n", i, num_events_so_far);
    }
    
    free(e_times);
    
    return 0;
    
}

/* knuth method */
double generate_event_time(double lambda)
{
    extern double run_time;
    extern int events;
    events++;
    
    run_time += -log(drand48()) / lambda;
    
    return run_time;
}
