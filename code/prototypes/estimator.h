#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

// estimatorIWLS.c
double** get_subintervals(double time, int subintervals);
void free_pointer_arr(void **arr, int length);
double get_interval_midpoint(int interval_number, double time, int subintervals);
double* initialise_weights(int subintervals);

#endif
