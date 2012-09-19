#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

// poisson.c
double generate_event_time(double lambda);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);

// math_functions.c
int fact(int i);

// output_data.c
void run_trials(int times_to_run, double lambda, double time_span);
void print_num_events_in_second(int events_each_sec[]);
void print_detailed(int events_each_sec[], double event_times[]);

