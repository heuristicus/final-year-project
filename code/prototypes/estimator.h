#include <stdio.h>
#include <stdlib.h>

// math_util.c
long double fact(int i);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
int rolling_window(double *event_times, int num_events, double start_time, double timespan, int *output_array);

// estimatorIWLS.c
double** get_subintervals(double time, int subintervals);
void free_pointer_arr(void **arr, int length);

