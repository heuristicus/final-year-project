#include "generator.h"
#include "paramlist.h"
#include "file_util.h"
#include "math_util.h"
#include "general_util.h"

#define DEFAULT_ARR_SIZE 50
#define PARSER_MAXVARS 10

static int generated_outfile = 0;
static char* generator_params[] = {"nstreams", "lambda", "interval_time",
				   "timedelta", "verbosity", "expression"};

void on_error(muParserHandle_t hParser);
muFloat_t* var_factory(const muChar_t* a_szName, void* pUserData);
int check_expr_vars(muParserHandle_t hparser, struct paramlist* params);
double* parser_tptr(muParserHandle_t hparser);
void view_expr(muParserHandle_t hparser);

void generate(char* paramfile, char* outfile, int nfuncs, int nstreams, int output_switch)
{
    paramlist* params = get_parameters(paramfile);
    char* prefix = get_string_param(params, "expression_outfile");

    if (outfile == NULL){
	outfile = get_string_param(params, "outfile");
	if (outfile == NULL){
	    printf("You must specify an output file to use. Add \"outfile\" to your"\
		   "parameter file.\n");
	    exit(1);
   	}
    }
    
    if (!has_required_params(params, generator_params, sizeof(generator_params)/sizeof(char*))){
	print_string_array("Some parameters required for generating streams are missing. " \
			   "Ensure that your parameter file contains the following entries"\
			   " and try again. If you have not defined the expression to use, "\
			   " this can be done by adding something like \"expression a+b*x\""\
			   " and defining values for the variables; \"a 10\" etc.",
			   generator_params, sizeof(generator_params)/sizeof(char*));
	exit(1);
    }

    if (nstreams == 1)
	nstreams = get_int_param(params, "nstreams");
    double lambda = get_double_param(params, "lambda");
    double interval_time = get_double_param(params, "interval_time");
    double_arr* time_delta = get_double_list_param(params, "timedelta");
    char* expression = get_string_param(params, "expression");
    char* stream_ext = get_string_param(params, "stream_ext");
    double output_interval = get_double_param(params, "output_interval");
    double step = get_double_param(params, "output_step");

    muParserHandle_t hparser = mupCreate(0);
    mupSetVarFactory(hparser, var_factory, NULL);
    mupSetErrorHandler(hparser, on_error);
    
    mupSetExpr(hparser, expression);

    if (!check_expr_vars(hparser, params) || mupError(hparser)){
	printf("Expression is invalid. Check that you have defined all the variables.\n");
	mupRelease(hparser);

	free_list(params);
	free(time_delta);
	exit(1);
    }

    char* out = malloc(strlen(prefix) + strlen(outfile) + strlen(stream_ext) + 10);

    if (nfuncs > 1){
	list_to_file("gen_params.txt", "w", params);
    }

    if (nstreams > time_delta->len){
	printf("Number of streams specified (%d) is greater than number of timedelta values (%d)."\
	       " Please reduce the number of streams or add more timedelta values.\n", nstreams, time_delta->len);
	exit(1);
    }

    int i;
    
    init_rand(0.0);

    for (i = 0; i < nfuncs; ++i) {
	printf("Generating stream set %d.\n", i);
	sprintf(out, "%s_%d_%s%s", prefix, i, outfile, stream_ext);
	_generate(params, out, interval_time, lambda, hparser, time_delta, output_switch,
		  nstreams, step, output_interval);
    }

    free(out);
    mupRelease(hparser);
    free_double_arr(time_delta);
    free_list(params);
}

/*
 * Initialises the generator with some values specified by the arguments passed to the program,
 * and those that are contained in the parameter file, if there is one (there should be).
 * The order in which the arguments are is defined inside start.c
 */
void _generate(paramlist* params, char* outfile, double interval_time, double lambda, 
	       muParserHandle_t hparser, double_arr* time_delta, int output_switch,
	       int nstreams, double step, double output_interval)
{
#ifdef VERBOSE
    printf("Number of runs: %d\nLambda: %lf\nInterval time: %lf\nTime deltas: ",
	   nstreams, lambda, interval_time);
    int i;

    for (i = 0; i < tdlen; ++i) {
	printf("%lf", time_delta[i]);
	if (i != tdlen - 1)
	    printf(", ");
	else
	    printf("\n");
    }

    printf("Log verbosity: %d\n", outswitch);
#endif

    run_time_nstreams(hparser, lambda, interval_time, time_delta, nstreams,
		      outfile, output_switch, step, output_interval);
}

