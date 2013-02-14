#include "estimator.h"
#include "general_util.h"
#include <stdio.h>
#include "struct.h"

double_multi_arr* combine_functions(est_arr** estimates, double_arr* time_delay,
				    double start, double interval_time, double step, int num_estimates);
double_multi_arr* combine_gauss_vectors(gauss_vector** V, double_arr* time_delay,
					double start, double interval_time, double step, int num_vectors);
