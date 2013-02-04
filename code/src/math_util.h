#ifndef _MATHUTIL_H
#define _MATHUTIL_H

#include "general_util.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

long double fact(int n);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
int* sum_events_in_interval(double *event_times, int num_events, double start_time,
			    double end_time, int num_subintervals);
double get_poisson_noise(double mean);
double get_gaussian_noise(double mean, double std_dev);
double* get_interval_midpoints(double start_time, double end_time, int num_subintervals);
double get_interval_midpoint(int interval_index, double start_time, double end_time,
			     int num_subintervals);
void init_rand(double seed);
double get_uniform_rand();
double get_rand_gaussian();

double avg(double *arr, int len);
double TSS(double *dependent_variables, int len);
double ESS(double *estimates, double *dependent_variables, int len);
double RSS(double *dependent_variables, double *independent_variables,
	   double (*f)(double), int len);
double sum_double_arr(double *arr, int len);
int sum_int_arr(int *arr, int len);

double evaluate_function(double a, double b, double x);
double get_intercept(double x, double y, double gradient);
double get_gradient(double a_x, double a_y, double b_x, double b_y);
double get_midpoint(double a, double b);
double* get_intercept_and_gradient(double a_x, double a_y, double b_x, double b_y);

gaussian* make_gaussian(double mean, double stdev);
double gaussian_contribution_at_point(double x, gaussian* g, double weight);
double** gaussian_contribution(gaussian* g, double start, double end,
			       double step, double weight);
double sum_gaussians_at_point(double x, gauss_vector* G);
double_multi_arr* gauss_transform(gauss_vector* G, double start, double end,
				  double resolution);
double_multi_arr* shifted_transform(gauss_vector* V, double start, double interval,
				    double step, double resolution);
double* random_vector(int len, double multiplier);
double* weight_vector(double weight, int len);
gauss_vector* gen_gaussian_vector_uniform(double stdev, double start,
					  double interval_time, double step,
					  double multiplier);
gauss_vector* gen_gaussian_vector_from_array(double* means, int len, double stdev,
					     double multiplier, int random_weights);
double find_min_value(double* data, int len);
double* add_to_arr(double* data, int len, double add);
double** kernel_density(double* events, int len, double start, double end,
			double bandwidth, double resolution);
double kernel_density_at_point(double* events, int len, int x, double bandwidth);
double gaussian_kernel(double x, double mean, double stdev);
int find_min_value_int(int* data, int len);
double find_max_value(double* data, int len);
double* multiply_arr(double* data, int len, double multiplier);

#endif

