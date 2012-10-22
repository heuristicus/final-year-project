#include <stdio.h>

long double fact(int i);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
int rolling_window(double *event_times, int num_events, double start_time, double timespan, int *output_array);

