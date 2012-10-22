#include "poisson.h"

#define DEFAULT_ARR_SIZE 50
#define DEFAULT_WINDOW_SIZE 1.0

/*
 * Initialises the generator with some values specified by the arguments passed to the program,
 * and those that are contained in the parameter file, if there is one (there should be).
 * The order in which the arguments are is defined inside start.c
 */
void initialise_generator(char **args)
{
    int i, nruns = 1;
    char *outfile = NULL;
    char *paramfile = NULL;
    paramlist *params = NULL;
    char *tmp;
        
    for (i = 0; i <= sizeof(args)/sizeof(char*); ++i){
	if (args[i] == NULL)
	    continue;
	
	if (i == 0)
	    outfile = args[i];
	if (i == 1)
	    paramfile = args[i];
	if (i == 2)
	    nruns = atoi(args[i]);
    }
    
    if (paramfile != NULL){
	params = get_parameters(paramfile);
    }

    if ((outfile = select_output_file(outfile, get_param_val(params, "outfile"))) == NULL){
	free(params);
	return;
    }
    

    if ((tmp = get_param_val(params, "nruns")) != NULL)
	nruns = atoi(tmp);
        
    printf("number of runs: %d\n", nruns);

    muParserHandle_t hparser = mupCreate(0);

    char *eqn = "a-(b*sin(alpha*t))"; // check syntax is correct. Nothing to check if eqn is wrong.
    
    mupSetExpr(hparser, eqn);
    
    double a = 10.0, b = 5.0, alpha = 0.05;
    
    mupDefineVar(hparser, "a", &a);
    mupDefineVar(hparser, "b", &b);
    mupDefineVar(hparser, "alpha", &alpha);
    
    //run_time_nonhom(hparser, 100.0, 0.0, 100.0, outfile);
    double time_delta[2] = {0.0, 15.0};
    run_time_nstreams(hparser, 100.0, 100.0, time_delta, 2, outfile);
    
    mupRelease(hparser);
    
    free_list(params);
}

/*
 * Selects an output file. Will prompt user if there is a file specified in both the command line
 * and in the parameter file.
 */
char* select_output_file(char* cur_out, char* param_out)
{
    char* outfile;
    
    if (param_out == NULL && cur_out == NULL){
	printf("No parameter for the output file found in the parameters file or command line arguments. Auto-generating...\n");
	outfile = generate_outfile();
    } else if (param_out != NULL && cur_out != NULL){
	printf("Output file found in both parameter file and command line arguments.\n");
	int u = -1;
	while (u < 0 || u > 1){
	    printf("To use the file %s, enter 1. To use the file %s, enter 0.\n", param_out, cur_out);
	    scanf("%d", &u);
	}

	if (u == 1){
	    outfile = param_out;
	} else {
	    outfile = cur_out;
	}
    } else if (cur_out == NULL){
	outfile = param_out;
    } else {
	outfile = cur_out;
    }
    
    printf("Will output to %s\n", outfile);

    return outfile;
        
}

/* Helper method to run a nonhomogenous process for a specific length
 * of time. Allocates required memory and prints output data to a file.
 */
void run_time_nonhom(muParserHandle_t hparser, double lambda, double start_time, double runtime, char *outfile)
{
    double *et = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    double *lv = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    
    double **eptr = &et;
    double **lptr = &lv;
    
    int size = run_to_time_non_homogenous(hparser, lambda, start_time, runtime, eptr, lptr, DEFAULT_ARR_SIZE);

    int i;
    printf("exited\n");
    double_to_file(outfile, "a", *eptr, *lptr, size);
    printf("output to file\n");
    int nsize = size / DEFAULT_WINDOW_SIZE;
            
    int *rolling = calloc(nsize, sizeof(int));
    int *time_steps = calloc(nsize, sizeof(int));
    int roll_size = rolling_window(*eptr, size, start_time, DEFAULT_WINDOW_SIZE, rolling);
    printf("calculated roll\n");
    for (i = 0; i < roll_size; ++i){
    	time_steps[i] = DEFAULT_WINDOW_SIZE * i;
    }
    
    int_to_file(outfile, "a", time_steps, rolling, roll_size);
    
    
    free(time_steps);
    free(rolling);
    free(*eptr);
    free(*lptr);

}

