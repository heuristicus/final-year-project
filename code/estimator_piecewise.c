#include "estimator.h"
#include "file_util.h"
#include "math_util.h"
#include <string.h>

//#define ERROR_THRESHOLD 10000
#define DEFAULT_INTERVALS 4
    
int check_next_time(double *estimate, double start_time, double limit);
void estimate_to_file(char *filename, double *estimate);

char *eventfile;
double **interval_estimates;

int main(int argc, char *argv[])
{

    eventfile = argv[1];
    char *outfile = argv[2];
    char *out_separate = malloc(strlen(outfile) + 5);
    interval_estimates = malloc((DEFAULT_INTERVALS + 1) * sizeof(double*));
    interval_estimates[0] = malloc(sizeof(double)); // we will store the size of the array in the first memory location
    interval_estimates += 1; 
    double interval_estimates_size = DEFAULT_INTERVALS;
    
    int i, j;
    double step = 33;
    
    double total_time = 100.0;
    int subintervals = total_time/step;
    double **estimates = malloc(subintervals * sizeof(double));

    double check_forwards = 15; // time to look forwards and see if we can use the same parameters
                
    int accept_estimate;

    double interval_start = 0;
    double interval_end = 0;
    double prev_interval_end = 0;
                
    for (i = 0; i < subintervals; ++i) {
	interval_start = prev_interval_end;
	interval_end = interval_start + step;
	// Stop the end interval exceeding the total time of the data.
	interval_end = interval_end > total_time ? total_time : interval_end;
	if (i - subintervals - 1 == 0) // If this is the last interval, end it at the total time.
	    interval_end = total_time;
			
	sprintf(out_separate, "%s_%d", outfile, i);
	printf("--------RUN %d STARTING:--------\n", i);
	printf("interval start: %lf, interval end: %lf\n", interval_start, interval_end);
	estimates[i] = estimate_IWLS(argv[1], out_separate, interval_start, interval_end, 10, 3);
	if (i > interval_estimates_size) {
	    // Play with the pointer so that the reallocation works.
	    interval_estimates -= 1;
	    interval_estimates = realloc(interval_estimates, (interval_estimates_size *= 2) * sizeof(double*));
	    interval_estimates += 1;
	}
	
	interval_estimates[i] = malloc(4 * sizeof(double));
	interval_estimates[i][0] = estimates[i][0];
	interval_estimates[i][1] = estimates[i][1];
	interval_estimates[i][2] = interval_start;

	if (i < subintervals - 1) { // don't try and estimate past the end of the data that we have
	    printf("subintervals %d, entering next check\n", subintervals);
	    for (j = 1; j < 4; ++j) {
		// Check if we can use the estimate we got above to estimate the next lot of data.
		// If we can't estimate a long interval, try with a few shorter intervals.
		accept_estimate = check_next_time(estimates[i], interval_end, check_forwards/j);
	    }
	}

	if (accept_estimate) {
	    interval_estimates[i][3] = interval_end + check_forwards/j;
	} else { // can't estimate next block using the current estimates
	    interval_estimates[i][3] = interval_end;
	}
	prev_interval_end = interval_estimates[i][3];
	
//	printf("updating interval estimate count\n");
	**(interval_estimates - 1) = i + 1; // store the size of the array before the data
    }

    
    for (i = 0; i < **(interval_estimates - 1); ++i) {
	printf("Interval start: %lf, interval end: %lf, a estimate: %lf, b estimate: %lf\n", interval_estimates[i][2], interval_estimates[i][3], interval_estimates[i][0], interval_estimates[i][1]);
    }

    for (i = 0; i < **(interval_estimates -1); ++i){
	char *f = malloc(strlen(argv[2]) + 4);
	sprintf(f, "%s_%d", argv[2], i);
	
	estimate_to_file(f, interval_estimates[i]);
	
	free(f);
    }

    free(out_separate);
    free_pointer_arr((void**)estimates, subintervals);
    interval_estimates -= 1;
    free_pointer_arr((void**)interval_estimates, **interval_estimates);
    
    return 0;
}

/*
 * Check the next time period to see if we can use the estimates from 
 * the previous time period to approximate their values in the next one 
 * as well. The return value is either 1 or 0, representing whether the
 * time interval [start_time, start_time + limit] can be reasonably
 * approximated by the estimate that we have passed in.
 */
int check_next_time(double *estimate, double start_time, double limit)
{
    /* double *evdata = get_event_data_interval(start_time, start_time + limit, eventfile); */

    /* int len = evdata[0] - 1; */
    /* evdata = evdata + 1; */
    
    /* int *bindata = sum_events_in_interval(evdata, len + 1, start_time, start_time + limit, 10); */
    
    char *out = malloc(10);
    
    sprintf(out, "s_%d.out", (int)start_time);
            
    /* int i; */

    /* for (i = 0; i < 10; ++i){ */
    /* 	printf("bin %d: %d\n", i, bindata[i]); */
    /* }*/

    double *res = estimate_OLS(eventfile, out, start_time, start_time + limit, 10);


    
    int i;
    
    for (i = 0; i < 4; ++i) {
	printf("OLS result: %lf, our result: %lf, difference: %lf\n", res[i], estimate[i], res[i] - estimate[i]);
    }

    free(out);
    
    // Specify arbitrary thresholds for acceptance or rejection
    if ((abs(res[0] - estimate[0]) > 5 ) || (abs(res[1] - estimate[1]) > 0.5)){
	free(res);
	return 0;
    }
        
    free(res);
    
    return 1;
    
}

void estimate_to_file(char *filename, double *estimate)
{
    double a = estimate[0];
    double b = estimate[1];
    double start = estimate[2];
    double end = estimate[3];
    
    // We will subtract from this value to make sure we cover the whole
    // length of the interval.
    double counter = start;
    
    printf("int end %lf, int start %lf\n", end, start);
    
    FILE *fp = fopen(filename, "w");
        
    while(counter < end){
	fprintf(fp, "%lf %lf\n", counter, a + counter * b);
//	printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
	if (end - counter <= 1){
	    counter += end - counter;
//	    printf("last bit %lf\n", counter);
//	    printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
	    fprintf(fp, "%lf %lf\n", counter, a + counter * b);
	} else {
	    counter += 1;
//	    printf("standard.\n");
	}
//	printf("counter: %lf, end %lf\n", counter, end);
    }

    fprintf(fp, "\n\n");
    
    fclose(fp);
    
}
