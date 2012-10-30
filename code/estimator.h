#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

double** get_subintervals(double interval_time, int num_subintervals);
void free_pointer_arr(void **arr, int length);
double get_interval_midpoint(int interval_index, double interval_time, int num_subintervals);
double* initialise_weights(int num_subintervals);
double a_estimate(double alpha, double interval_time, int num_subintervals);
double b_estimate(double beta, double interval_time, int num_subintervals);
double beta_estimate(double* weights, double *midpoints, int *bin_counts, double mean_x, int num_subintervals);
double alpha_estimate(double mean_y, double mean_x, double beta_estimate);
double* get_interval_midpoints(double interval_time, int num_subintervals);
double SSE(double *weights, double *midpoints, int *bin_counts, double alpha, double beta, int num_subintervals);
int* get_bin_counts(char *filename, double interval_time, int num_subintervals);
double mean_x(double *midpoints, double *weights, int num_subintervals);
double mean_Y(int *bin_counts, double *weights, int num_subintervals);
double* initialise_random_variables(int *bin_counts, int length);
double constraint_a(double *weights, double *midpoints, int *bin_counts, double interval_time, int num_subintervals);
double constraint_b(double *weights, double *midpoints, int *bin_counts, double interval_time, int num_subintervals);
double* lambda_estimate(double *midpoints, double a, double b, double interval_time, int num_subintervals);
void weight_estimate(double *weights, double *lambda, int num_subintervals);


#endif
