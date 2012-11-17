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
double homogeneous_time(double lambda);
void generate_event_times_homogeneous(double lambda, double time, int max_events, double *event_times);
void run_to_event_limit_non_homogeneous(muParserHandle_t hparser, double lambda, double start_time, int max_events, double *event_times, double *lambda_vals);
int run_to_time_non_homogeneous(muParserHandle_t hparser, double lambda, double time_delta, double start_time, double time_to_run, double **event_times, double **lambda_vals, int arr_len);
void run_time_nonhom(muParserHandle_t hparser, double lambda, double time_delta, double start_time, double runtime, char *outfile, int outswitch);
void run_events_nonhom(muParserHandle_t hparser, double lambda, double time_delta, int events, char *outfile, int outswitch);
void run_time_nstreams(muParserHandle_t hparser, double lambda, double runtime, double *time_delta, int nstreams, char *outfile, int outswitch);
void generate(char **args);

#endif
