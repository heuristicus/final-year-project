#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

double** get_subintervals(double time, int subintervals);
void free_pointer_arr(void **arr, int length);
double get_interval_midpoint(int interval_number, double time, int subintervals);
double* initialise_weights(int subintervals);
double a_estimate();
double b_estimate();
double beta_estimate();
double alpha_estimate();
double weight_estimate();
double lambda_estimate();
double* get_interval_midpoints(double time, int subintervals);
double mean_x(double *weights, int *midpoints, int size);
double mean_Y();
double SSE(double *rand_var, double *weights, double *midpoints, double alpha, double beta, int size);
int* get_bin_counts(char *filename, double interval_time, int interval_num);
double* initialise_random_variables(int *bin_counts, int length);


#endif
