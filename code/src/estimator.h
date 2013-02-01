#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "paramlist.h"
#include "math_util.h"
#include "file_util.h"
#include "general_util.h"

#define EST_TYPE_ERROR "%s is not a valid estimator. Try -a [ols|iwls|pc|base].\n"

est_arr* estimate(char* paramfile, char* infile, char* outfile, char* estimator_type);
int has_required_params(paramlist* params, char** required_params, int len);
est_arr* run_ols(paramlist* params, char* infile, char* outfile);
est_arr* run_iwls(paramlist* params, char* infile, char* outfile);
est_arr* run_pc(paramlist* params, char* infile, char* outfile);
est_arr* run_base(paramlist* params, char* infile, char* outfile);
double** run_gauss(paramlist* params, char* infile, char* outfile);

// Use these rather than the functions below.
est_arr* estimate_OLS(paramlist* params, char *infile, char *outfile);
est_arr* estimate_IWLS(paramlist* params, char *infile, char *outfile);
est_arr* estimate_piecewise(paramlist* params, char *event_file, char *output_file);
est_arr* estimate_baseline(paramlist* params, char *event_file, char *output_file);
void estimate_gaussian(paramlist* params, char* infile, char* outfile);

// Easier to use functions above than these.
est_arr* _estimate_baseline(char *event_file, char *output_file, double interval_start, 
			    double interval_end, int IWLS_iterations, int IWLS_subintervals,
			    int max_breakpoints, double max_extension, double min_interval_proportion,
			    double pmf_threshold, double pmf_sum_threshold);
est_arr* _estimate_piecewise(char* event_file, char* output_file, double interval_start, 
			    double interval_end, int IWLS_iterations, int IWLS_subintervals, 
			     int max_breakpoints, double max_extension, double min_interval_proportion,
			     double pmf_threshold, double pmf_sum_threshold);
est_arr* _estimate_OLS(char* infile, char* outfile, double start_time, double end_time,
			int num_subintervals);
est_arr* _estimate_IWLS(char* infile, char* outfile, double start_time, double end_time,
			 int num_subintervals, int iterations);
void _estimate_gaussian(char* infile, char* outfile, double start,
			double interval_length, double stdev, double resolution);

double** get_subintervals(double start_time, double end_time, int num_subintervals);
void free_pointer_arr(void **arr, int length);

#endif
