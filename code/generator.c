#include "generator.h"
#include "paramlist.h"
#include "file_util.h"
#include "math_util.h"

#define DEFAULT_ARR_SIZE 50
#define DEFAULT_WINDOW_SIZE 1.0

/*
 * Initialises the generator with some values specified by the arguments passed to the program,
 * and those that are contained in the parameter file, if there is one (there should be).
 * The order in which the arguments are is defined inside start.c
 */
void generate(char **args)
{
    int i, nruns = 1;
    char *outfile = NULL;
    char *paramfile = NULL;
    paramlist *params = NULL;
    char *tmp;
    
    double interval_time = 10.0;
    double lambda = 100.0;
        
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

    // If there is a value of nruns in the param file, use that instead of the default
    if ((tmp = get_param_val(params, "nruns")) != NULL)
	nruns = atoi(tmp);

    if ((tmp = get_param_val(params, "lambda")) != NULL)
	lambda = atol(tmp);

    if ((tmp = get_param_val(params, "interval_time")) != NULL)
    	interval_time = atol(tmp);
                
    printf("Number of runs: %d\nLambda: %lf\nInterval time: %lf\n", nruns, lambda, interval_time);

    muParserHandle_t hparser = mupCreate(0);

    /* char *eqn = "a-(b*sin(alpha*t))"; //check syntax is correct. Nothing to check if eqn is wrong. */
    /* double a = 10.0, b = 5.0, alpha = 0.05; */
    /* mupSetExpr(hparser, eqn); */
    /* mupDefineVar(hparser, "a", &a); */
    /* mupDefineVar(hparser, "b", &b); */
    /* mupDefineVar(hparser, "alpha", &alpha); */
    
    
    
    char *eqn = "a+b*t";
    double a = 5, b = 1;
    mupSetExpr(hparser, eqn);
    
    mupDefineVar(hparser, "a", &a);
    mupDefineVar(hparser, "b", &b);

    /* mupDefineVar(hparser, "t", &time_to_run); */
    	
    mupSetExpr(hparser, eqn);
        
    /* double testl = mupEval(hparser); */
    
    /* printf("test lambda = %lf\n", testl); */
    
    double time_delta[2] = {0.0, 15.0};
    run_time_nstreams(hparser, lambda, interval_time, time_delta, 1, outfile, 3);
    
    mupRelease(hparser);
    
    free_list(params);
}

/* 
 * Generates a series of streams, with a time delay between them. 
 * The time_delta array should contain the difference in time between 
 * each consecutive stream. The values affect the result of evaluating
 * the lambda function at each timestep. e.g. for a time_delta [10.0, 25.0]
 * the value passed to the first stream's lambda function will be t + 10.0,
 * and the second will be t + 25.0. The outswitch parameter determines whether
 * all data will be output to the file, or just the event times.
 * *IMPORTANT* The value of lambda MUST exceed the maximum value of the function! *IMPORTANT*
 */
void run_time_nstreams(muParserHandle_t hparser, double lambda, double runtime, double *time_delta, int nstreams, char *outfile, int outswitch)
{
    int i;
            
    for (i = 0; i < nstreams; ++i){
	printf("Generating event stream %d\n", i);
	run_time_nonhom(hparser, lambda, time_delta[i - 1], runtime, outfile, outswitch);
    }

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
 * The outswitch parameter determines what data will be output to the file.
 * 
 */
void run_time_nonhom(muParserHandle_t hparser, double lambda, double start_time, double runtime, char *outfile, int outswitch)
{
    double *et = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    double *lv = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    
    double **eptr = &et;
    double **lptr = &lv;
    
    int size = run_to_time_non_homogenous(hparser, lambda, start_time, runtime, eptr, lptr, DEFAULT_ARR_SIZE);

    if (outswitch == 0) // Outputs only event data - this is what the real data will be like.
	double_to_file(outfile, "w", *eptr, size);
    if (outswitch == 1) // Outputs only events and lambda values
	mult_double_to_file(outfile, "w", *eptr, *lptr, size);
    if (outswitch == 2){ // Outputs all data, including bin counts
	int num_intervals = (start_time + runtime) / DEFAULT_WINDOW_SIZE;

	int *bin_counts = sum_events_in_interval(*eptr, size, start_time, runtime, num_intervals);
    
	/* int i; */
	/* for (i = 0; i < num_intervals; ++i){ */
	/*     printf("Interval %d: %d\n", i, bin_counts[i]); */
	/* } */
    
	double *midpoints = get_interval_midpoints(start_time, runtime, num_intervals);

	/* for (i = 0; i < num_intervals; ++i){ */
	/*     printf("%lf\n", midpoints[i]); */
	/* } */
    
	int_dbl_to_file(outfile, "w", midpoints, bin_counts, num_intervals);
	free(midpoints);
	free(bin_counts);
    }
    if (outswitch == 3){
	char *tmp = malloc(strlen(outfile) + 3);
	sprintf(tmp, "%s_ev", outfile);
	double_to_file(tmp, "w", *eptr, size);
	
	sprintf(tmp, "%s_ad", outfile);

	mult_double_to_file(tmp, "w", *eptr, *lptr, size);

	int num_intervals = (start_time + runtime) / DEFAULT_WINDOW_SIZE;

	int *bin_counts = sum_events_in_interval(*eptr, size, start_time, runtime, num_intervals);
    
	/* int i; */
	/* for (i = 0; i < num_intervals; ++i){ */
	/*     printf("Interval %d: %d\n", i, bin_counts[i]); */
	/* } */
    
	double *midpoints = get_interval_midpoints(start_time, runtime, num_intervals);

	/* for (i = 0; i < num_intervals; ++i){ */
	/*     printf("%lf\n", midpoints[i]); */
	/* } */
    
	int_dbl_to_file(tmp, "a", midpoints, bin_counts, num_intervals);
	free(midpoints);
	free(bin_counts);
	free(tmp);
    }

    free(*eptr);
    free(*lptr);
}



/* 
 * Helper method to run a nonhomogenous process until a specific 
 * number of events have occurred. Allocates required memory
 * and prints output data to a file. The outswitch parameter determines whether
 * all data will be output to the file, or just the event times.
 */
void run_events_nonhom(muParserHandle_t hparser, double lambda, double start_time, int events, char *outfile, int outswitch)
{
    double *et = malloc(events * sizeof(double));
    double *lv = malloc(events * sizeof(double));

    run_to_event_limit_non_homogenous(hparser, lambda, start_time, events, et, lv);
    mult_double_to_file(outfile, "a", et, lv, events);
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
    init_rand(0.0);
        
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
    init_rand(0.0);
            
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
    init_rand(0.0);
    
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

