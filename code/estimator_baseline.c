#include "estimator.h"
#include "general_util.h"
#include "math_util.h"

double** baseline_estimate(char *event_file, char *output_file, 
			   double interval_start, double interval_end);
double* get_breakpoint_vector(double **pieces, int max_breakpoints);
double** recalculate_expressions(double* breakpoint_vector, double* function_values, int max_breakpoints);
double* get_func_vals_at_breakpoints(double **pieces, int max_breakpoints);
double* get_breakpoint_midpoints(double* breakpoint_vector, double* func_vals, int len);

int main(int argc, char *argv[])
{
    
    baseline_estimate(argv[1], argv[2], 0, 100);
    return 0;
}

double** baseline_estimate(char *event_file, char *output_file, 
			   double interval_start, double interval_end)
{
    double max_breakpoints = 5, IWLS_iterations = 3, 
	IWLS_subintervals = 10, max_extension = 15;
    
    double **pieces = piecewise_estimate(event_file, NULL, interval_start, interval_end, 
					 max_breakpoints, IWLS_iterations, IWLS_subintervals, 
					 max_extension);

    int len = *pieces[0] - 1;

    printf("there are %d lines in the estimate, so there are %d breakpoints\n", len, len - 1);
    
    pieces++;

    int i;
    
    double *breakpoint_vector = get_breakpoint_vector(pieces, len);
    double *func_eval = get_func_vals_at_breakpoints(pieces, len);
    double *midpoints = get_breakpoint_midpoints(breakpoint_vector, func_eval, len);
    
    printf("breakpoint values\n");
    for (i = 0; i < len + 1; ++i) {
    	printf("%lf\n", breakpoint_vector[i]);
    }

    printf("function values\n");
    for (i = 0; i < len * 2; ++i) {
    	printf("%lf\n", func_eval[i]);
    }

    printf("midpoint values\n");
    for (i = 0; i < len + 1; ++i) {
	printf("%lf\n", midpoints[i]);
    }

    
    free_pointer_arr((void**)pieces, len);
    free(breakpoint_vector);
    free(func_eval);
    free(midpoints);
        
    // for each piece
    // find the midpoint at each breakpoint
    // find the equation for line starting at middle of previous breakpoint and ending at the next breakpoint
    // output the data to file

    return NULL;
}

/*
 * Gets the x values (times) of the breakpoints given by the
 * piecewise estimator. Includes the start and end of the interval.
 */
double* get_breakpoint_vector(double **pieces, int num_breakpoints)
{
    double *breakpoints = malloc((num_breakpoints + 1) * sizeof(double));
    
    int i;
    
    for (i = 0; i < num_breakpoints && pieces[i] != NULL; ++i) {
	printf("pieces %d %lf\n", i, pieces[i][2]);
    	breakpoints[i] = pieces[i][2];
    }
    printf("pieces %d %lf\n", i, pieces[i - 1][3]);
    breakpoints[i] = pieces[i - 1][3]; // add the end of the interval
    return breakpoints;
}

/*
 * Calculates the values of all the estimated functions at the breakpoints.
 * The function values will be calculated at the start and end of each
 * subinterval, which is marked by the breakpoints.
 */
double* get_func_vals_at_breakpoints(double **pieces, int num_breakpoints)
{
    int i, j;
    double *func_eval = malloc(num_breakpoints * 2 * sizeof(double));
        
    for (i = 0, j = 0; i < num_breakpoints + 1; ++i, j += 2) {
	if (pieces[i] == NULL)
	    break;
	
	printf("%lf, %lf, %lf, %lf\n", pieces[i][0], pieces[i][1], pieces[i][2], pieces[i][3]);
	printf("function at interval start (%lf): %lf\n ", pieces[i][2], evaluate_function(pieces[i][0], pieces[i][1], pieces[i][2]));
	printf("function at interval end (%lf) %lf\n", pieces[i][3], evaluate_function(pieces[i][0], pieces[i][1], pieces[i][3]));
	func_eval[j] = evaluate_function(pieces[i][0], pieces[i][1], pieces[i][2]);
	func_eval[j+1] = evaluate_function(pieces[i][0], pieces[i][1], pieces[i][3]);
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
    double *midpoints = malloc((len + 1) * sizeof(double));

    midpoints[0] = func_vals[0];
    int i, j;
    
    for (i = 1, j = 1; i < len; i++, j += 2){
	printf("i %d, j %d\n", i, j);
	printf("breakpoint %lf\n", breakpoint_vector[i]);
	printf("midpoint of [%lf, %lf] is midpoint: %lf\n", func_vals[j], func_vals[j+1], get_midpoint(func_vals[j], func_vals[j+1]));
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
double** recalculate_expressions(double* breakpoint_vector, double* function_values, int num_breakpoints)
{
    /* double **adjusted_estimates = malloc(num_breakpoints); */

    /* get_intercept_and_gradient(); */
    
    
    /* return NULL; */
    return NULL;
}
