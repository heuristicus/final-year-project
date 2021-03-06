#include "estimator.h"

double* get_breakpoint_midpoints(double* breakpoint_vector, double* func_vals, int len);
double* get_breakpoint_vector(est_data** pieces, int max_breakpoints);
double** recalculate_expressions(double* breakpoint_vector, double* function_values, int max_breakpoints);
double* get_func_vals_at_breakpoints(est_data** pieces, int max_breakpoints);
est_arr* recalculate_estimates(double* breakpoint_vector, double* function_values, int len);

/*
 * Helper function for baseline estimator. Extracts parameters from the parameter list and
 * passes them to the estimator.
 */
est_arr* estimate_baseline(paramlist* params, char *event_file, char *output_file)
{
    int subint = get_int_param(params, "base_iwls_subintervals");
    int iterations = get_int_param(params, "base_iwls_iterations");
    int breakpoints = get_int_param(params, "base_max_breakpoints");
    double max_extension = get_double_param(params, "base_max_extension");
    double start = get_double_param(params, "est_start_time");
    double end = get_double_param(params, "est_interval_time") + start;
    double min_interval_proportion = get_double_param(params, "base_min_interval_proportion");
    double pmf_threshold = get_double_param(params, "base_pmf_threshold");
    double pmf_sum_threshold = get_double_param(params, "base_pmf_sum_threshold");

    return _estimate_baseline(event_file, output_file, start, end, iterations, 
			      subint, breakpoints, max_extension, min_interval_proportion,
			      pmf_threshold, pmf_sum_threshold);
}
/*
 * Estimate the given event data using the baseline technique. A piecewise estimate of the data is made,
 * and the start and end points of consecutive estimates are moved so that they are joined at the midpoint
 * between them.
 */
est_arr* _estimate_baseline(char* event_file, char* output_file, double interval_start, 
			    double interval_end, int IWLS_iterations, int IWLS_subintervals,
			    int max_breakpoints, double max_extension, double min_interval_proportion, 
			    double pmf_threshold, double pmf_sum_threshold)
{
    est_arr* pieces = _estimate_piecewise(event_file, NULL, interval_start, interval_end,
					 IWLS_iterations,IWLS_subintervals, max_breakpoints,
					  max_extension, min_interval_proportion, pmf_threshold, 
					  pmf_sum_threshold);

//    print_estimates(pieces);
    
    int len = pieces->len;

#ifdef VERBOSE
    int i;
    for (i = 0; i < len; ++i) {
	printf("original function %d: a %lf, b %lf\n", i, pieces->estimates[i]->est_a, pieces->estimates[i]->est_b);
    }
#endif

    
    double* breakpoint_vector = get_breakpoint_vector(pieces->estimates, len);
    double* func_eval = get_func_vals_at_breakpoints(pieces->estimates, len);
    double* midpoints = get_breakpoint_midpoints(breakpoint_vector, func_eval, len);
    est_arr* new_est = recalculate_estimates(breakpoint_vector, midpoints, len);

    char* out = malloc(strlen(output_file) + strlen(".data") + 5);
    sprintf(out, "%s.dat", output_file);
        
    output_estimates(out, new_est->estimates, len);
    
#ifdef VERBOSE3
    printf("breakpoint values\n");
    for (i = 0; i < len + 1; ++i) {
    	printf("%d %lf\n", i, breakpoint_vector[i]);
    }

    printf("function values\n");
    for (i = 0; i < len * 2; ++i) {
    	printf("%d %lf\n", i, func_eval[i]);
    }

    printf("midpoint values\n");
    for (i = 0; i < len + 1; ++i) {
    	printf("%d %lf\n", i, midpoints[i]);
    }
#endif

    free_est_arr(pieces);
    free(breakpoint_vector);
    free(midpoints);
    free(func_eval);
    free(out);

    return new_est;
}

/*
 * Gets the x values (times) of the breakpoints given by the
 * piecewise estimator. Includes the start and end of the interval.
 */
double* get_breakpoint_vector(est_data** pieces, int num_breakpoints)
{
    double* breakpoints = malloc((num_breakpoints + 1) * sizeof(double));
    
    int i;
    
    for (i = 0; i < num_breakpoints && pieces[i] != NULL; ++i) {
#ifdef VERBOSE
	printf("pieces %d %lf\n", i, pieces[i]->start);
#endif
    	breakpoints[i] = pieces[i]->start;
    }
#ifdef VERBOSE
    printf("pieces %d %lf\n", i, pieces[i - 1]->end);
#endif
    breakpoints[i] = pieces[i - 1]->end; // add the end of the interval
    return breakpoints;
}

