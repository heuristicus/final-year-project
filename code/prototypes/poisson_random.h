#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include <muParserDLL.h>

// poisson_generator.c
double homogenous_time(double lambda);
void init_rand(void);
void generate_event_times_homogenous(double lambda, double time, int max_events, double* event_times);
void run_to_event_limit_non_homogenous(muParserHandle_t hparser, double lambda, int max_events, double *event_times, double *lambda_vals);
int run_to_time_non_homogenous(muParserHandle_t hparser, double lambda, double max_time, double** event_times, double** lambda_vals, int arr_len);

// math_functions.c
long double fact(int i);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
