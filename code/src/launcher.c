#include <launcher.h>
 
static char *estimators[] = {"iwls", "ols", "pc", "base"};
static struct option opts[] =
    {
	{"experiment", required_argument, 0, 'x'},
	{"generate", required_argument, 0, 'g'},
	{"estimate", required_argument, 0, 'e'},
	{"estimator", required_argument, 0, 'a'},
	{"infile",  required_argument, 0, 'i'},
	{"outfile",  required_argument, 0, 'o'},
	{"defparam", required_argument, 0, 'd'},
	{"numruns",    required_argument, 0, 'n'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
    };

int main(int argc, char *argv[])
{
    int c;
    int opt_ind;
    
    int exp = 0;
    int gen = 0;
    int est = 0;
    int nruns = 1;
    char* paramfile = NULL;
    char* outfile = NULL;
    char* infile = NULL;
    char* estimator_type = NULL;
    

    if (argc == 1){
	printf("%s\n\nusage: %s options\n\n%s\n%s\n%s\n", PROG_DESC, argv[0], OPT_INFO, VERSION, BUGREPORT);
	exit(1);
    }
        
    while((c = getopt_long(argc, argv, "x:g:e:a:i:o:d:n:h", opts, &opt_ind)) != -1){
    	switch(c){
    	case 'e':
    	    // Need to specify which estimator to use and the input file - put all of this in the param file
    	    est = 1;
    	    if (exp + gen + est > 1){
    		printf("Choose only one of -e, -g or -x. You can run either an estimator, a generator or experiments,"\
    		       " but not more than one at once.\n");
    		exit(1);
    	    }
	    printf("%s\n", optarg);
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
    	    gen = 1;
    	    if (exp + gen + est > 1){
    		printf("Choose only one of -e, -g or -x. You can run either an estimator, a generator or experiments,"\
    		       " but not more than one at once.\n");
    		exit(1);
    	    }
    	    paramfile = strdup(optarg);
    	    break;
    	case 'h':
    	    printf("%s\n\n\n%s\n%s\n%s\n", PROG_DESC, OPT_INFO, VERSION, BUGREPORT);
    	    exit(1);
    	case 'i':
    	    infile = strdup(optarg);
    	    break;
    	case 'n':
    	    nruns = atoi(optarg);
    	    break;
    	case 'o':
    	    outfile = strdup(optarg);
    	    break;
    	case 'x':
    	    exp = 1;
    	    if (exp + gen + est > 1){
    		printf("Choose only one of -e, -g or -x. You can run either an estimator, a generator or experiments," \
    		       "but not more than one at once.\n");
    		exit(1);
    	    }
    	    paramfile = strdup(optarg);
    	    break;
    	default:
    	    printf("%s\n\nusage: %s options\n\n%s\n", PROG_DESC, argv[0], OPT_INFO);
    	    exit(1);
    	}
    }

    

    //    printf("numruns %d, exp %d, gen %d, est %d, paramfile %s, outfile %s\n", nruns, exp, gen, est, paramfile, outfile);

    run_requested_operations(gen, est, exp, paramfile, infile, outfile, nruns, estimator_type);

    return 0;
}

void run_requested_operations(int generator, int estimator, int experiment, char* paramfile, char* infile, char* outfile, int nruns, char* estimator_type)
{
    if (generator == 1){
	generate(paramfile, outfile, nruns);
    } else if (estimator == 1){
	if (paramfile == NULL){
	    printf("You must specify a parameter file to use.\nTry running \"launcher -e [your parameter file] -i iwls\"\n");
	    exit(1);
	}
	estimate(paramfile, infile, outfile, estimator_type);
    } else if (experiment == 1){
	printf("experimenting\n");
    } else {
	printf("No action specified. You can run either an estimator, a generator or experiments by using "\
	       "the -e, -g or -x switches respectively.\n");
    }
}

/*
 * Checks whether an estimator name is valid.
 */
int estimator_valid(char* name)
{
    int i;
    
    for (i = 0; i < sizeof(estimators)/sizeof(char*); ++i) {
	if (strcmp(name, estimators[i]) == 0)
	    return 1;
    }

    return 0;
}
