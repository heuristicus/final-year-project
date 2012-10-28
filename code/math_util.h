#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef _MATHUTIL_H
#define _MATHUTIL_H

long double fact(int i);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
int* sum_events_in_interval(double *event_times, int num_events, double interval_time, int interval_num);
double get_poisson_noise(double mean);
double get_gaussian_noise(double mean, double std_dev);
double rand_gauss(void);
double* get_interval_midpoints(double interval_time, int num_subintervals);
double get_interval_midpoint(int interval_index, double interval_time, int num_subintervals);

#endif
