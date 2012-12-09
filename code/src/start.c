#include "generator.h"

static char *prog_description = 
{
"This program simulates photon arrival times using a poisson process."
};

static char *options_info = 
{
"OPTIONS\n"
"\t -p or --paramfile\n"
"\t\t The file containing parameters to use for generation. This can be used to specify a large number of options.\n\n"
"\t -o or --outfile\n"
"\t\t Data will be output to this file.\n\n"
"\t -n or --numruns\n"
"\t\t Number of times to run generation. Use this to generate multiple streams."
};

char *valid_switches[3] = {"-o", "-p", "-n"};

int main(int argc, char *argv[])
{
    char **args = parse_args(argc, argv);
    generate(args);
    
    free(args);
    
    return 0;
}

/*
 * o is the output file, p is the parameter file, n is the number of times
 * to run the generation process. 
 */
char** parse_args(int argc, char *argv[])
{
    static struct option opts[] =
	{
	    {"paramfile",  required_argument, 0, 'p'},
	    {"outfile",  required_argument, 0, 'o'},
	    {"numruns",    required_argument, 0, 'n'},
	    {0, 0, 0, 0}
	};

    int c;
    int opt_ind;

    // Arguments may not be received every time so use calloc to prevent memory issues.
    char **args = calloc(3, sizeof(char*));
        
    while((c = getopt_long(argc, argv, "o:p:n:", opts, &opt_ind)) != -1){
	switch(c){
	case 'o':
	    args[0] = optarg;
	    break;
	case 'p':
	    args[1] = optarg;
	    break;
	case 'n':
	    args[2] = optarg;
	    break;
	default:
	    printf("%s\n\nusage: %s options\n\n%s\n", prog_description, argv[0], options_info);
	    free(args);
	    exit(1);
	}
    }
    
    return args;
}
