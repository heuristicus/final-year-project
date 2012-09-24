#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

// pfunc.c
double homogenous_time(double lambda);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
void init_rand(void);
double non_homogenous_time(double eqsolution);
double* generate_event_times_homogenous(double lambda, double time, int max_events);

// math_functions.c
long double fact(int i);

// output_data.c
void run_trials(int times_to_run, double lambda, double time_span);
void print_num_events_in_second(int events_each_sec[], double time_run);
void print_detailed(int events_each_sec[], double event_times[], double time_run, double lambda);
int initialise(double lambda, double time_to_run);
void print_event_times(double event_times[], int events, double lambda);
