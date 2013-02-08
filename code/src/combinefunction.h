#include "estimator.h"
#include "general_util.h"
#include <stdio.h>
#include "struct.h"

double_multi_arr* combine_functions(est_arr** estimates, double_arr* time_delay, 
			  double interval_time, int num_estimates, double step);
est_data* data_at_point(est_arr* estimate, double check_time);
double estimate_at_point(est_arr* estimate, double time);
