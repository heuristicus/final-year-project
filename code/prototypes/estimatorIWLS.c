#include "estimator.h"

int main(int argc, char *argv[])
{
    double time = 10.0;
    int subintervals = 2;
        
    double **intervals = get_subintervals(time, subintervals);
    
    int i;
    
    for (i = 0; i < subintervals; ++i){
	printf("Interval %d is [%lf, %lf, %lf]\n", i, intervals[i][0], intervals[i][1], intervals[i][2]);
    }

    free_pointer_arr((void **) intervals, subintervals);
    
    return 0;
}

/*
 * Gets the start, midpoint and end of each time interval, defined
 * by the total time and number of subintervals.
 */
double** get_subintervals(double time, int subintervals)
{
    int i;
    // Storage for start and end of subinterval.
    double **subinterval = malloc(subintervals * sizeof(double*));
    for (i = 0; i < subintervals; ++i){
	subinterval[i] = malloc(3 * sizeof(double));
	subinterval[i][0] = (i * time)/subintervals;
	subinterval[i][1] = get_interval_midpoint(i + 1, time, subintervals);
	subinterval[i][2] = ((i + 1) * time) / subintervals;
    }

    return subinterval;
}

double get_interval_midpoint(int interval_number, double time, int subintervals)
{
    return (interval_number - 1.0/2.0) * (time / subintervals);
}
