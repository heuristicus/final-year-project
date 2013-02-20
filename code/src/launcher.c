#include "launcher.h"
 
static char *estimators[] = {"iwls", "ols", "pc", "base", "gauss"};
static char *generators[] = {"mup", "rand"};
static struct option opts[] =
    {
	{"experiment", required_argument, 0, 'x'},
	{"generate", required_argument, 0, 'g'},
	{"estimate", required_argument, 0, 'e'},
	{"estimator", required_argument, 0, 'a'},
	{"infile",  required_argument, 0, 'i'},
	{"outfile",  required_argument, 0, 'o'},
	{"paramfile",  required_argument, 0, 'p'},
	{"defparam", required_argument, 0, 'd'},
	{"nstreams",    required_argument, 0, 'n'},
	{"outtype",    required_argument, 0, 't'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
    };

int main(int argc, char *argv[])
{
    int c;
    int opt_ind;
    
    launcher_args* args = make_arg_struct();
    char* paramfile = NULL;
    char* extra_paramfile = NULL;
    char* outfile = NULL;
    char* infile = NULL;
    char* estimator_type = NULL;
    char* generator_type = NULL;

    if (argc == 1){
	printf("%s\n\nusage: %s options\n\n%s\n%s\n%s\n", PROG_DESC, argv[0], OPT_INFO, VERSION, BUGREPORT);
	exit(1);
    }
        
    while((c = getopt_long(argc, argv, "x:g:e:a:i:o:d:n:f:hrt:", opts, &opt_ind)) != -1){
    	switch(c){
    	case 'e': // estimate
    	    args->est = 1;
    	    paramfile = strdup(optarg);
    	    break;
	case 'a': // estimation algorithm
	    if (!exists_in_arr(estimators, sizeof(estimators)/sizeof(char*), optarg)){
		printf(EST_TYPE_ERROR, optarg);
		exit(1);
	    } else {
		printf("Estimation will be performed using estimator %s\n", optarg);
		estimator_type = strdup(optarg);
	    }
	    break;
	case 'd': // default paramfile
	    create_default_param_file(optarg);
	    exit(1);
	    break;
	case 'f': // specify generator type/function
	    if (!exists_in_arr(generators, sizeof(generators)/sizeof(char*), optarg)){
		printf("Unknown generator type %s. Use mup or rand.\n", optarg);
		exit(1);
	    } else {
		generator_type = strdup(optarg);
	    }
	    break;
    	case 'g': // generate streams or functions
    	    args->gen = 1;
    	    paramfile = strdup(optarg);
    	    break;
    	case 'h': // print help
    	    printf("%s\n\n%s\n%s\n%s\n", PROG_DESC, OPT_INFO, VERSION, BUGREPORT);
    	    exit(1);
    	case 'i': // specify input file
    	    infile = strdup(optarg);
    	    break;
    	case 'n': // specify number of streams/functions
    	    args->nstreams = atoi(optarg);
    	    break;
    	case 'o': // specify output file
    	    outfile = strdup(optarg);
    	    break;
	case 'r': // switch to generate random functions. Used with -g
	    args->rfunc = 1;
    	    break;
	case 't': // specify output type when generating random functions
	    args->writing = atoi(optarg);
	    break;
	case 'p':
	    extra_paramfile = strdup(optarg);
	    break;
    	case 'x':
    	    args->exp = 1;
    	    paramfile = strdup(optarg);
    	    break;
    	default:
    	    printf("%s\n\nusage: %s options\n\n%s\n", PROG_DESC, argv[0], OPT_INFO);
    	    exit(1);
    	}

	if (args->exp + args->gen + args->est > 1){
    		printf("Choose only one of -e, -g, -x. You can run either"\
		       " an estimator, experiments or the generator,"\
		       "but not more than one at once.\n");
    		exit(1);
	}
    }


    run_requested_operations(args, paramfile, extra_paramfile, infile, outfile, estimator_type, generator_type);

    free(estimator_type);
    free(generator_type);
    free(paramfile);
    free(extra_paramfile);
    free(infile);
    free(outfile);
    free(args);

    return 0;
}

void run_requested_operations(launcher_args* args, char* paramfile, char* extra_paramfile,
			      char* infile, char* outfile, char* estimator_type, 
			      char* generator_type)
{
    if (args->gen == 1){
	if (paramfile == NULL){
	    printf("You must specify a parameter file to use.\nTry running "\
		   "\"launcher -g [your parameter file]\"\n");
	    exit(1);
	} else if (args->rfunc == 1){
	    printf("Generating gaussians.\n");
	    generate_gaussian_data(paramfile, infile, outfile, args->nstreams, args->writing);
	} else if (generator_type == NULL || strcmp(generator_type, "mup") == 0){
	    printf("Generating event stream with expression from parameter file.\n");
	    generate(paramfile, outfile, args->nstreams, args->writing);
	} else if (strcmp(generator_type, "rand") == 0){
	    printf("Generating event stream with random functions.\n");
	    generate_from_gaussian(paramfile, outfile, infile, args->nstreams);
	} else {
	    printf("something bad happened.\n");
	}
    } else if (args->est == 1){
	if (paramfile == NULL){
	    printf("You must specify a parameter file to use.\nTry running "\
		   "\"launcher -e [your parameter file] -a [estimator]\"\n");
	    exit(1);
	} else if (estimator_type == NULL){
	    printf("No estimator type specified. Retrieving from paramfile.\n");
	    paramlist* params = get_parameters(paramfile);
	    if ((estimator_type = get_string_param(params, "est_type")) == NULL){
		printf("Parameter est_type not specified in file. Aborting.\n");
		free_list(params);
		exit(1);
	    } else {
		printf("Estimator type set to %s.\n", estimator_type);
	    }
	}
	if (args->nstreams > 1){
	    printf("Running estimates of multiple (%d) streams.\n", args->nstreams);
	    multi_estimate(paramfile, infile, outfile, args->nstreams, args->writing, estimator_type);
	} else {
	    printf("Estimating single stream.\n");
	    // mess with outfile here to output it nicely.
	    est_arr* result = estimate(paramfile, infile, outfile, estimator_type);
	    if (result != NULL)
		free_est_arr(result);
	}
    } else if (args->exp == 1){
	printf("experimenting\n");
	if (extra_paramfile == NULL){
	    printf("You need to specify the file from which to read default parameters."\
		   " Use the -p switch to do so.\n");
	    exit(1);
	}
	run_experiments(paramfile, extra_paramfile);
    } else {
	printf("No action specified. You can run either an estimator, a generator or"\
	       " experiments by using the -e, -g or -x switches respectively.\n");
    }
}

/*
 * Estimate a series of streams. Constructs the file to read data from by using
 * parameters used to output the data from the generator, and then runs estimators
 * on each file. Data is then stored and once all estimates have been made the data
 * is combined to make a single estimate.
 */
void multi_estimate(char* paramfile, char* infile, char* outfile, int nstreams, int output_switch, char* estimator_type)
{
    paramlist* params = get_parameters(paramfile);

    char* fname = get_string_param(params, "outfile"); // default generator output filename
    char* pref = get_string_param(params, "stream_ext"); // default extension
    double step = get_double_param(params, "output_step");
    char* est_delta = get_string_param(params, "estimate_delta");
    double start = get_double_param(params, "est_start_time");
    double_arr* delays = NULL;// Will be used to store time delays
    int gauss = strcmp(estimator_type, "gauss") == 0;
    
    if (step <= 0)
	step = DEFAULT_STEP;

    if (outfile == NULL && output_switch != 0){
	if (gauss)
	    outfile = get_string_param(params, "gauss_est_outfile");
	else
	    outfile = get_string_param(params, "est_outfile");
    }
	    
    if (fname == NULL || pref == NULL){
	printf("You must include the parameters \"outfile\" and \"stream_ext\" in"\
	       " your parameter file.\n");
	exit(1);
    } else if (outfile == NULL && output_switch != 0){
	if (gauss)
	    printf("You must include the parameter \"gauss_est_outfile\" in your" \
		   " parameter file, or specify the output file with the -o switch.\n");
	else
	    printf("You must include the parameter \"est_outfile\" in your" \
		   " parameter file,area or specify the output file with the -o switch.\n");
	exit(1);
    }

    printf("running estimator %s for %d streams\n", estimator_type, nstreams);
    
    char* infname = malloc(strlen(fname) + strlen(pref) + strlen(".dat") + 5);
    char* outname = malloc(strlen(outfile) + strlen(".dat") + strlen("_combined") + 5);
    int i;

    void** estimates;
    
    if (gauss)
	estimates = malloc(sizeof(gauss_vector*) * nstreams);
    else
	estimates = malloc(sizeof(est_arr*) * nstreams);

    for (i = 0; i < nstreams; ++i) {
	sprintf(infname, "%s%s%d.dat", fname, pref, i);
	if (outfile != NULL && output_switch >= 1)
	    sprintf(outname, "%s_%d", outfile, i);
	
	if (gauss)
	    estimates[i] = estimate_gaussian_raw(params, infname, outname);
	else
	    estimates[i] = estimate(paramfile, infname, outname, estimator_type);
    }

    if (strcmp(est_delta, "yes") == 0){
	char* delta_method = get_string_param(params, "delta_est_method");
	char* hierarchical = get_string_param(params, "delta_est_hierarchical");
	delays = init_double_arr(nstreams);

	// The delay estimates take the first function as a base, so we assume that
	// it has no delay - the delays of the other functions will be calculated
	// relative to this.
	delays->data[0] = 0;

	// The input file for the base function is used for all delay estimates, so
	// just read it once.
	sprintf(infname, "%s%s0.dat", fname, pref);
	double_arr* base_stream_events = get_event_data_all(infname);
	double normaliser = 1;

	if (strcmp(delta_method, "pmf") == 0 && gauss){
	    // Find a normaliser to apply to the values of the estimated function.
	    // In its raw form, the estimated function is not on the same scale as
	    // the original if the gaussian estimator was used because of the way
	    // that gaussians are summed.
	    normaliser = find_normaliser(params, estimates[0], base_stream_events,
					 estimator_type);
	}

	for (i = 1; i < nstreams; ++i) {
	    printf("Estimating delta for stream 0 and stream %d\n", i);

	    // Read sets of events for the two streams from different files.
	    sprintf(infname, "%s%s%d.dat", fname, pref, i);
	    if (outfile != NULL && output_switch >= 1)
		sprintf(outname, "%s_%d", outfile, i-1);

	    double_arr* f2_events = get_event_data_all(infname);

	    if (strcmp(delta_method, "area") == 0){
		delays->data[i] = estimate_delay_area(params, outname, estimates[0],
						      estimates[i], hierarchical,
						      estimator_type, output_switch);
	    } else if (strcmp(delta_method, "pmf") == 0){
		delays->data[i] = estimate_delay_pmf(params, outname, base_stream_events,
						     f2_events, estimates[0],
						     estimates[i], normaliser, 
						     hierarchical, estimator_type, output_switch);
	    }
	    
	    // This data is not reused, so free it
	    free_double_arr(f2_events);
	}

	for (i = 0; i < delays->len; ++i) {
	    printf("Delay for stream %d: %lf\n", i, delays->data[i]);
	}

	free_double_arr(base_stream_events);
    } else {
	if ((delays = get_double_list_param(params, "timedelta")) == NULL){
	    printf("You must specify the time delay between each stream. "\
		   "Add something like \"timedelta 0,10,20\" to your parameter file\n");
	    exit(1);
	}
    }


    double interval_time = 0;

    if ((interval_time = get_double_param(params, "interval_time")) == 0){
	printf("interval_time not specified.\n");
	exit(1);
    }

    double_multi_arr* final_estimate = NULL;
    if (gauss) {
	final_estimate = combine_gauss_vectors((gauss_vector**)estimates, delays, start, interval_time, step, nstreams);
    } else {
	final_estimate = combine_functions((est_arr**)estimates, delays, start, interval_time, step, nstreams);
    }

    if (outfile != NULL && output_switch >= 1){
	sprintf(outname, "%s_combined.dat", outfile);
	output_double_multi_arr(outname, "w", final_estimate);
    }
    
    free(infname);
    free(outname);
    free_list(params);
    free_double_multi_arr(final_estimate);
    free_double_arr(delays);
    for (i = 0; i < nstreams; ++i) {
	if (gauss)
	    free_gauss_vector((gauss_vector*)estimates[i]);
	else
	    free_est_arr((est_arr*)estimates[i]);
    }
    free(estimates);
}

launcher_args* make_arg_struct()
{
    launcher_args* a = malloc(sizeof(launcher_args));
    
    a->est = 0;
    a->exp = 0;
    a->gauss = 0;
    a->gen = 0;
    a->nstreams = 1;
    a->writing = 1;
    a->rfunc = 0;

    return a;
}

/*
 * Checks whether a string exists in the given array
 */
int exists_in_arr(char** arr, int len, char* name)
{
    if (name == NULL)
	return 0;
    
    int i;
    
    for (i = 0; i < len; ++i) {
	if (strcmp(name, arr[i]) == 0)
	    return 1;
    }

    return 0;
}

