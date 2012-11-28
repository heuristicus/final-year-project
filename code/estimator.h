#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

double* estimate_OLS(char *infile, char *outfile, double start_time, double interval_time, int num_subintervals);
double* estimate_IWLS(char *infile, char *outfile, double start_time, double interval_time, int num_subintervals, int iterations);
double** get_subintervals(double start_time, double end_time, int num_subintervals);
void free_pointer_arr(void **arr, int length);



#endif
