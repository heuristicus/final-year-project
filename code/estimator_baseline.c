#include "estimator.h"
#include "general_util.h"
#include "math_util.h"

double** baseline_estimate(char *event_file, char *output_file, 
			   double interval_start, double interval_end);
double* get_breakpoint_vector(double **pieces, int max_breakpoints);
double** recalculate_expressions(double* breakpoint_vector, double* function_values, int max_breakpoints);
double* get_func_vals_at_breakpoints(double **pieces, int max_breakpoints);

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

    printf("there are %d lines in the estimate\n", len);
    
    pieces++;

    int i, j;

    double *breakpoint_vector = get_breakpoint_vector(pieces, len);
    double *func_eval = get_func_vals_at_breakpoints(pieces, len);
    
    printf("breakpoint values\n");
    for (i = 0; i < len; ++i) {
    	printf("%lf\n", breakpoint_vector[i]);
    }

    printf("function values\n");
    for (i = 0; i < len * 2; ++i) {
    	printf("%lf\n", func_eval[i]);
    }
    
    printf("midpoints at breakpoint\n");
    for (i = 0, j = 1; i < len + 1; ++i, j += 2) {
	printf("breakpoint: %lf, y midpoint: %lf\n", breakpoint_vector[i], get_midpoint(func_eval[j], func_eval[j+1]));
    }

    free_pointer_arr((void**)pieces, len);
    free(breakpoint_vector);
    free(func_eval);
    
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
