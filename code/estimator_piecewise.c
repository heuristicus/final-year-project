#include "estimator.h"
#include <string.h>

//#define ERROR_THRESHOLD 10000

int main(int argc, char *argv[])
{

    char *infile = argv[1];
    char *outfile = argv[2];
    char *out_separate = malloc(strlen(outfile) + 5);
    
    
    int i;

    double **estimates = malloc(3 * sizeof(double));
    double total_time = 100.0;
    int subintervals = 3;
    double subinterval = total_time / subintervals;
            
    for (i = 0; i < 3; ++i) {
	sprintf(out_separate, "%s_%d", outfile, i);
	printf("--------RUN %d STARTING:--------\n", i);
	printf("interval start: %lf, subinterval end: %lf\n", subinterval * i, subinterval * (i + 1));
	estimates[i] = estimate_IWLS(argv[1], out_separate, subinterval * i, subinterval * (i + 1), 10, 3);
    }

//    double *p2_est = estimate_IWLS(argv[1], argv[2], 50.0, 100.0, 10, 3);
    
/*     for (i = 0; i < 4; ++i) { */
/* 	printf("First run: %lf\n", p1_est[i]); */
/* //	printf("Second run: %lf\n", p2_est[i]); */
/*     }  */
    
    /* printf("First chunk a estimate: %lf\n Second chunk a estimate: %lf\n", p1_est[0], p2_est[0]); */
    /* printf("First chunk b estimate: %lf\n Second chunk b estimate: %lf\n", p1_est[1], p2_est[1]); */
    /* printf("First chunk SSE a b: %lf\n Second chunk SSE a b: %lf\n", p1_est[2], p2_est[2]); */
    /* printf("First chunk SSE alpha beta: %lf\n Second chunk SSE alpha beta: %lf\n", p1_est[3], p2_est[3]); */
    
    free(out_separate);
    
    return 0;
}