/*
 * Generates a series of event streams from a number of functions. The functions
 * must have filenames which follow a given scheme, defined by parameters in the
 * paramfile. The copy_base parameter specifies whether to copy a single generated
 * function into multiple functions. This has the effect of generating multiple stream
 * pairs from the same function.
 */
void generate_from_gaussian(char* paramfile, char* outfile, char* infile,
			    int nstreams, int nfuncs, int output_type, int copy_base)
{
    int i;
    paramlist* params = get_parameters(paramfile);
    
    // Initialise random number generator here so that fast regeneration does not produce
    // duplicates.
    init_rand(0.0);
    
    double stdev = get_double_param(params, "gauss_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double step = get_double_param(params, "gauss_generation_step");
    double resolution = get_double_param(params, "gauss_resolution");
    char* stream_ext = get_string_param(params, "stream_ext");
    double_arr* time_delta = get_double_list_param(params, "timedelta");
    char* funcfile = get_string_param(params, "function_outfile");

    if (time_delta->len < nstreams){
	printf("You have not specified time delta values between all streams.\n"\
	       "Specified: %d, required: %d.\nPlease add values to the timedelta"\
	       " parameter in your parameter file.\n", time_delta->len, nstreams);
	exit(1);
    }

    if (outfile == NULL){
	outfile = get_string_param(params, "outfile");
    }

    if (nfuncs > 1){
	list_to_file("gen_params.txt", "w", params);
    }

    if (nstreams > time_delta->len){
	printf("Number of streams specified (%d) is greater than number of timedelta values (%d)."\
	       " Please reduce the number of streams or add more timedelta values.\n", nstreams, time_delta->len);
	exit(1);
    }

    char* infname = malloc(strlen(funcfile) + 5);
    sprintf(infname, "%s_%d.dat", funcfile, i);
    FILE* fp = NULL;
    
    if ((fp = fopen(infname, "r")) == NULL || copy_base){
	if (errno == ENOENT || copy_base) {
	    printf("File %s not found. Generating gaussian data.\n", infname);
	    if (copy_base){
		generate_gaussian_data(paramfile, NULL, NULL, 1, output_type);
		copy_gauss_files(funcfile, nfuncs, output_type);
	    } else {
		generate_gaussian_data(paramfile, NULL, NULL, nfuncs, output_type);
	    }
	    
	} else {
	    printf("Could not access file %s. Exiting.\n", infname);
	    perror("Error");
	    exit(1);
	}
    } else {
	fclose(fp);
    }

    for (i = 0; i < nfuncs; ++i) {
	sprintf(infname, "%s_%d.dat", funcfile, i);
	_generate_from_gaussian(params, outfile, infname, stdev, start, interval,
				step, resolution, stream_ext, time_delta, nstreams);
    }

    free(infname);
    free_double_arr(time_delta);
}

/*
 * Generates an event stream by using a random function provided by a set of 
 * gaussians. The gaussians are expected as input in raw form from the infile.
 * If no input is provided, a random function will be generated and events will
 * be generated from that.
 */
void _generate_from_gaussian(paramlist* params, char* outfile, char* infile,
			     double stdev, double start, double interval,
			     double step, double resolution, char* stream_ext,
			     double_arr* time_delta, int nstreams)
{
    int i;
    gauss_vector* G = NULL;

    if (infile == NULL){
	// Should always be passed an input file - exit if not.
	printf("Expected input file to _generate_from_gaussian. Exiting.\n");
	exit(1);
    } else {
	G = read_gauss_vector(infile);
	printf("Reading from %s.\n", infile);
    }
    
    double_multi_arr* T = gauss_transform(G, start, start+interval, resolution);
    double max = find_max_value(T->data[1], T->lengths[1]);
    double min = find_min_value(T->data[1], T->lengths[1]);
    double lambda = ceil(-min + max);

    char* out = malloc(strlen(outfile) + strlen(stream_ext) + 5 + strlen(".dat") + strlen(infile));
    char* intrunc = malloc(strlen(infile));
    snprintf(intrunc, strlen(infile) - 3, "%s", infile);

    for (i = 0; i < nstreams; ++i) {
	sprintf(out, "%s_%s%s%d.dat", intrunc, outfile, stream_ext, i);
    	double_multi_arr* stream = nonhom_from_gaussian(G, lambda, start, interval, time_delta->data[i], -min);
	output_double_multi_arr(out, "w", stream);
	free_double_multi_arr(stream);
    }

    free_gauss_vector(G);
    free_double_multi_arr(T);
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
	if (base_time >= end)
	    break;
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
 * are generated uniformly over the interval specified in the parameter file. The 
 * output type specifies what data to output about the generated data. You will always
 * get the gaussians in their raw form, with a value of 0, and any increase on that will
 * generate more files.
 * A value of 0 outputs raw gaussians, which can be used to generate event streams. 
 * 1 outputs the discrete transform, which gives the value of the function at evenly
 * spaced points along the x-axis. 2 outputs the contribution of each individual gaussian
 * used to create the function to file.
 */
void generate_gaussian_data(char* paramfile, char* infile, char* outfile,
			    int number, int output_type)
{
    paramlist* params = get_parameters(paramfile);
    
    double stdev = -1, alpha = -1;
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double step = get_double_param(params, "gauss_generation_step");
    double resolution = get_double_param(params, "gauss_resolution");
    double multiplier = get_double_param(params, "gauss_func_multiplier");

    init_rand(0.0);

    if (strcmp(get_string_param(params, "simple_stdev"), "no") == 0){
	alpha = get_double_param(params, "stdev_alpha");
	if (alpha == -1){
	    printf("You have not specified the alpha to use to calculate the"\
		   " standard deviation. Add \"stdev_alpha\" to your parameter"\
		   " file.\n");
	    free_list(params);
	    exit(1);
	} else {
	    stdev = alpha * step;
	    printf("Gaussians will have standard deviation  %lf * %lf (%lf).\n",
		   alpha, step, alpha * step);
	}
    } else {
	stdev = get_double_param(params, "gauss_stdev");
	if (stdev == -1){
	    printf("You have not specified the standard devation for the gaussians"\
		   " used to generate functions. Please add \"gauss_stdev\" to your"\
		   " parameter file.\n");
	    free_list(params);
	    exit(1);
	}
    }
    
    if (multiplier == 0){
	printf("WARNING: Multiplier for function generation is zero! Your functions"\
	       " will probably be completely flat!\n");
    }
    
    if (outfile == NULL && output_type > 0){
    	if (infile == NULL){
	    outfile = get_string_param(params, "function_outfile");
    	} else {
	    outfile = get_string_param(params, "stream_function_outfile");
	}
	if (outfile == NULL){
	    printf("No file to output to could be found. Please define function"\
		   "_outfile or stream_outfile in your parameter file, or provide"\
		   " one using the -o switch. Default parameter files can be"\
		   " created using the -d switch.\n");
	    free_list(params);
	    exit(1);
	}
    }

    printf("Reading from %s.\n", infile);

    if (outfile != NULL && output_type != 0){
	// Allocate enough memory for the output filename so that we can reuse it
	char* out = malloc(strlen(outfile) + 5 + strlen(".dat") + strlen("_contrib"));
	
	int i;
	for (i = 0; i < number; ++i) {
	    sprintf(out, "%s_%d.dat", outfile, i);
	    gauss_vector* G = _generate_gaussian(infile, stdev, start, interval, 
						 step, resolution, multiplier);
	
	    double* wts = G->w;
	    G->w = multiply_arr(wts, G->len, multiplier);
	    free(wts);

	    if (output_type >= 1){
		output_gaussian_vector(out, "w", G);
		printf("Raw gaussian data output to %s. Use this file if you wish"\
		       " to generate event streams.\n", out);
	    }
	    if (output_type >= 2){
		// Normalisation might be necesary here
		sprintf(out, "%s_%d_sum.dat", outfile, i);
		double_multi_arr* func = shifted_transform(G, start, interval, step, resolution);
		output_double_multi_arr(out, "w", func);
		free_double_multi_arr(func);
		printf("Gaussian sum output to %s.\n", out);
	    }
	    if (output_type >= 3){
		sprintf(out, "%s_%d_contrib.dat", outfile, i);
		output_gaussian_contributions(out, "w", G, start, start + interval, resolution, 1);
		printf("Individual gaussian contributions output to %s.\n", out);
	    }
	    free_gauss_vector(G);
	}
	free(out);
    }
    

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
    double_arr* means = NULL;
    gauss_vector* G;
    
    if (infile == NULL){
	G = gen_gaussian_vector_uniform(stdev, start, start + interval, gen_step, multiplier);
    } else {
	means = get_event_data_all(infile);
	G = gen_gaussian_vector_from_array(means->data, means->len, stdev, multiplier, 1);
	free(means);
    }

    return G;
}

/*
 * Check the parsed equation to ensure that there are values for each
 * variable specified in the parameter file, and that a variable t is
 * present in the equation.
 */
int check_expr_vars(muParserHandle_t hparser, struct paramlist* params)
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
	const muChar_t* mu_vname = 0;
	muFloat_t* vaddr = 0;
	char* vname;
	    
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
    const muChar_t* mu_vname = 0;
    muFloat_t* vaddr = 0;
    char* vname;
	    
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
muFloat_t* var_factory(const muChar_t* varname, void* pUserData)
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
		       double_arr* time_delta, int nstreams, char* outfile,
		       int outswitch, double step, double output_interval)
{
    int i;
    char* out = malloc(strlen(outfile) + 20);
    for (i = 0; i < nstreams; ++i){
	printf("Generating event stream %d\n", i);
	sprintf(out, "%s%d", outfile, i); // save each stream to a separate file
	run_time_nonhom(hparser, lambda, time_delta->data[i], 0, end_time, out,
			outswitch, step, output_interval);
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
	printf("No parameter for the output file found in the parameter file"\
	       " or command line arguments. Auto-generating...\n");
	outfile = generate_outfile("generator_output", 0);
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
		     double start_time, double end_time, char *outfile, int outswitch,
		     double step, double output_interval)
{
    double_arr* events = run_to_time_non_homogeneous(hparser, lambda, time_delta, start_time, \
						     end_time, DEFAULT_ARR_SIZE);

    if (outswitch > 0){
	char* out = malloc(strlen(outfile) + strlen("_function.dat") + 10);
	if (outswitch >= 1){
	    sprintf(out, "%s.dat", outfile);
	    //printf("Event data output to %s\n", out);
	    double_to_file(out, "w", events->data, events->len);
	} 
	if (outswitch >= 2){
	    sprintf(out, "%s_func.dat", outfile);
	    FILE *fp = fopen(out, "w");
	    double time = start_time;
	    mupDefineVar(hparser, "t", &time); // Set the address muparser uses for variable t
	    while (time <= end_time){
		fprintf(fp, "%lf %lf\n", time + time_delta, mupEval(hparser));
		time += step;
	    }
	    fclose(fp);
	    printf("Function data output to %s\n", out);
	}
	if (outswitch >= 3){
	    sprintf(out, "%s_bins.dat", outfile);
	    int num_intervals = (end_time - start_time) / output_interval;

	    int* bin_counts = sum_events_in_interval(events->data, events->len, start_time, end_time, num_intervals);
	    double* midpoints = get_interval_midpoints(start_time, end_time, num_intervals);
    
	    int_dbl_to_file(out, "w", midpoints, bin_counts, num_intervals);
	    free(midpoints);
	    free(bin_counts);
	    //printf("Bin data output to %s\n", out);
	}
	free(out);
    }
    free_double_arr(events);
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
double_arr* run_to_time_non_homogeneous(muParserHandle_t hparser, double lambda, 
				double time_delta, double start_time, 
				double end_time, int arr_len)
{
    double base_time = start_time;
    double shifted_time = time_delta + start_time;
    double rand, non_hom_lambda, hom_out;
    int i = 0, arr_max = arr_len;
    double_arr* ret = init_double_arr(arr_len);
    
    mupDefineVar(hparser, "t", &shifted_time); // Set the address muparser uses for variable t

    while (base_time < end_time){
	hom_out = homogeneous_time(lambda);
	base_time += hom_out;
	if (base_time >= end_time)
	    break;
	shifted_time += hom_out;
	non_hom_lambda = mupEval(hparser);
	//printf("%lf %lf\n", time, non_hom_lambda);//more granularity on lambda values // get this into output file
	if ((rand = get_uniform_rand()) <= non_hom_lambda / lambda){
	    // Number of events may exceed the number of array locations initally assigned
	    // so may need to reallocate memory to store more.
	    if (i >= arr_max){
		//printf("array length %d exceeds initial max %d\n", i,	arr_len);
		ret->data = realloc(ret->data, i * 2 * sizeof(double));
		if (!ret->data){// exit if allocation failed.
		    printf("Memory reallocation for arrays failed. Exiting.\n");
		    exit(1);
		}
		arr_max = i * 2;
	    }
	    ret->data[i] = base_time;
	    ++i;
	}
	
    }
    ret->len = i;
    return ret;
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
    double* et = malloc(events * sizeof(double));
    double* lv = malloc(events * sizeof(double));

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
