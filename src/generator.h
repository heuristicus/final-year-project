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
#include "paramlist.h"
#include "general_util.h"

// generator.c
double homogeneous_time(double lambda);
double_arr* generate_event_times_homogeneous(double lambda, double time);
void run_to_event_limit_non_homogeneous(muParserHandle_t hparser, double lambda,
					double start_time, int max_events,
					double* event_times, double* lambda_vals);
double_arr* run_to_time_non_homogeneous(muParserHandle_t hparser, double lambda,
				double time_delta, double start_time, 
				double time_to_run, int arr_len);
void run_time_nonhom(muParserHandle_t hparser, double lambda, double time_delta,
		     double start_time, double runtime, char* outfile, int outswitch,
		     double step, double output_interval);
void run_events_nonhom(muParserHandle_t hparser, double lambda, double time_delta,
		       int events, char* outfile, int outswitch);
void run_time_nstreams(muParserHandle_t hparser, double lambda, double runtime,
		       double_arr* time_delta, int nstreams, char* outfile,
		       int outswitch, double step, double output_interval);
void generate(char* paramfile, char* outfile, int nfuncs, int nstreams, int output_switch);
void _generate(paramlist* params, char* outfile, double interval_time, double lambda, 
	       muParserHandle_t hparser, double_arr* time_delta, int
	       output_switch, int nstreams, double step, double output_interval);
gauss_vector* _generate_gaussian(char* infile, double stdev, double start,
				 double interval, double gen_step,
				 double resolution, double multiplier);
double_multi_arr* nonhom_from_gaussian(gauss_vector* G, double lambda, 
				       double start, double interval,
				       double time_delta, double shift);
void generate_from_gaussian(char* paramfile, char* outfile, char* infile,
			    int nfuncs, int nstreams, int output_type, int copy_base);
void _generate_from_gaussian(paramlist* params, char* outfile, char* infile,
			     double stdev, double start, double interval,
			     double step, double resolution, char* stream_ext,
			     double_arr* time_delta, int nstreams, int output_type);
void generate_gaussian_data(char* paramfile, char* infile, char* outfile,
			    int number, int output_type);

#endif
