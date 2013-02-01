#include <launcher.h>
#include "combinefunction.h"
#include "paramlist.h"
#include "file_util.h"
#include "math_util.h"

 
static char *estimators[] = {"iwls", "ols", "pc", "base", "gauss"};
static struct option opts[] =
    {
	{"experiment", required_argument, 0, 'x'},
	{"generate", required_argument, 0, 'g'},
	{"estimate", required_argument, 0, 'e'},
	{"estimator", required_argument, 0, 'a'},
	{"infile",  required_argument, 0, 'i'},
	{"outfile",  required_argument, 0, 'o'},
	{"defparam", required_argument, 0, 'd'},
	{"nstreams",    required_argument, 0, 'n'},
	{"randfunc",    required_argument, 0, 'r'},
	{"estall", no_argument, 0, 'l'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
    };

int main(int argc, char *argv[])
{
    int c;
    int opt_ind;
    
    launcher_args* args = malloc(sizeof(launcher_args));
    args->nstreams = 1;
    char* paramfile = NULL;
    char* outfile = NULL;
    char* infile = NULL;
    char* estimator_type = NULL;
    

    if (argc == 1){
	printf("%s\n\nusage: %s options\n\n%s\n%s\n%s\n", PROG_DESC, argv[0], OPT_INFO, VERSION, BUGREPORT);
	exit(1);
    }
        
    while((c = getopt_long(argc, argv, "x:g:e:a:i:o:d:n:hls:r:", opts, &opt_ind)) != -1){
    	switch(c){
    	case 'e':
    	    // Need to specify which estimator to use and the input file - put all of this in the param file
    	    args->est = 1;
    	    paramfile = strdup(optarg);
    	    break;
	case 'a':
	    if (!estimator_valid(optarg)){
		printf(EST_TYPE_ERROR, optarg);
		exit(1);
	    } else {
		printf("Estimation will be performed using estimator %s\n", optarg);
		estimator_type = strdup(optarg);
	    }
	    break;
	case 'd':
	    create_default_param_file(optarg);
	    exit(1);
	    break;
    	case 'g':
    	    args->gen = 1;
    	    paramfile = strdup(optarg);
    	    break;
    	case 'h':
    	    printf("%s\n\n\n%s\n%s\n%s\n", PROG_DESC, OPT_INFO, VERSION, BUGREPORT);
    	    exit(1);
    	case 'i':
    	    infile = strdup(optarg);
    	    break;
	case 'l':
	    args->estall = 1;
	    break;
    	case 'n':
    	    args->nstreams = atoi(optarg);
    	    break;
    	case 'o':
    	    outfile = strdup(optarg);
    	    break;
	case 'r':
	    paramfile = strdup(optarg);
	    args->rfunc = 1;
    	    break;
	case 's':
	    paramfile = strdup(optarg);
	    args->gauss = 1;
	    break;
    	case 'x':
    	    args->exp = 1;
    	    paramfile = strdup(optarg);
    	    break;
    	default:
    	    printf("%s\n\nusage: %s options\n\n%s\n", PROG_DESC, argv[0], OPT_INFO);
    	    exit(1);
    	}

	if (args->exp + args->gen + args->est + args->gauss + args->rfunc > 1){
    		printf("Choose only one of -e, -g, -x or -s. You can run either"\
		       " an estimator, or experiments, or generate gaussians or"\
		       " poisson streams, but not more than one at once.\n");
    		exit(1);
	}
    }

    run_requested_operations(args, paramfile, infile, outfile, estimator_type);

    free(estimator_type);
    free(paramfile);
    free(infile);
    free(outfile);
    free(args);

    return 0;
}

void run_requested_operations(launcher_args* args, char* paramfile, char* infile, 
			      char* outfile, char* estimator_type)
{
    if (args->gen == 1){
	if (paramfile == NULL){
	    printf("You must specify a parameter file to use.\nTry running "\
		   "\"launcher -g [your parameter file]\"\n");
	    exit(1);
	}
	generate(paramfile, outfile, args->nstreams);
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
	if (args->estall && args->nstreams > 1){
	    multi_estimate(paramfile, infile, outfile, estimator_type, args->nstreams);
	} else {
	    printf("estimating single stream\n");
	    estimate(paramfile, infile, outfile, estimator_type);
	}
    } else if (args->exp == 1){
	printf("experimenting\n");
    } else if (args->gauss == 1){
	printf("generating gaussians\n");    
	generate_gaussians(paramfile, outfile, infile);
    } else if (args->rfunc == 1){
	generate_random_function(paramfile, outfile, args->nstreams);
    } else {
	printf("No action specified. You can run either an estimator, a generator or experiments by using "\
	       "the -e, -g or -x switches respectively.\n");
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
    char* fname = get_string_param(params, "outfile");
    char* pref = get_string_param(params, "stream_ext");
//    char* tmp = NULL;
//    double* time_delta = NULL;
	    
    if (fname == NULL || pref == NULL){
	printf("You must include the parameters \"outfile\" and \"stream_ext\" in"\
	       " your parameter file.\n");
	exit(1);
    }

    printf("running estimator %s for %d streams\n", "gauss", nstreams);
    char* infname = malloc(strlen(fname) + strlen(pref) + 3);
    char* outname = malloc(10);
    int i;
    
    for (i = 0; i < nstreams; ++i) {
	sprintf(infname, "%s%s%d_ev", fname, pref, i);
	sprintf(outname, "%s%d", "gaussout", i);
	estimate_gaussian(params, infname, outname);
    }
}

void multi_est_default(char* paramfile, char* infile, char* outfile, char* estimator_type, int nstreams)
{
    paramlist* params = get_parameters(paramfile);
    char* fname = get_string_param(params, "outfile");
    char* pref = get_string_param(params, "stream_ext");
    char* tmp = NULL;
    double* time_delta = NULL;
	    
    if (fname == NULL || pref == NULL){
	printf("You must include the parameters \"outfile\" and \"stream_ext\" in"\
	       " your parameter file.\n");
	exit(1);
    }

    char* infname = malloc(strlen(fname) + strlen(pref) + 3);
    printf("running estimator %s for %d streams\n", estimator_type, nstreams);

    est_arr** allstreams = malloc(nstreams * sizeof(est_arr*));
    int i;
    for (i = 0; i < nstreams; ++i) {
	sprintf(infname, "%s%s%d_ev", fname, pref, i);
	allstreams[i] = estimate(paramfile, infname, outfile, estimator_type);
    }
	    
    /* Find time delay here*/
    if ((tmp = get_string_param(params, "timedelta")) != NULL){
	char **vals = string_split(tmp, ',');
	int tdlen = atoi(vals[0]) - 1;
	time_delta = malloc((tdlen + 1) * sizeof(double));
	
	for (i = 1; i < tdlen + 1; ++i) {
	    time_delta[i - 1] = atof(vals[i]);
	}

	free_pointer_arr((void**) vals, atoi(vals[0]));
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

    est_arr* combined = combine_function(allstreams, time_delta, interval_time, nstreams);
    output_estimates(outfile, combined->estimates, combined->len);
    free_est_arr(combined);
    free_list(params);
}

/*
 * Checks whether an estimator name is valid.
 */
int estimator_valid(char* name)
{
    if (name == NULL)
	return 0;
    
    int i;
    
    for (i = 0; i < sizeof(estimators)/sizeof(char*); ++i) {
	if (strcmp(name, estimators[i]) == 0)
	    return 1;
    }

    return 0;
}
