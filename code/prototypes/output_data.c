#include "poisson_random.h"

#define MAX_EVENTS 1000

double *e_times;
int *events_in_sec;

int main(int argc, char *argv[])
{
    double lambda = 5.0, runtime = 10.0;
    int events;
    
    
    events = initialise(lambda, runtime);
    //print_detailed(events_in_sec, e_times, runtime, lambda);
    //print_num_events_in_second(events_in_sec, runtime);
    print_event_times(e_times, events, lambda);
        
    //printf("prob: %.15lf\n", prob_num_events_in_time_span(1.0, 2.0, 5.0, 6));
    free(e_times);
    free(events_in_sec);
        
    return 0;
}

/* initialises the e_times and events_in_sec arrays by generating events for time_to_run seconds. Returns the total number of events generated. Also seeds the random number generator. */
int initialise(double lambda, double time_to_run)
{
    events_in_sec = calloc((int) time_to_run, sizeof(int));
    
    int i;
    
    for (i = 0; i < time_to_run; ++i)
	events_in_sec[i] = 0;
        
    int events = 0;
    double run_time = 0.0;
    
    if ((e_times = calloc(MAX_EVENTS, sizeof(double))) == NULL){
	printf("Could not allocate memory for event time array. Exiting.\n");
	exit(1);
    }
    
    init_rand();
    
    while ((run_time += generate_event_time(lambda)) < time_to_run){
	e_times[events] = run_time;
	events_in_sec[(int) run_time]++;
	events++;
    }    

    return events;
}

/* peforms the specified number of trials. Calls the generate event method until the total time elapsed is greater than time_span and totals events in the time elapsed. This is repeated times_to_run times, and the average number of events is calculated. */
void run_trials(int times_to_run, double lambda, double time_span)
{
    int events_total, events_run, i;
    double run_time;

    for (i = 0, events_run = 0; i < times_to_run; ++i, events_run = 0){
	for (run_time = 0; run_time < time_span;){
	    if ((run_time += generate_event_time(lambda)) > time_span)
		break;
	    	    events_run++;
	}
	events_total += events_run;
    }
    printf("Running %d times, average number of events: %.3lf\n", times_to_run, (double) events_total / (double) times_to_run);
}

void print_event_times(double event_times[], int events, double lambda)
{
    int i;
    
    for (i = 0; i < events; ++i){
	printf("%lf %lf\n", lambda, event_times[i]);
    }
}

/* prints the second, and the number of events that occurred in that second. */
void print_num_events_in_second(int events_each_sec[], double time_run)
{
    int num_events_so_far, i;
        
    for (i = 0, num_events_so_far = 0; i < time_run; ++i){
    	num_events_so_far += events_each_sec[i];
    	printf("%d %d\n", i, num_events_so_far);
    }    
}


/* displays detailed data about a run */
void print_detailed(int events_each_sec[], double event_times[], double time_run, double lambda)
{
    int i, j, num_events_so_far = 0;
        
    for (i = 0; i < time_run; ++i){
    	printf("events in second %d: %d\n", i, events_each_sec[i]);
	printf("probability of this happening %.10lf\n", prob_num_events_in_time_span((double) i, (double) i + 1, lambda, events_each_sec[i]));
    	printf("times of events this second:\n");
    	for (j = 0; j < events_each_sec[i]; ++j){
    	    printf("%lf\n", event_times[num_events_so_far + j]);
    	}
    	num_events_so_far += events_each_sec[i];
    	printf("events so far: %d\n", num_events_so_far);
    	putchar('\n');
    }

}
