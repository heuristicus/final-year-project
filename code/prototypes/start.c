#include "poisson.h"

char *valid_switches[3] = {"-o", "-p", "-n"};

int main(int argc, char *argv[])
{
    /* char *outfile; */
    /* int freeflag = 0; // free outfile or not */

    char **args = parse_args(argc, argv);
    
    free(args);
    
    return 0;
}

char** parse_args(int argc, char *argv[])
{
    int i;
    char **switches = malloc(argc/2 * sizeof(char*));
    char **args = malloc(argc/2 * sizeof(char*));
        
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
	printf("usage: %s [-o output_file] [-p param_file] [-n number_of_streams]\n", argv[0]);
    }
    
    for (i = 1; i < argc; ++i){
	if (i % 2 == 0){ // check args
	    handle_arg(switches[i / 2 - 1], args, argv[i]); // mess around with array locs to get the last switch
	} else { // check switches
	    if (!valid(argv[i])){
		printf("%s: invalid option %s.\nTry `%s --help' for more information.\n", argv[0], argv[i], argv[0]);
		exit(1);
	    }
	    switches[i / 2] = argv[i]; // switches are every second argument
	}
    }

    free(switches);
        
    return args;
    
}

// Checks the validity of a switch
int valid(char* sw)
{
    int i;
    
    for (i = 0; i < sizeof(valid_switches)/sizeof(char*); ++i){
	if (strcmp(sw, valid_switches[i]) == 0)
	    return 1;
    }
    return 0;
}

/*
 * Put each argument in its prespecified location in the argument array. This location
 * corresponds to the location of the argument in the predefined valid_switches array.
 */
void handle_arg(char *sw, char **args, char *arg)
{
    int i;

    for (i = 0; strcmp(sw, valid_switches[i]) != 0; ++i);
    
    args[i] = arg;
        
}
