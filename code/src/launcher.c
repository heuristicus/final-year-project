#include "generator.h"
#include "estimator.h"

#define PROG_DESC "This program simulates photon arrival times using a poisson process."
#define OPT_INFO "OPTIONS\n"						\
    "\t -e or --estimate\n"						\
    "\t\t Estimate the underlying function from a given set of photon stream data.\n\n"	\
    "\t -g or --generate\n"						\
    "\t\t Generate a photon stream.\n\n"				\
    "\t -h or --help\n"							\
    "\t\t Display this meessage.\n\n"					\
    "\t -i or --infile\n"						\
    "\t\t Specify the file to use as input to the estimator.\n\n"	\
    "\t -n or --numruns\n"						\
    "\t\t Number of times to run generation. Use this to generate multiple streams.\n\n" \
    "\t -o or --outfile\n"						\
    "\t\t Data will be output to this file.\n\n"			\
    "\t -p or --paramfile\n"						\
    "\t\t The file containing parameters to use. This can be used to specify a large number of options.\n\n" \
    "\t -x or --experiment\n"						\
    "\t\t Run an experiment.\n\n"					\

int main(int argc, char *argv[])
{
    struct option opts[] =
	{
	    {"experiment", required_argument, 0, 'x'},
	    {"generate", required_argument, 0, 'g'},
	    {"estimate", required_argument, 0, 'e'},
	    {"paramfile",  required_argument, 0, 'p'},
	    {"outfile",  required_argument, 0, 'o'},
	    {"numruns",    required_argument, 0, 'n'},
	    {"help", no_argument, 0, 'h'},
	    {0, 0, 0, 0}
	};

    int c;
    int opt_ind;
    
    int exp = 0;
    int gen = 0;
    int est = 0;
    int nruns = 1;
    char* paramfile = NULL;
    char* outfile = NULL;
    char* infile = NULL;

    if (argc == 1){
	printf("%s\n\nusage: %s options\n\n%s\n", PROG_DESC, argv[0], OPT_INFO);
	exit(1);
    }
        
    while((c = getopt_long(argc, argv, "o:p:n:heg:x:i", opts, &opt_ind)) != -1){
    	switch(c){
    	case 'e':
    	    // Need to specify which estimator to use and the input file - put all of this in the param file
    	    est = 1;
    	    if (exp + gen + est > 1){
    		printf("Choose only one of -e, -g or -x. You can run either an estimator, a generator or experiments,"\
    		       " but not more than one at once.\n");
    		exit(1);
    	    }
    	    paramfile = optarg;
    	    break;
    	case 'g':
    	    gen = 1;
    	    if (exp + gen + est > 1){
    		printf("Choose only one of -e, -g or -x. You can run either an estimator, a generator or experiments,"\
    		       " but not more than one at once.\n");
    		exit(1);
    	    }
    	    paramfile = optarg;
    	    break;
    	case 'h':
    	    printf("%s\n\nusage: %s options\n\n%s\n", PROG_DESC, argv[0], OPT_INFO);
    	    exit(1);
    	case 'i':
    	    infile = optarg;
    	    break;
    	case 'n':
    	    nruns = atoi(optarg);
    	    break;
    	case 'o':
    	    outfile = optarg;
    	    break;
    	case 'p':
    	    paramfile = optarg;
    	    break;
    	case 'x':
    	    exp = 1;
    	    if (exp + gen + est > 1){
    		printf("Choose only one of -e, -g or -x. You can run either an estimator, a generator or experiments," \
    		       "but not more than one at once.\n");
    		exit(1);
    	    }
    	    paramfile = optarg;
    	    break;
    	default:
    	    printf("%s\n\nusage: %s options\n\n%s\n", PROG_DESC, argv[0], OPT_INFO);
    	    exit(1);
    	}
    }

    printf("numruns %d, exp %d, gen %d, est %d, paramfile %s, outfile %s\n", nruns, exp, gen, est, paramfile, outfile);

    if (gen == 1){
	printf("generating.\n");
	generate(paramfile, outfile, nruns);
    } else if (est == 1){
	if (infile == NULL && paramfile == NULL){
	    printf("You must specify an input file or a parameter file to use.\n");
	    exit(1);
	}
	printf("estimating\n");
//	estimate();
    } else if (exp == 1){
	printf("experimenting\n");
    } else {
	printf("No action specified. You can run either an estimator, a generator or experiments by using "\
	       "the -e, -g or -x switches respectively.\n");
    }

    return 0;
}
