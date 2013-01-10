#include <stdio.h>
#include <stdlib.h>

#ifndef _ESTIMATOR_H
#define _ESTIMATOR_H

#define EST_TYPE_ERROR "%s is not a valid estimator. Try -a [ols|iwls|pc|base].\n"


typedef struct
{
    double est_a;
    double est_b;
    double start;
    double end;
} est_data;

typedef struct
{
    est_data **estimates;
    int len;
} est_arr;

est_data* estimate_OLS(char *infile, char *outfile, double start_time, double interval_time,
		       int num_subintervals);
est_data* estimate_IWLS(char *infile, char *outfile, double start_time, double interval_time,
			int num_subintervals, int iterations);
est_arr* piecewise_estimate(char *event_file, char *output_file, double interval_start, 
			    double interval_end, double IWLS_iterations, double IWLS_subintervals,
			    double max_breakpoints, double max_extension);
est_arr** baseline_estimate(char *event_file, char *output_file, double interval_start, 
			   double interval_end, int IWLS_iterations, int IWLS_subintervals,
			   int max_breakpoints, double max_extension);

double** get_subintervals(double start_time, double end_time, int num_subintervals);
void free_pointer_arr(void **arr, int length);

#endif
