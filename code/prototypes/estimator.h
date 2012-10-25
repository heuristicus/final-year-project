#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

double** get_subintervals(double time, int subintervals);
void free_pointer_arr(void **arr, int length);
double get_interval_midpoint(int interval_number, double time, int subintervals);
double* initialise_weights(int subintervals);
double estimate_beta();
double estimate_alpha();
double* get_interval_midpoints(double time, int subintervals);
double mean_x(double *weights, double *midpoints, int size);
double mean_Y();
double SSE(double *rand_var, double *weights, double *midpoints, double alpha, double beta, int size);
double* get_bin_counts(double **intervals, int size, char *filename, double interval_length);

#endif
