#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

double** get_subinterval_data(double interval_time, int num_subintervals);
double get_interval_midpoint(int interval_number, double interval_time, int num_subintervals);
double* initialise_weights(int num_subintervals);
double a_estimate(double alpha, double interval_time, int num_subintervals);
double b_estimate(double beta, double interval_time, int num_subintervals);
double beta_estimate(double* weights, double *midpoints, double *rand_var, double mean_x, int num_subintervals);
double alpha_estimate(double mean_y, double mean_x, double beta_estimate);
double mean_x(double *midpoints, double *weights, int num_subintervals);
double SSE(double *rand_var, double *weights, double *midpoints, double alpha, double beta, int size);
int* get_bin_counts(char *filename, double interval_time, int num_subintervals);
double mean_Y(double *random_variables, double *weights, int num_subintervals);
double* initialise_random_variables(double *midpoints, int num_subintervals, double alpha, double beta);
double constraint_a(double *weights, double *midpoints, double *random_variables, double interval_time, int num_subintervals);
double constraint_b(double *weights, double *midpoints, double *random_variables, double interval_time, int num_subintervals);
void randvar_estimate(double *random_variables, double *midpoints, double a, double b, double interval_time, int num_subintervals);
void weight_estimate(double *weights, double *random_variables, int num_subintervals);


#endif
