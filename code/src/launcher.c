#include "launcher.h"
 
static char *estimators[] = {"iwls", "ols", "pc", "base", "gauss"};
static char *generators[] = {"mup", "rand"};
static struct option opts[] =
    {
	{"stutter",    required_argument, 0, 's'},
	{"experiment", required_argument, 0, 'x'},
	{"generate", required_argument, 0, 'g'},
	{"estimate", required_argument, 0, 'e'},
	{"estimator", required_argument, 0, 'a'},
	{"infile",  required_argument, 0, 'i'},
	{"outfile",  required_argument, 0, 'o'},
	{"paramfile",  required_argument, 0, 'p'},
	{"defparam", required_argument, 0, 'd'},
	{"nstreams",    required_argument, 0, 'n'},
	{"count",    required_argument, 0, 'c'},
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
    int createparam = 0;

    if (argc == 1){
	printf("%s\n\nusage: %s options\n\n%s\n%s\n%s\n", PROG_DESC, argv[0], OPT_INFO, VERSION, BUGREPORT);
	exit(1);
    }
        
    while((c = getopt_long(argc, argv, "x:g:e:a:i:o:d:n:f:hrt:rp:c:s", opts, &opt_ind)) != -1){
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
	    paramfile = strdup(optarg);
	    createparam = 1;
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
    	case 'n': // specify number of streams
    	    args->nstreams = atoi(optarg);
    	    break;
	case 'c': // specify number of functions to estimate or generate from
	    args->nfuncs = atoi(optarg);
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
	case 's':
	    args->stutter = 1;
	    break;
    	case 'x':
    	    args->exp = 1;
	    if (createparam)
		break;
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

    if (createparam){
	if (args->exp == 1)
	    create_default_param_file(paramfile, "experiment");
	else
	    create_default_param_file(paramfile, "default");
	exit(1);
	free(paramfile);
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
	    generate_gaussian_data(paramfile, infile, outfile, args->nfuncs, args->writing);
	} else if (generator_type == NULL || strcmp(generator_type, "mup") == 0){
	    printf("Generating event stream with expression from parameter file.\n");
	    generate(paramfile, outfile, args->nstreams, args->writing);
	} else if (strcmp(generator_type, "rand") == 0){
	    printf("Generating event stream with random functions.\n");
	    generate_from_gaussian(paramfile, outfile, infile, args->nstreams, args->nfuncs);
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
	    multi_estimate(paramfile, infile, outfile, args->nstreams, args->nfuncs, args->writing, estimator_type);
	} else {
	    printf("Estimating single stream.\n");
	    // mess with outfile here to output it nicely.
	    est_arr* result = estimate(paramfile, infile, outfile, estimator_type, args->writing);
	    if (result != NULL)
		free_est_arr(result);
	}
    } else if (args->exp == 1){
	if (extra_paramfile == NULL){
	    printf("You need to specify the file from which to read default parameters."\
		   " Use the -p switch to do so.\n");
	    exit(1);
	}
	if (args->stutter == 1){
	    stutter_stream(infile, paramfile, extra_paramfile, args->nfuncs, args->nstreams);
	} else {
	    run_experiments(paramfile, extra_paramfile, infile, outfile, args->nstreams, args->nfuncs, args->writing);
	}
    } else {
	printf("No action specified. You can run either an estimator, a generator or"\
	       " experiments by using the -e, -g or -x switches respectively.\n");
    }
}



launcher_args* make_arg_struct()
{
    launcher_args* a = malloc(sizeof(launcher_args));
    
    a->est = 0;
    a->exp = 0;
    a->gauss = 0;
    a->gen = 0;
    a->nstreams = 1;
    a->nfuncs = 1;
    a->writing = 1;
    a->rfunc = 0;
    a->stutter = 0;

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