/*
 * Calculates the values of all the estimated functions at the breakpoints.
 * The function values will be calculated at the start and end of each
 * subinterval, which is marked by the breakpoints.
 */
double* get_func_vals_at_breakpoints(est_data** pieces, int num_breakpoints)
{
    int i, j;
    double* func_eval = malloc(num_breakpoints * 2 * sizeof(double));
        
    for (i = 0, j = 0; i < num_breakpoints + 1; ++i, j += 2) {
	if (pieces[i] == NULL)
	    break;
	
	/* printf("%lf, %lf, %lf, %lf\n", pieces[i]->est_a, pieces[i]->est_b, pieces[i]->start, pieces[i]->end); */
	/* printf("function at interval start (%lf): %lf\n ", pieces[i]->start, evaluate_function(pieces[i]->est_a, pieces[i]->est_b, pieces[i]->start)); */
	/* printf("function at interval end (%lf) %lf\n", pieces[i]->end, evaluate_function(pieces[i]->est_a, pieces[i]->est_b, pieces[i]->end)); */
	func_eval[j] = evaluate_function(pieces[i]->est_a, pieces[i]->est_b, pieces[i]->start);
	func_eval[j+1] = evaluate_function(pieces[i]->est_a, pieces[i]->est_b, pieces[i]->end);
    }

    return func_eval;
}

/*
 * Finds the midpoints of the function values for each estimate at the breakpoints. The function
 * result at the end of the interval before the breakpoint and the function result at the
 * start of the interval after the breakpoint are checked, and the point directly between them is found.
 * The first and last breakpoints are the start and end of the interval, and as such do not have
 * another point to compare against, and so are returned as is.
 */
double* get_breakpoint_midpoints(double* breakpoint_vector, double* func_vals, int len)
{
    double* midpoints = malloc((len + 1) * sizeof(double));

    midpoints[0] = func_vals[0];
    int i, j;
    
    for (i = 1, j = 1; i < len; i++, j += 2){
	/* printf("i %d, j %d\n", i, j); */
	/* printf("breakpoint %lf\n", breakpoint_vector[i]); */
	/* printf("midpoint of [%lf, %lf] is midpoint: %lf\n", func_vals[j], func_vals[j+1], get_midpoint(func_vals[j], func_vals[j+1])); */
	midpoints[i] = get_midpoint(func_vals[j], func_vals[j+1]);
    }

    midpoints[i] = func_vals[j];

    return midpoints;
}

/*
 * Recalculates the estimated expression used to represent each interval so that
 * its start point and end points are at points corresponding to midpoints at a
 * breakpoint. The start of the first line and end of the last are left untouched.
 */
est_arr* recalculate_estimates(double* breakpoint_vector, double* function_values, int len)
{

    est_data** new_est = malloc(len * sizeof(est_data*));

    int i;
    
    for (i = 0; i < len; ++i) {
	/* printf("breakpoint %lf, function %lf\n", breakpoint_vector[i], function_values[i]); */
	/* printf("start point %lf, end point %lf\n", breakpoint_vector[i], breakpoint_vector[i+1]); */
	/* printf("start func %lf, end func %lf\n", function_values[i], function_values[i+1]); */

#ifdef VERBOSE
	printf("function %d starts at (%.2lf, %.2lf) and ends at (%.2lf, %.2lf)\n", i, breakpoint_vector[i], function_values[i],  breakpoint_vector[i+1], function_values[i+1]);
#endif
	double* tmp = get_intercept_and_gradient(breakpoint_vector[i], function_values[i],  breakpoint_vector[i+1], function_values[i+1]);
	est_data* this_interval = malloc(sizeof(est_data));
//	printf("tmp0 is %lf, tmp1 is %lf, start %lf, end %lf\n", tmp[0], tmp[1], breakpoint_vector[i], breakpoint_vector[i+1]);
	this_interval->est_a = tmp[0];
	this_interval->est_b = tmp[1];
	this_interval->start = breakpoint_vector[i];
	this_interval->end = breakpoint_vector[i+1];

	new_est[i] = this_interval;
	free(tmp);
#ifdef VERBOSE
	printf("new a %lf, new b %lf, start %lf, end %lf\n", new_est[i]->est_a, new_est[i]->est_b, new_est[i]->start, new_est[i]->end);
#endif
    }

    est_arr *newarr = malloc(sizeof(est_arr));
    newarr->len = i;
    newarr->estimates = new_est;
        
    return newarr;
}
