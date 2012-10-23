#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <muParserDLL.h>

#ifndef _GENERATOR_H
#define _GENERATOR_H

// start.c
char** parse_args(int argc, char *argv[]);
int valid(char *sw);
void handle_arg(char *sw, char **args, char *arg);

// generator.c
double homogenous_time(double lambda);
void init_rand(void);
void generate_event_times_homogenous(double lambda, double time, int max_events, double *event_times);
void run_to_event_limit_non_homogenous(muParserHandle_t hparser, double lambda, double start_time, int max_events, double *event_times, double *lambda_vals);
int run_to_time_non_homogenous(muParserHandle_t hparser, double lambda, double start_time, double time_to_run, double **event_times, double **lambda_vals, int arr_len);
void run_time_nonhom(muParserHandle_t hparser, double lambda, double start_time, double runtime, char *outfile, int outswitch);
void run_events_nonhom(muParserHandle_t hparser, double lambda, double start_time, int events, char *outfile, int outswitch);
void run_time_nstreams(muParserHandle_t hparser, double lambda, double runtime, double *time_delta, int nstreams, char *outfile, int outswitch);
void initialise_generator(char **args);

// math_functions.c
long double fact(int i);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
int rolling_window(double *event_times, int num_events, double start_time, double timespan, int *output_array);

#endif
