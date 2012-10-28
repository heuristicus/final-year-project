#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

double** get_subintervals(double time, int subintervals);
void free_pointer_arr(void **arr, int length);
double get_interval_midpoint(int interval_number, double time, int subintervals);
double* initialise_weights(int subintervals);
double a_estimate(double alpha, double total_time, int intervals);
double b_estimate(double beta, double total_time, int intervals);
double beta_estimate(double* weights, int* bin_counts, double *rand_var, double mean_x, int subintervals);
double alpha_estimate(double mean_y, double mean_x, double beta_estimate);
double weight_estimate();
double lambda_estimate();
double* get_interval_midpoints(double time, int subintervals);
double mean_x(int *bin_counts, double *weights, int size);
double mean_Y();
double SSE(double *rand_var, double *weights, int *bin_counts, double alpha, double beta, int size);
int* get_bin_counts(char *filename, double interval_time, int interval_num);
double mean_Y(double *random_variables, double *weights, int size);
double* initialise_random_variables(int *bin_counts, int length);
double constraint_a(double *weights, int *bin_counts, double *random_variables, double time, int intervals);
double constraint_b(double *weights, int *bin_counts, double *random_variables, double time, int intervals);

#endif
