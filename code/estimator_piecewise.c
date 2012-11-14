#include "estimator.h"
#include "file_util.h"
#include "math_util.h"
#include <string.h>

#define INTERVAL_EPSILON 0.5

int check_next_time(char *infile, double *estimate, double start_time, double limit);
void estimate_to_file(char *filename, double *estimate, char *mode);
void output_estimates(char *filename, double **estimates, int len);
void piecewise_estimate(char *infile, char *outfile, double start_time, double end_time, int break_points);

void piecewise_estimate(char *infile, char *outfile, double start_time, double end_time, int break_points)
{
    
    char *out_separate = malloc(strlen(outfile) + 5); // separate outfiles for each piece
    double **interval_estimates = malloc((break_points + 1) * sizeof(double*)); // estimates for each piece
    interval_estimates[0] = calloc(1, sizeof(double)); // we will store the size of the array in the first memory location
    interval_estimates += 1;
    
    int i, j;
    double interval_time = end_time - start_time;
    double step = interval_time/break_points;
    
    double **estimates = malloc(break_points * sizeof(double*));

    double check_forwards = 15; // time to look forwards and see if we can use the same parameters
                
    double interval_start = start_time;
    double interval_end = 0;
    double prev_interval_end = 0;

    for (i = 0; i < break_points && prev_interval_end <= end_time; ++i) {
	int accept_estimate = 0;
	interval_start = start_time + prev_interval_end;
	interval_end = interval_start + step;

	
	// Stop the end interval exceeding the total time of the data.
	interval_end = interval_end > end_time ? end_time : interval_end;
	if (i - break_points - 1 == 0) // If this is the last interval, end it at the total time.
	    interval_end = interval_time;
			
	if (abs(interval_start - interval_end) < INTERVAL_EPSILON){
	    printf("Interval start and end are equal\n");
	    break;
	}

	sprintf(out_separate, "%s_%d", outfile, i);
	printf("--------RUN %d STARTING:--------\n", i);
	printf("interval start: %lf, interval end: %lf\n", interval_start, interval_end);
	estimates[i] = estimate_IWLS(infile, out_separate, interval_start, interval_end, 10, 3);
	/* if (i > interval_estimates_size) { */
	/*     // Play with the pointer so that the reallocation works. */
	/*     interval_estimates -= 1; */
	/*     interval_estimates = realloc(interval_estimates, (interval_estimates_size *= 2) * sizeof(double*)); */
	/*     interval_estimates += 1; */
	/* } */
	
	// Save data for this run
	interval_estimates[i] = malloc(4 * sizeof(double));
	interval_estimates[i][0] = estimates[i][0];
	interval_estimates[i][1] = estimates[i][1];
	interval_estimates[i][2] = interval_start;

	if (interval_start <= end_time && interval_end < end_time) { // don't try and estimate past the end of the data that we have
	    printf("Checking if the estimates can be extended for the next time period.\n\n");
	    for (j = 1; j < 4; ++j) {
		printf("Checking estimates against estimate for time period [%lf, %lf]\n", interval_end, interval_end + check_forwards/j);
		// Check if we can use the estimate we got above to estimate the next lot of data.
		// If we can't estimate a long interval, try with a few shorter intervals.
		accept_estimate = check_next_time(infile, estimates[i], interval_end, check_forwards/j);
		if (accept_estimate == 1){
		    printf("Estimates for next time period are similar. Extending the line.\n");
		    break;
		} else {
		    printf("Estimates are not similar enough.\n\n");
		}
	    }
	}

	if (accept_estimate) {
	    interval_estimates[i][3] = interval_end + check_forwards/j;
	} else { // can't estimate next block using the current estimates
	    interval_estimates[i][3] = interval_end;
	}
	printf("Estimates for interval [%lf, %lf]: a = %lf, b = %lf\n\n\n", interval_start, interval_end, interval_estimates[i][0], interval_estimates[i][1]);
	prev_interval_end = interval_estimates[i][3];
	
//	printf("updating interval estimate count\n");
	**(interval_estimates - 1) += 1; // store the size of the array before the data
    }

    
    for (i = 0; i < **(interval_estimates - 1); ++i) {
	printf("Interval start: %lf, interval end: %lf, a estimate: %lf, b estimate: %lf\n", interval_estimates[i][2], interval_estimates[i][3], interval_estimates[i][0], interval_estimates[i][1]);
    }

    output_estimates(outfile, interval_estimates, **(interval_estimates - 1));
    
    free(out_separate);
    interval_estimates -= 1;
    // We may end up with fewer estimates than break points due to extension
    free_pointer_arr((void**)estimates, (**interval_estimates));
    free_pointer_arr((void**)interval_estimates, (**interval_estimates) + 1); // free everything 
    
}

int main(int argc, char *argv[])
{

    piecewise_estimate(argv[1], argv[2], 0, 100, 5);
        
    return 0;
}



/*
 * Check the next time period to see if we can use the estimates from 
 * the previous time period to approximate their values in the next one 
 * as well. The return value is either 1 or 0, representing whether the
 * time interval [start_time, start_time + limit] can be reasonably
 * approximated by the estimate that we have passed in.
 */
int check_next_time(char *infile, double *estimate, double start_time, double limit)
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

    double *res = estimate_OLS(infile, out, start_time, start_time + limit, 10);


    
    int i;
    
    for (i = 0; i < 4; ++i) {
	printf("OLS result: %lf, our result: %lf, difference: %lf\n", res[i], estimate[i], res[i] - estimate[i]);
    }

    int ret = 1;
    // Specify arbitrary thresholds for acceptance or rejection
    if ((abs(res[0] - estimate[0]) > 10 ) || (abs(res[1] - estimate[1]) > 0.5) || (res[1] < 0 && estimate[1] > 0) || (res[1] > 0 && estimate[1] < 0)){
	printf("threshold broken\n");
	ret = 0;
    }

    printf("retval = %d\n", ret);
    
    free(res);
    free(out);
    
    return ret;
    
}

/*
 * Outputs a series of estimates to separate files. The filename provided
 * will have the interval number appended to it - the first will be
 * "filename_0"
 */
void output_estimates(char *filename, double **estimates, int len)
{
    int i;
    //char *f;
        
    for (i = 0; i < len; ++i) {
	//f = malloc(strlen(filename) + 4);
	//sprintf(f, "%s_%d", filename, i);
	estimate_to_file(filename, estimates[i], i > 0 ? "a" : "w");
    }

}

/*
 * Prints a single set of estimates to an output file. This will calculate
 * the value of the function for each second within the interval.
 */
void estimate_to_file(char *filename, double *estimate, char *mode)
{
    double a = estimate[0];
    double b = estimate[1];
    double start = estimate[2];
    double end = estimate[3];
    
    // We will subtract from this value to make sure we cover the whole
    // length of the interval.
    double counter = start;
    
    printf("int end %lf, int start %lf\n", end, start);
    
    FILE *fp = fopen(filename, mode);
        
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
