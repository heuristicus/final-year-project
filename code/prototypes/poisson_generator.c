#include "poisson_random.h"

#define MAX_EQN_LEN 30 // equation length max 30 chars

int main(int argc, char *argv[])
{

    int max = 100;
    
    double* event_times = malloc(max * sizeof(double));
    double* lambda_vals = malloc(max * sizeof(double));

    muParserHandle_t hparser = mupCreate(0);

    char* eqn = "a-sin(alpha*t)";
    
    mupSetExpr(hparser, eqn);
    double a = 2, b = 5.0, alpha = 0.1;
    
    mupDefineVar(hparser, "a", &a);
    mupDefineVar(hparser, "b", &b);
    mupDefineVar(hparser, "alpha", &alpha);

    //generate_event_times_non_homogenous(hparser, 10, 10, max, event_times, lambda_vals);
    run_to_time_non_homogenous(hparser, 10.0, 100.0, event_times, lambda_vals, max);
    //run_to_event_limit_non_homogenous(hparser, 10.0, max, event_times, lambda_vals);
       
    return 0;
}


/* seeds the random variable to be used. Should only be called once per run (?). */
void init_rand(void)
{
    srand48(time(NULL));
}

/* knuth method. Generates time to next even in a homogenous poisson process. */
double homogenous_time(double lambda)
{
    return -log(drand48()) / lambda;
}

/* generates an event time for a non-homogenous poisson process. This will require solving lambda(t) and passing its result as a parameter. Not sure how to do this or if there is a better way. */
double non_homogenous_time(double time)
{
    return log(time);
}

/* generates time for events in a homogenous poisson process until time is exceeded. Puts event times into the array passed in the parameters. Puts a -1 in the array location after the last event*/
void generate_event_times_homogenous(double lambda, double time, int max_events, double* event_times)
{
    init_rand();
        
    double run_time = 0;
    int i = 0;
    
    while ((run_time += homogenous_time(lambda)) < time && i < max_events){
	event_times[i] = run_time;
	++i;
    }

    if (i < max_events)
	event_times[i] = -1.0;
    
}

void run_to_time_non_homogenous(muParserHandle_t hparser, double lambda, double max_time, double* event_times, double* lambda_vals, int arr_len)
{
    init_rand();
    
    double run_time = 0.0, rand, non_hom_lambda, arr_max = arr_len;
    int i = 0;
    
    mupDefineVar(hparser, "t", &run_time);
    
    while ((run_time += homogenous_time(lambda)) < max_time){
	non_hom_lambda = mupEval(hparser);
	printf("%lf %lf\n", run_time, non_hom_lambda);//more granularity on lambda values
	if ((rand = drand48()) <= non_hom_lambda / lambda){
	    // Number of events may exceed the number of array locations initally assigned
	    // so may need to reallocate memory to store more.
	    if (i >= arr_max){
		//printf("array length %d exceeds initial max %d\n", i, arr_len);
		void *_tmp = realloc(event_times, i * 2 * sizeof(double)); // twice the original size.
		void *_tmp2 = realloc(lambda_vals, i * 2 * sizeof(double)); // twice the original size.
		if (!_tmp || !_tmp2){// exit if allocation failed.
		    printf("Memory reallocation for arrays failed. Exiting.\n");
		    exit(1);
		}
		arr_max = i * 2;
		event_times = (double*) _tmp;
		lambda_vals = (double*) _tmp2;
	    }
	    event_times[i] = run_time;
	    lambda_vals[i] = non_hom_lambda;
	    ++i;
	} 
    }
    printf("\n\n");//for gnuplot index separation
    int j;
    
    for (j = 0; j < i && event_times[j] > 0; ++j){
	printf("%lf %lf\n", event_times[j], lambda_vals[j]);
    }

    free(event_times);
    free(lambda_vals);
          
}

void run_to_event_limit_non_homogenous(muParserHandle_t hparser, double lambda, int max_events, double* event_times, double* lambda_vals)
{
    init_rand();
    
    double run_time = 0.0, rand, non_hom_lambda;
    int i = 0;
    
    mupDefineVar(hparser, "t", &run_time);
    
    while (i < max_events){
	run_time += homogenous_time(lambda);
	non_hom_lambda = mupEval(hparser);
	printf("%lf %lf\n", run_time, non_hom_lambda);//more granularity on lambda values
	if ((rand = drand48()) <= non_hom_lambda / lambda){
	    event_times[i] = run_time;
	    lambda_vals[i] = non_hom_lambda;
	    ++i;
	}
    }
    printf("\n\n");//for gnuplot separation of indices
    int j;
    
    for (j = 0; j < i && event_times[j] > 0; ++j){
	printf("%lf %lf\n", event_times[j], lambda_vals[j]);
    }
    
    free(event_times);
    free(lambda_vals);
}
