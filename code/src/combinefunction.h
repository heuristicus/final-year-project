#include "estimator.h"
#include "struct.h"

est_arr* combine_function(est_arr** estimates, double* time_delay, double interval_time, int num_estimates);
est_data* data_at_point(est_arr* estimate, double check_time);
double estimate_at_point(est_arr* estimate, double time);
