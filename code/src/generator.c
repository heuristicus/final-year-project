#include "generator.h"
#include "paramlist.h"
#include "file_util.h"
#include "math_util.h"
#include "general_util.h"

#define DEFAULT_ARR_SIZE 50
#define DEFAULT_WINDOW_SIZE 1.0
#define PARSER_MAXVARS 10

static int generated_outfile = 0;

void on_error(muParserHandle_t hParser);
muFloat_t* var_factory(const muChar_t* a_szName, void *pUserData);
int check_expr_vars(muParserHandle_t hparser, struct paramlist *params);
double* parser_tptr(muParserHandle_t hparser);
void view_expr(muParserHandle_t hparser);

//#define VERBOSE

/*
 * Initialises the generator with some values specified by the arguments passed to the program,
 * and those that are contained in the parameter file, if there is one (there should be).
 * The order in which the arguments are is defined inside start.c
 */
void generate(char *paramfile, char *outfile, int nruns)
{
    int i;
    paramlist *params = NULL;
    char *tmp;

    double interval_time = 10.0;
    double lambda = 100.0;
    double *time_delta;
    int tdlen;
    int outswitch = 3;
    char *expr;

    if (paramfile != NULL){
	params = get_parameters(paramfile);
    } else {
	printf("You have not specified a parameter file. Please specify one in the command line call.\n");
	exit(1);
    }

    if ((outfile = select_output_file(outfile, get_string_param(params, "outfile"))) == NULL){
	free(params);
	return;
    }

    // Check param files for parameters and use those instead of defaults if they exist
    if (nruns == 1 && (tmp = get_string_param(params, "nruns")) != NULL)
	nruns = atoi(tmp);

    if ((tmp = get_string_param(params, "lambda")) != NULL)
	lambda = atol(tmp);

    if ((tmp = get_string_param(params, "interval_time")) != NULL)
    	interval_time = atol(tmp);

    if ((tmp = get_string_param(params, "timedelta")) != NULL){
	char **vals = string_split(tmp, ',');
	tdlen = atoi(vals[0]) - 1;
	time_delta = malloc((tdlen + 1) * sizeof(double));
	
	for (i = 1; i < tdlen + 1; ++i) {
	    time_delta[i - 1] = atof(vals[i]);
	}

	free_pointer_arr((void**) vals, atoi(vals[0]));
    } else { // setup default time delta for one loop
	time_delta = malloc(sizeof(double));
	time_delta[0] = 0.0;
	tdlen = 1;
    }

    if ((tmp = get_string_param(params, "verbosity")) != NULL)
	outswitch = atoi(tmp);

    if ((tmp = get_string_param(params, "expression")) != NULL){
	expr = tmp;
    } else {
	printf("You have not defined an expression to use.\nPlease do so in the parameter file (add \"expression a+b*x\" and define values for the variables; \"a 10\" etc.)\n");
	exit(1);
    }

#ifdef VERBOSE
    printf("Number of runs: %d\nLambda: %lf\nInterval time: %lf\nTime deltas: ", nruns, lambda, interval_time);

    for (i = 0; i < tdlen; ++i) {
	printf("%lf", time_delta[i]);
	if (i != tdlen - 1)
	    printf(", ");
	else
	    printf("\n");
    }

    printf("Log verbosity: %d\n", outswitch);
#endif

    muParserHandle_t hparser = mupCreate(0);
    mupSetVarFactory(hparser, var_factory, NULL);
    mupSetErrorHandler(hparser, on_error);
    
    mupSetExpr(hparser, expr);

    if (!check_expr_vars(hparser, params) || mupError(hparser)){
	printf("Expression is invalid. Exiting.\n");
	mupRelease(hparser);

	if (generated_outfile)
	    free(outfile);

	free_list(params);
	free(time_delta);
	exit(1);
    }
    
    run_time_nstreams(hparser, lambda, interval_time, time_delta, nruns, outfile, outswitch);
    
    mupRelease(hparser);

    if (generated_outfile)
	free(outfile);

    free_list(params);
    free(time_delta);
}

/*
 * Generates an event stream by using a random function provided by a set of 
 * gaussians. The gaussians are expected as input in raw form from the infile.
 * If no input is provided, a random function will be generated and events will
 * be generated from that.
 */
