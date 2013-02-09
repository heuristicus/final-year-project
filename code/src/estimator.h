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

#define EST_TYPE_ERROR "%s is not a valid estimator. Try -a [ols|iwls|pc|base|gauss].\n"

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
double_multi_arr* estimate_gaussian(paramlist* params, char* infile, char* outfile);

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
double_multi_arr* _estimate_gaussian(char* infile, char* outfile, double start,
			double interval_length, double stdev, double resolution);

double** get_subintervals(double start_time, double end_time, int num_subintervals);
void free_pointer_arr(void **arr, int length);
gauss_vector* _n_estimate_gaussian(char* infile, char* outfile, double start,
				   double interval_length, double stdev);
gauss_vector* n_estimate_gaussian(paramlist* params, char* infile, char* outfile);

double area_at_point_gauss(gauss_vector* f1, gauss_vector* f2, double x, double delay);
double area_at_point_base(est_arr* f1, est_arr* f2, double x, double delay);
double estimate_delay_area(void* f1, void* f2, double max_delay,double resolution,
			   double step, char* type);
double estimate_delay_pmf(void* f1, void* f2, double_arr* events, double max_delay,
			  double resolution, double step, char* type);
double total_area_estimate(void* f1, void* f2, double start,
			   double end, double resolution, double delay, char* type);

/* double total_area_estimate_base(est_arr* f1, est_arr* f2, double start, */
/* 				double end, double resolution, double delay); */
/* double total_area_estimate_gauss(gauss_vector* f1, gauss_vector* f2, double start, double end, */
/* 			   double resolution, double delay); */
/* double estimate_delay_area_gauss(gauss_vector* f1, gauss_vector* f2, */
/* 				    double max_delay, double resolution); */


/* double estimate_delay_area_baseline(est_arr* f1, est_arr* f2, */
/* 				    double max_delay, double resolution); */

/* double estimate_delay_discrete_area(double_multi_arr* f1, double_multi_arr* f2, */
/* 				    double max_delay); */
/* double total_area_estimate(double_multi_arr* f1, double_multi_arr* f2, double delay); */

#endif
