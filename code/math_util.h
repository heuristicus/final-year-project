#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gmp.h>

#ifndef _MATHUTIL_H
#define _MATHUTIL_H

long double fact(int n);
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k);
int* sum_events_in_interval(double *event_times, int num_events, double start_time, double end_time, int num_subintervals);
double get_poisson_noise(double mean);
double get_gaussian_noise(double mean, double std_dev);
double rand_gauss(void);
double* get_interval_midpoints(double start_time, double end_time, int num_subintervals);
double get_interval_midpoint(int interval_index, double start_time, double end_time, int num_subintervals);
void init_rand(double seed);
double avg(double *arr, int len);
double TSS(double *dependent_variables, int len);
double ESS(double *estimates, double *dependent_variables, int len);
double RSS(double *dependent_variables, double *independent_variables, double (*f)(double), int len);
double poisson_PMF(double lambda, int k);
mpz_t* fast_fact(int n);

#endif
