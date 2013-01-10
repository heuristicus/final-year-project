#include "estimator.h"
#include "paramlist.h"
#include "file_util.h"

void estimate(char* paramfile, char* infile, char* outfile, char estimator_type);
int has_required_params(paramlist* params, char** required_params, int len);

static char *iwls_params[] = {"iwls_iterations", "start_time", "interval_time", "iwls_subintervals"};

/*
 * Runs the specified estimator using the provided parameter and output files. Performs
 * checks on the required parameters and does not run if they are not specified in the parameter file.
 */
void estimate(char* paramfile, char* infile, char* outfile, char estimator_type)
{
    paramlist* params = get_parameters(paramfile);
    
    switch (estimator_type){
    case 'o': // OLS
	printf("OLS estimate\n");
	break;
    case 'i': // IWLS
	printf("IWLS estimate\n");
	if (has_required_params(params, iwls_params, sizeof(iwls_params)/sizeof(char*))){
	    int subint = atoi(get_param_val(params, "iwls_subintervals"));
	    int iterations = atoi(get_param_val(params, "iwls_iterations"));
	    double start = atof(get_param_val(params, "start_time"));
	    double end = atof(get_param_val(params, "interval_time")) + start;
	    
	    if (infile == NULL){
		infile = get_param_val(params, "infile");
	    }
	    if (outfile == NULL){
		outfile = get_param_val(params, "est_outfile");
	    }

	    if(infile == NULL || outfile == NULL){
		printf("You must specify input and output files to use. Add \"infile\" and \"est_outfile\" to your parameter file.\n");
		exit(1);
	    }
	    
	    estimate_IWLS(infile, outfile, start, end, subint, iterations);
	    
	} else {
	    printf("Some parameters required for IWLS estimate are missing. Please add them to the parameter file.\n");
	    exit(1);
	}
	break;
    case 'p': // Piecewise
	printf("Piecewise estimate\n");
	break;
    case 'b': // Baseline
	printf("Baseline estimate\n");
	break;
    }
}

/*
 * Checks that the paramlist provided contains parameters with names corresponding to the
 * strings provided in the required_params array.
 */
int has_required_params(paramlist* params, char** required_params, int len)
{
    int i;
    int ok = 1;
    
    for (i = 0; i < len; ++i) {
	if (get_param(params, required_params[i]) == NULL){
	    printf("Missing parameter: %s\n", required_params[i]);
	    ok = 0;
	}
    }

    return ok;
}
