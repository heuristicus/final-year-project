#include "estimator.h"
#include "paramlist.h"
#include "file_util.h"

void estimate(char* paramfile, char* infile, char* outfile, char estimator_type);
int has_required_params(paramlist* params, char** required_params, int len);

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
	//	static char[][] iwls_params = {"iwls_iterations", "start_time", "interval_time", "iwls_subintervals"};
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
    
    for (i = 0; i < len; ++i) {
	if (get_param(params, required_params[i]) == NULL)
	    return 0;
    }

    return 1;
}
