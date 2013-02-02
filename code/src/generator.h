#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <muParserDLL.h>
#include "math_util.h"
#include "struct.h"

// generator.c
double homogeneous_time(double lambda);
void generate_event_times_homogeneous(double lambda, double time, int max_events,
				      double *event_times);
void run_to_event_limit_non_homogeneous(muParserHandle_t hparser, double lambda,
					double start_time, int max_events,
					double *event_times, double *lambda_vals);
int run_to_time_non_homogeneous(muParserHandle_t hparser, double lambda,
				double time_delta, double start_time, 
				double time_to_run, double **event_times,
				double **lambda_vals, int arr_len);
void run_time_nonhom(muParserHandle_t hparser, double lambda, double time_delta,
		     double start_time, double runtime, char *outfile, int outswitch);
void run_events_nonhom(muParserHandle_t hparser, double lambda, double time_delta,
		       int events, char *outfile, int outswitch);
void run_time_nstreams(muParserHandle_t hparser, double lambda, double runtime,
		       double *time_delta, int nstreams, char *outfile, int outswitch);
void generate(char *paramfile, char *outfile, int nruns);
gauss_vector* _generate_gaussian(char* infile, double stdev, double start, double interval,
			 double gen_step, double resolution);
double_multi_arr* nonhom_from_gaussian(gauss_vector* G, double lambda, 
				       double start, double interval, double time_delta, double shift);
void generate_from_gaussian(char* paramfile, char* outfile, char* infile, int nstreams);

void generate_gaussian_data(char* paramfile, char* infile, char* outfile, int number, int output_type);

#endif