/* 
 * Generates a series of streams, with a time delay between them. 
 * The time_delta array should contain the difference in time between 
 * each consecutive stream. The values affect the result of evaluating
 * the lambda function at each timestep. e.g. for a time_delta [10.0, 25.0]
 * the value passed to the first stream's lambda function will be t + 10.0,
 * and the second will be t + 25.0.
 */
void run_time_nstreams(muParserHandle_t hparser, double lambda, double runtime, double *time_delta, int nstreams, char *outfile)
{
    int i;
            
    for (i = 1; i < nstreams; ++i){
	printf("%d\n", i);
	run_time_nonhom(hparser, lambda, time_delta[i], runtime, outfile);
    }

}

/* 
 * Helper method to run a nonhomogenous process until a specific 
 * number of events have occurred. Allocates required memory
 * and prints output data to a file.
 */
void run_events_nonhom(muParserHandle_t hparser, double lambda, double start_time, int events, char *outfile)
{
    double *et = malloc(events * sizeof(double));
    double *lv = malloc(events * sizeof(double));

    run_to_event_limit_non_homogenous(hparser, lambda, start_time, events, et, lv);
    double_to_file(outfile, "a", et, lv, events);
}

/* 
 * seeds the random variable to be used. Should only be called once
 * per run. 
 */
void init_rand(void)
{
    srand48(time(NULL));
}

/* knuth method. Generates time to next event in a homogenous poisson process. */
double homogenous_time(double lambda)
{
    return -log(drand48()) / lambda;
}

/* 
 * Generates time for events in a homogenous poisson process until 
 * time is exceeded. 
 * Puts event times into the array passed in the parameters. 
 * Puts a -1 in the array location after the last event
 */
void generate_event_times_homogenous(double lambda, double time, int max_events, double *event_times)
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

/* 
 * Runs a non-homogenous poisson process until the specified time has
 * elapsed. 
 * The event_times and lambda_vals array will be populated with the 
 * time of an event and the result of evaluating lambda(t) at that 
 * time. Will attempt to reallocate memory for arrays if the number 
 * of events exceeds the size of arrays passed. 
 * Returns the final array location in which there is something 
 * stored.
 */
int run_to_time_non_homogenous(muParserHandle_t hparser, double lambda, double t_delta, double time_to_run, double **event_times, double **lambda_vals, int arr_len)
{
    init_rand();
    
    double run_time = 0,  end_time = time_to_run, arr_max = arr_len, func_in = run_time + t_delta;
    double rand, non_hom_lambda, hom_out;
    int i = 0;
    
    mupDefineVar(hparser, "t", &func_in);

    while (run_time < end_time){
	hom_out = homogenous_time(lambda);
	run_time += hom_out;
	func_in += hom_out;
	non_hom_lambda = mupEval(hparser);
	//printf("%lf %lf\n", run_time, non_hom_lambda);//more granularity on lambda values // get this into output file
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

    return i;
    
}



/* 
 * Runs a non-homogenous poisson process until the number of events 
 * specified have occurred. The event_times and lambda_vals array 
 * will be populated with the time of an event and the result of 
 * evaluating lambda(t) at that time.
 */
void run_to_event_limit_non_homogenous(muParserHandle_t hparser, double lambda, double t_delta, int max_events, double *event_times, double *lambda_vals)
{
    init_rand();
    
    double run_time = 0, func_in = t_delta + run_time;
    double rand, non_hom_lambda, hom_out;
    int i = 0;
    
    mupDefineVar(hparser, "t", &func_in);
    
    while (i < max_events){
	hom_out = homogenous_time(lambda);
	run_time += hom_out;
	func_in += hom_out;
	non_hom_lambda = mupEval(hparser);
	//printf("%lf %lf\n", run_time, non_hom_lambda);//more granularity on lambda values // get into the output file
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

