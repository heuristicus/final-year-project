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
	    // combine rfunc and gauss - they are pretty much the same!
	    printf("Generating gaussians.\n");
	    generate_gaussian_data(paramfile, infile, outfile, args->nstreams, args->writing);
	} else if (generator_type == NULL || strcmp(generator_type, "mup") == 0){
	    generate(paramfile, outfile, args->nstreams);
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
	    multi_estimate(paramfile, infile, outfile, estimator_type, args->nstreams);
	} else {
	    printf("estimating single stream\n");
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
void multi_estimate(char* paramfile, char* infile, char* outfile, char* estimator_type,
		    int nstreams)
{
    if (strcmp(estimator_type, "gauss") == 0){
	paramlist* params = get_parameters(paramfile);
	multi_est_gauss(params, infile, outfile, nstreams);
	free_list(params);
    } else {
	multi_est_default(paramfile, infile, outfile, estimator_type, nstreams);
    }
}

void multi_est_gauss(paramlist* params, char* infile, char* outfile, int nstreams)
{
    char* fname = get_string_param(params, "outfile"); // default generator output filename
    char* pref = get_string_param(params, "stream_ext"); // default extension
    
    if (outfile == NULL){
	outfile = get_string_param(params, "gauss_est_outfile");
    }
	    
    if (fname == NULL || pref == NULL){
	printf("You must include the parameters \"outfile\" and \"stream_ext\" in"\
	       " your parameter file.\n");
	exit(1);
    } else if (outfile == NULL){
	printf("You must include the parameter \"gauss_est_outfile\" in your"\
	       " parameter file, or specify the output file with the -o switch.\n");
	exit(1);
    }

    printf("running estimator %s for %d streams\n", "gauss", nstreams);
    char* infname = malloc(strlen(fname) + strlen(pref) + strlen(".dat") + 5);
    char* outname = malloc(strlen(outfile) + strlen(".dat") + 5);
    int i;
    
    for (i = 0; i < nstreams; ++i) {
	sprintf(infname, "%s%s%d.dat", fname, pref, i);
	sprintf(outname, "%s_%d.dat", outfile, i);
	estimate_gaussian(params, infname, outname);
    }
    free(infname);
    free(outname);
}

void multi_est_default(char* paramfile, char* infile, char* outfile, char* estimator_type, int nstreams)
{
    paramlist* params = get_parameters(paramfile);

    char* fname = get_string_param(params, "outfile"); // default generator output filename
    char* pref = get_string_param(params, "stream_ext"); // default extension
    double step = get_double_param(params, "output_sample_step");
    if (step <= 0)
	step = DEFAULT_STEP;

    char* tmp = NULL;
    double* time_delta = NULL;
	    
    if (fname == NULL || pref == NULL){
	printf("You must include the parameters \"outfile\" and \"stream_ext\" in"\
	       " your parameter file.\n");
	exit(1);
    }

    char* infname = malloc(strlen(fname) + strlen(pref) + strlen(".dat") + 5);
    // output individual estimates as well.
    char* stream_out = malloc(strlen(fname) + strlen(pref) + strlen("_est.dat") + 5);
    printf("running estimator %s for %d streams\n", estimator_type, nstreams);

    est_arr** allstreams = malloc(nstreams * sizeof(est_arr*));

    int i;
    for (i = 0; i < nstreams; ++i) {
	sprintf(infname, "%s%s%d_ev", fname, pref, i);
	sprintf(stream_out, "%s%s%d_est.dat", fname, pref, i);
	allstreams[i] = estimate(paramfile, infname, stream_out, estimator_type);
    }
	    
    /* Find time delay here*/
    if ((tmp = get_string_param(params, "timedelta")) != NULL){
	string_arr* vals = string_split(tmp, ',');
	int tdlen = vals->len;
	time_delta = malloc((tdlen + 1) * sizeof(double));
	
	for (i = 1; i < tdlen + 1; ++i) {
	    time_delta[i - 1] = atof(vals->data[i]);
	}

	free_string_arr(vals);
    } else {
	printf("You must specify the time delay between each stream. "\
	       "Add something like \"timedelta 0,10,20\" to your parameter file\n");
	exit(1);
    }
    
    double interval_time = 0;

    if ((interval_time = get_double_param(params, "interval_time")) == 0){
	printf("interval_time not specified.\n");
	exit(1);
    }

    double_multi_arr* combined = combine_functions(allstreams, time_delta, interval_time, nstreams, step);
    output_double_multi_arr(outfile, "w", combined);

    free_list(params);
    free_double_multi_arr(combined);
    free(infname);
    free(stream_out);
}

launcher_args* make_arg_struct()
{
    launcher_args* a = malloc(sizeof(launcher_args));
    
    a->est = 0;
    a->exp = 0;
    a->gauss = 0;
    a->gen = 0;
    a->nstreams = 1;
    a->writing = 0;
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