void generate_from_gaussian(char* paramfile, char* outfile, char* infile, int nstreams)
{
    gauss_vector* G;
    paramlist* params = get_parameters(paramfile);

    double stdev = get_double_param(params, "gauss_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double step = get_double_param(params, "gauss_generation_step");
    double resolution = get_double_param(params, "gauss_resolution");
    char* stream_ext = get_string_param(params, "stream_ext");
    
    double* time_delta;
    char* tmp;
    int tdlen;
    int i;
    
    if ((tmp = get_string_param(params, "timedelta")) != NULL){
	char** vals = string_split(tmp, ',');
	tdlen = atoi(vals[0]) - 1;
	time_delta = malloc(tdlen * sizeof(double));
	
	for (i = 0; i < tdlen; ++i) {
	    time_delta[i] = atof(vals[i + 1]);
	}

	free_pointer_arr((void**) vals, atoi(vals[0]));
    } else {
	printf("no timedelta provided.\n");
	exit(1);
    }

    if (outfile == NULL){
	outfile = get_string_param(params, "outfile");
    }
    
    if (infile == NULL){
	double multiplier = get_double_param(params, "gauss_func_multiplier");
	if (multiplier == 0){
	    printf("WARNING: Multiplier for function generation is zero! Your functions" \
		   " will probably be completely flat!\n");
	}
	G = gen_gaussian_vector_uniform(stdev, start, interval, step, multiplier);
	printf("No input file specified. Generating random function.\n");
    } else {
	G = read_gauss_vector(infile);
	printf("Reading from %s.\n", infile);
    }

    double_multi_arr* T = gauss_transform(G, start, start+interval, resolution);
    
    double max = find_max_value(T->data[1], T->lengths[1]);
    double min = find_min_value(T->data[1], T->lengths[1]);
    double lambda = ceil(-min + max);
//    output_gauss_transform("gen_transform", "w", T->data, -min, T->lengths[1], 1);
//    output_gaussian_contributions("gen_gauss" , "w", G, start, start + interval, resolution, 1);

    char* out = malloc(strlen(outfile) + strlen(stream_ext) + 5 + strlen(".dat"));

    for (i = 0; i < nstreams; ++i) {
	sprintf(out, "%s%s%d.dat", outfile, stream_ext, i);
    	double_multi_arr* stream = nonhom_from_gaussian(G, lambda, start, interval, time_delta[i], -min);
	output_double_multi_arr(out, "w", stream);
	free_double_multi_arr(stream);
    }

    free_gauss_vector(G);
    free_double_multi_arr(T);
    free(time_delta);
    free_list(params);
    free(out);
}

/*
 * Generates a stream of events using a nonhomogeneous poisson process
 * with the fuction specified by a linear combination of gaussians as the
 * generating function.
 */
double_multi_arr* nonhom_from_gaussian(gauss_vector* G, double lambda, 
				       double start, double interval, double time_delta, double shift)
{
    init_rand(0.0);
            
    double base_time = start;
    double shifted_time = time_delta + start;
    double end = start + interval;
    double rand, non_hom_lambda, hom_out;
    int i = 0, arr_max = DEFAULT_ARR_SIZE;
    double_multi_arr* ret = malloc(sizeof(double_multi_arr));
    ret->data = malloc(2 * sizeof(double*));
    ret->data[0] = malloc(arr_max * sizeof(double));
    ret->data[1] = malloc(arr_max * sizeof(double));
    ret->len = 2;
    ret->lengths = malloc(2 * sizeof(int));
    
    while (base_time < end){
	hom_out = homogeneous_time(lambda);
	base_time += hom_out;
	shifted_time += hom_out;
	non_hom_lambda = sum_gaussians_at_point(shifted_time, G) + shift;
	if ((rand = get_uniform_rand()) <= non_hom_lambda / lambda){
	    if (i >= arr_max){
		ret->data[0] = realloc(ret->data[0], i * 2 * sizeof(double));
		ret->data[1] = realloc(ret->data[1], i * 2 * sizeof(double));
		if (!ret->data[0] || !ret->data[1]){
		    printf("Memory reallocation for arrays failed during generation. Exiting.\n");
		    exit(1);
		}
		arr_max = i * 2;
	    }
	    ret->data[0][i] = base_time;
	    ret->data[1][i] = non_hom_lambda;
	    ++i;
	}
    }

    ret->lengths[0] = ret->lengths[1] = i;

    return ret;
}

/*
 * Generates gaussian data of the type specified. If an input file is provided, 
 * the assumed 1-dimensional data provides the means for unweighted gaussians 
 * with the given standard deviation. If no input file is specified, then gaussians
 * are generated uniformly over the interval specified in the parameter file.
 * A value of 1 outputs raw gaussians, which can be used to generate event streams. 
 * 0 outputs the discrete transform, which gives the value of the function at evenly
 * spaced points along the x-axis.
 */
void generate_gaussian_data(char* paramfile, char* infile, char* outfile,
			    int number, int output_type)
{
    paramlist* params = get_parameters(paramfile);
    
    double stdev = get_double_param(params, "gauss_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double step = get_double_param(params, "gauss_generation_step");
    double resolution = get_double_param(params, "gauss_resolution");
    double multiplier = get_double_param(params, "gauss_func_multiplier");

    if (multiplier == 0){
	printf("WARNING: Multiplier for function generation is zero! Your functions"\
	       " will probably be completely flat!\n");
    }
    
    if (outfile == NULL){
	if (infile == NULL){
	    if (output_type == 0) {
		printf("Outputting raw uniformly spaced gaussians.\n");
		outfile = get_string_param(params, "gauss_func_outfile_raw");
	    } else {
		printf("Outputting discrete transform of uniformly spaced"\
		       " gaussians.\n");
		outfile = get_string_param(params, "gauss_func_outfile");
	    }
	} else {
	    if (output_type == 0) {
		printf("Outputting raw gaussians with means centred on data "\
		       "from file.\n");
		outfile = get_string_param(params, "gauss_event_func_outfile_raw");
	    } else {
		printf("Outputting discrete transform of gaussians with means"\
		       " centred on data from file.\n");
		outfile = get_string_param(params, "gauss_event_func_outfile");
	    }
	}
    }

    printf("Reading from %s.\n", infile);

    int i;

    char* out = malloc(strlen(outfile) + 5 + strlen(".dat"));

    for (i = 0; i < number; ++i) {
	sprintf(out, "%s_%d.dat", outfile, i);
	gauss_vector* G = _generate_gaussian(infile, stdev, start, interval, 
					     step, resolution, multiplier);
	
	double* wts = G->w;
	G->w = multiply_arr(wts, G->len, multiplier);
	free(wts);

	if (output_type == 0){
	    output_gaussian_vector(out, "w", G);
	} else {
	    // Need to apply something to normalise when you have input file. Dividing by the
	    // stdev is ok, but not good enough.
	    double_multi_arr* func = shifted_transform(G, start, interval, step, resolution);
    	    output_double_multi_arr(out, "w", func);
    	    free_double_multi_arr(func);
	    if (output_type == 2){
	    	char* c_out = malloc(strlen(outfile) + strlen("_contrib") + strlen(".dat") + 5);
	    	sprintf(c_out, "%s_%d_contrib.dat", outfile, i);
	    	output_gaussian_contributions(c_out, "w", G, start, start + interval, resolution, 1);
	    	free(c_out);
	    }
	}
	free_gauss_vector(G);
	printf("Output to %s.\n", out);
    }

    free(out);
    free_list(params);
}

/*
 * Generate a set of weighted gaussians. If an input file is provided, data points
 * are assumed to provide the means for gaussians.
 */
gauss_vector* _generate_gaussian(char* infile, double stdev, double start,
				 double interval, double gen_step, 
				 double resolution, double multiplier)
{
    double* means = NULL;
    gauss_vector* G;
    
    if (infile == NULL){
	G = gen_gaussian_vector_uniform(stdev, start, start + interval, gen_step, multiplier);
    } else {
	means = get_event_data_all(infile);
	G = gen_gaussian_vector_from_array(means + 1, means[0] - 1, stdev, multiplier, 1);
	free(means);
    }

    return G;
}

/*
 * Check the parsed equation to ensure that there are values for each
 * variable specified in the parameter file, and that a variable t is
 * present in the equation.
 */
int check_expr_vars(muParserHandle_t hparser, struct paramlist *params)
{
    char* tmp;
    int valid = 1;
    int tdef = 0;

    printf("Checking expression %s\n", mupGetExpr(hparser));
    int nvars = mupGetExprVarNum(hparser);
        
    if (nvars == 0){
	printf("Expression has no variables.\n");
	valid = 0;
    } else {
	int i;
	const muChar_t *mu_vname = 0;
	muFloat_t *vaddr = 0;
	char *vname;
	    
	for (i = 0; i < nvars; ++i){
	    // Get the variable name and value from the parser
	    mupGetExprVar(hparser, i, &mu_vname, &vaddr);
	    vname = (char*)mu_vname; // Cast to stop warnings
	    if ((tmp = get_string_param(params, vname)) != NULL){
		*vaddr = atof(tmp); // Set the value of the variable inside hparser
		printf("Varname: %s, value: %lf\n", vname, (double)*vaddr);
	    } else if (strcmp(vname, "t") == 0){
		printf("Variable t present.\n");
		tdef = 1;
	    } else if (tmp == NULL){
		printf("Value for %s is not defined in param file. Aborting.\n", vname);
		valid = 0;
		break;
	    } 
	}

	if (tdef != 1){
	    printf("The expression must contain a variable t.\n");
	    valid = 0;
	}
    }
    
    return valid;
}

/*
 * Display all variables in muparser, as well as their current values.
 */
void view_expr(muParserHandle_t hparser)
{
    int nvars = mupGetExprVarNum(hparser);
        
    int i;
    const muChar_t *mu_vname = 0;
    muFloat_t *vaddr = 0;
    char *vname;
	    
    for (i = 0; i < nvars; ++i){
	// Get the variable name and value from the parser
	mupGetExprVar(hparser, i, &mu_vname, &vaddr);
	vname = (char*)mu_vname; // Cast to stop warnings
	printf("Varname %s, value %lf\n", vname, (double)*vaddr);
    }
}

/*
 * Factory for creating new muparser variables. Space is automatically allocated
 * by the parser. Values can be retrieved by calling mupGetExprVar().
 */
muFloat_t* var_factory(const muChar_t* varname, void *pUserData)
{
  static muFloat_t var_buf[PARSER_MAXVARS];
  static int num_vars = 0;

  printf("Generating new variable \"%s\" (slots left: %d)\n", varname, PARSER_MAXVARS-num_vars);

  var_buf[num_vars] = 0;
  if (num_vars >= PARSER_MAXVARS - 1)
  {
     printf("Variable buffer overflow.");
     return NULL;
  }

  return &var_buf[num_vars++];
}

/*
 * Muparser error handler. Called whenever an error occurs when attempting to evaluate
 * a parsed expression.
 */
void on_error(muParserHandle_t hparser)
{
  printf("\nError when parsing equation:\n");
  printf("------\n");
  printf("Message:  \"%s\"\n", mupGetErrorMsg(hparser));
  printf("Token:    \"%s\"\n", mupGetErrorToken(hparser));
  printf("Position: %d\n", mupGetErrorPos(hparser));
  printf("Errc:     %d\n", mupGetErrorCode(hparser));
}

/* 
 * Generates a series of streams, with a time delay between them. 
 * The time_delta array should contain the difference in time between 
 * each consecutive stream. The values affect the result of evaluating
 * the lambda function at each timestep. e.g. for a time_delta [10.0, 25.0]
 * the value passed to the first stream's lambda function will be t + 10.0,
 * and the second will be t + 25.0. In this case, t is usually zero. 
 * The outswitch parameter determines whether all data will be output to the file,
 * or just the event times.
 * *IMPORTANT* The value of lambda MUST exceed the maximum value of the function! *IMPORTANT*
 */
void run_time_nstreams(muParserHandle_t hparser, double lambda, double end_time,
		       double *time_delta, int nstreams, char *outfile, int outswitch)
{
    int i;
    char *out = malloc(strlen(outfile) + strlen("_stream_n") + 10);
    for (i = 0; i < nstreams; ++i){
	printf("Generating event stream %d\n", i);
	sprintf(out, "%s_stream_%d", outfile, i); // save each stream to a separate file
	run_time_nonhom(hparser, lambda, time_delta[i], 0, end_time, out, outswitch);
    }
    free(out);
}

/*
 * Selects an output file. Will prompt user if there is a file specified in both
 * the command line and in the parameter file.
 */
char* select_output_file(char* cur_out, char* param_out)
{
    char* outfile;
    
    if (param_out == NULL && cur_out == NULL){
	printf("No parameter for the output file found in the parameters file "\
	       "or command line arguments. Auto-generating...\n");
	outfile = generate_outfile();
	generated_outfile = 1;
    } else if (param_out != NULL && cur_out != NULL){
	printf("Output file found in both parameter file and command line arguments.\n");
	int u = -1;
	int res = 0;
	while (res == 0 || (u < 0 || u > 1)){
	    printf("To use the file %s, enter 1. To use the file %s, enter 0.\n", param_out, cur_out);
	    res = scanf("%d", &u);
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
    
    return outfile;
}

/* Helper method to run a nonhomogeneous process for a specific length
 * of time. Allocates required memory and prints output data to a file.
 * The outswitch parameter determines what data will be output to the file.
 * 
 */
void run_time_nonhom(muParserHandle_t hparser, double lambda, double time_delta,
		     double start_time, double end_time, char *outfile, int outswitch)
{
    double *et = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    double *lv = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    
    double **eptr = &et;
    double **lptr = &lv;

    int size = run_to_time_non_homogeneous(hparser, lambda, time_delta, start_time,\
					   end_time, eptr, lptr, DEFAULT_ARR_SIZE);

    if (outswitch == 0) // Outputs only event data - this is what the real data will be like.
	double_to_file(outfile, "w", *eptr, size);
    if (outswitch == 1) // Outputs only events and lambda values
	mult_double_to_file(outfile, "w", *eptr, *lptr, size);
    if (outswitch == 2){ // Outputs all data, including bin counts
	int num_intervals = (end_time - start_time) / DEFAULT_WINDOW_SIZE;

	int *bin_counts = sum_events_in_interval(*eptr, size, start_time, end_time, num_intervals);
	double *midpoints = get_interval_midpoints(start_time, end_time, num_intervals);

#ifdef VERBOSE
	int i;
	for (i = 0; i < num_intervals; ++i){
	    printf("Interval %d: %d\n", i, bin_counts[i]);
	}

	for (i = 0; i < num_intervals; ++i){
	    printf("%lf\n", midpoints[i]);
	}
#endif
    
	int_dbl_to_file(outfile, "w", midpoints, bin_counts, num_intervals);
	free(midpoints);
	free(bin_counts);
    }
    if (outswitch == 3){
	char *tmp = malloc(strlen(outfile) + 4 + strlen(".dat")); // space for extra chars and null terminator
	sprintf(tmp, "%s%s", outfile, ".dat");
	double_to_file(tmp, "w", *eptr, size);
	
	sprintf(tmp, "%s_ad%s", outfile, ".dat");

	mult_double_to_file(tmp, "w", *eptr, *lptr, size);

	int num_intervals = (end_time - start_time) / DEFAULT_WINDOW_SIZE;

	int *bin_counts = sum_events_in_interval(*eptr, size, start_time, end_time, num_intervals);
	double *midpoints = get_interval_midpoints(start_time, end_time, num_intervals);

#ifdef VERBOSE
	int i;
	for (i = 0; i < num_intervals; ++i){
	    printf("Interval %d: %d\n", i, bin_counts[i]);
	}

	for (i = 0; i < num_intervals; ++i){
	    printf("midpoint %d: %lf\n", i, midpoints[i]);
	}
#endif
    
	int_dbl_to_file(tmp, "a", midpoints, bin_counts, num_intervals);
	free(midpoints);
	free(bin_counts);
	free(tmp);
    }

    free(*eptr);
    free(*lptr);
}

/* 
 * Generates time for events in a homogenous poisson process until 
 * time is exceeded. 
 * Puts event times into the array passed in the parameters. 
 * Puts a -1 in the array location after the last event
 */
void generate_event_times_homogenous(double lambda, double time, int max_events,
				     double *event_times)
{
    init_rand(0.0);
        
    double run_time = 0;
    int i = 0;
    
    while ((run_time += homogeneous_time(lambda)) < time && i < max_events){
	event_times[i] = run_time;
	++i;
    }

    if (i < max_events)
	event_times[i] = -1.0;
    
}

/* 
 * Runs a non-homogeneous poisson process until the specified time has
 * elapsed. 
 * The event_times and lambda_vals array will be populated with the 
 * time of an event and the result of evaluating lambda(t) at that 
 * time. Will attempt to reallocate memory for arrays if the number 
 * of events exceeds the size of arrays passed. 
 * Returns the final array location in which there is something 
 * stored.
 */
int run_to_time_non_homogeneous(muParserHandle_t hparser, double lambda, 
				double time_delta, double start_time, 
				double end_time, double **event_times, 
				double **lambda_vals, int arr_len)
{
    init_rand(0.0);
            
    double base_time = start_time;
    double shifted_time = time_delta + start_time;
    double rand, non_hom_lambda, hom_out;
    int i = 0, arr_max = arr_len;
    
    mupDefineVar(hparser, "t", &shifted_time); // Set the address muparser uses for variable t

    while (base_time < end_time){
	hom_out = homogeneous_time(lambda);
	base_time += hom_out;
	shifted_time += hom_out;
	non_hom_lambda = mupEval(hparser);
	//printf("%lf %lf\n", time, non_hom_lambda);//more granularity on lambda values // get this into output file
	if ((rand = get_uniform_rand()) <= non_hom_lambda / lambda){
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
	    event_times[0][i] = base_time;
	    lambda_vals[0][i] = non_hom_lambda;
	    ++i;
	}
	
    }

    return i;
    
}

/* 
 * Helper method to run a nonhomogeneous process until a specific 
 * number of events have occurred. Allocates required memory
 * and prints output data to a file. The outswitch parameter determines whether
 * all data will be output to the file, or just the event times.
 */
void run_events_nonhom(muParserHandle_t hparser, double lambda, double start_time,
		       int events, char *outfile, int outswitch)
{
    double *et = malloc(events * sizeof(double));
    double *lv = malloc(events * sizeof(double));

    run_to_event_limit_non_homogeneous(hparser, lambda, start_time, events, et, lv);
    mult_double_to_file(outfile, "a", et, lv, events);
}

/* 
 * Runs a non-homogeneous poisson process until the number of events 
 * specified have occurred. The event_times and lambda_vals array 
 * will be populated with the time of an event and the result of 
 * evaluating lambda(t) at that time.
 */
void run_to_event_limit_non_homogeneous(muParserHandle_t hparser, double lambda, 
					double t_delta, int max_events, 
					double *event_times, double *lambda_vals)
{
    init_rand(0.0);
    
    double run_time = 0, func_in = t_delta + run_time;
    double rand, non_hom_lambda, hom_out;
    int i = 0;
    
    mupDefineVar(hparser, "t", &func_in);
            
    while (i < max_events){
	hom_out = homogeneous_time(lambda);
	run_time += hom_out;
	func_in += hom_out;
	non_hom_lambda = mupEval(hparser);
	//printf("%lf %lf\n", run_time, non_hom_lambda);//more granularity on lambda values // get into the output file
	if ((rand = get_uniform_rand()) <= non_hom_lambda / lambda){
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

/* 
 * Generates time for events in a homogeneous poisson process until 
 * time is exceeded. 
 * Puts event times into the array passed in the parameters. 
 * Puts a -1 in the array location after the last event
 */
void generate_event_times_homogeneous(double lambda, double time,
				      int max_events, double *event_times)
{
    init_rand(0.0);
        
    double run_time = 0;
    int i = 0;
    
    while ((run_time += homogeneous_time(lambda)) < time && i < max_events){
	event_times[i] = run_time;
	++i;
    }

    if (i < max_events)
	event_times[i] = -1.0;
    
}

/* knuth method. Generates time to next event in a homogeneous poisson process. */
double homogeneous_time(double lambda)
{
    return -log(get_uniform_rand()) / lambda;
}
