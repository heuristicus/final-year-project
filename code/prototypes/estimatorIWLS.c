#include "estimator.h"

int main(int argc, char *argv[])
{
    double time = 10.0;
    int subintervals = 20;
    
    
    double **intervals = get_subintervals(time, subintervals);
    
    int i;
    
    for (i = 0; i < subintervals; ++i){
	printf("Interval %d is [%lf, %lf]\n", i, intervals[i][0], intervals[i][1]);
    }

    free_pointer_arr((void **) intervals, subintervals);
    
    return 0;
}

double** get_subintervals(double time, int subintervals)
{
    int i;
    // Storage for start and end of subinterval.
    double **subinterval = malloc(subintervals * sizeof(double*));
    for (i = 0; i < subintervals; ++i){
	subinterval[i] = malloc(2 * sizeof(double));
	subinterval[i][0] = (i * time)/subintervals;
	subinterval[i][1] = ((i + 1) * time) / subintervals;
    }

    return subinterval;
}

void free_pointer_arr(void **arr, int length)
{
    int i;
    
    for (i = 0; i < length; ++i){
	free(arr[i]);
    }
    free(arr);
}
