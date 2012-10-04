#include "poisson_random.h"

#define MAX_EQN_LEN 30 // equation length max 30 chars

int main(int argc, char *argv[])
{

    int max = 100;
    
    double *et = malloc(max * sizeof(double));
    double *lv = malloc(max * sizeof(double));
    
    double** eptr = &et;
    double** lptr = &lv;
    
    muParserHandle_t hparser = mupCreate(0);

    char* eqn = "a-sin(alpha*t)";
    
    mupSetExpr(hparser, eqn);
    double a = 2, b = 5.0, alpha = 0.1;
    
    mupDefineVar(hparser, "a", &a);
    mupDefineVar(hparser, "b", &b);
    mupDefineVar(hparser, "alpha", &alpha);

    //generate_event_times_non_homogenous(hparser, 10, 10, max, event_times, lambda_vals);
    //run_to_time_non_homogenous(hparser, 100.0, 100.0, event_times, lambda_vals, max);
    int size = run_to_time_non_homogenous(hparser, 100.0, 100.0, eptr, lptr, max);
    //run_to_event_limit_non_homogenous(hparser, 10.0, max, event_times, lambda_vals);
    int i;
    
    for (i = 0; i < size; ++i){
	//printf("%lf, %lf\n", eptr[0][i], lptr[0][i]);
    }

    double window_size = 2.0;
    
    int *rolling = malloc((size / window_size) * sizeof(int));
    
    int roll_size = rolling_window(*eptr, size, window_size, rolling);
    printf("rsize %d\n", roll_size);
    
    for (i = 0; i < roll_size; ++i){
	printf("i am %d\n", i);
	printf("%d\n", rolling[i]);
    }
    
    free(rolling);
    free(*eptr);
    free(*lptr);
    mupRelease(hparser);
    
    return 0;
}


/* seeds the random variable to be used. Should only be called once per run (?). */
void init_rand(void)
{
    srand48(time(NULL));
}

/* knuth method. Generates time to next event in a homogenous poisson process. */
double homogenous_time(double lambda)
{
    return -log(drand48()) / lambda;
}

/* generates time for events in a homogenous poisson process until time is exceeded. 
   Puts event times into the array passed in the parameters. 
   Puts a -1 in the array location after the last event*/
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

/* runs a non-homogenous poisson process until the specified time has elapsed. 
   The event_times and lambda_vals array will be populated with the time of an event 
   and the result of evaluating lambda(t) at that time. Will attempt to reallocate 
   memory for arrays if the number of events exceeds the size of arrays passed. 
   returns the final array location in which there is something stored.*/
int run_to_time_non_homogenous(muParserHandle_t hparser, double lambda, double max_time, double** event_times, double** lambda_vals, int arr_len)
{
    init_rand();
    
    double run_time = 0.0, rand, non_hom_lambda, arr_max = arr_len;
    int i = 0;
    
    mupDefineVar(hparser, "t", &run_time);
    
    while ((run_time += homogenous_time(lambda)) < max_time){
	non_hom_lambda = mupEval(hparser);
	//printf("%lf %lf\n", run_time, non_hom_lambda);//more granularity on lambda values
	if ((rand = drand48()) <= non_hom_lambda / lambda){
	    // Number of events may exceed the number of array locations initally assigned
	    // so may need to reallocate memory to store more.
	    if (i >= arr_max){
		//printf("array length %d exceeds initial max %d\n", i, arr_len);
		*event_times = realloc(*event_times, i * 2 * sizeof(double)); // twice the original size.
		*lambda_vals = realloc(*lambda_vals, i * 2 * sizeof(double)); // twice the original size.
		if (!lambda_vals || !event_times){// exit if allocation failed.
		    printf("Memory reallocation for arrays failed. Exiting.\n");
		    exit(1);
		}
		arr_max = i * 2;

	    }
	    //printf("putting in arrays\n");
	    event_times[0][i] = run_time;
	    lambda_vals[0][i] = non_hom_lambda;
	    ++i;
	} 
    }
    printf("\n\n");//for gnuplot index separation
    
    return i;
    
}



/* runs a non-homogenous poisson process until the number of events specified have occurred. 
   The event_times and lambda_vals array will be populated with the time of an event 
   and the result of evaluating lambda(t) at that time. */
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
    
    for (j = 0; j < i && event_times[j] > 0; ++j)
	printf("%lf %lf\n", event_times[j], lambda_vals[j]);
    
    /* this should be removed - need to have the arrays intact after operation */
    free(event_times);
    free(lambda_vals);
}
