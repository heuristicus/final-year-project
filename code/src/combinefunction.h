#include "estimator.h"
#include "general_util.h"
#include <stdio.h>
#include "struct.h"

double_multi_arr* combine_functions(est_arr** estimates, double* time_delay, 
			  double interval_time, int num_estimates, double step);
