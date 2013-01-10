#include "estimator.h"
#include "paramlist.h"
#include "file_util.h"
#include "general_util.h"

void estimate(char* paramfile, char* infile, char* outfile, char* estimator_type);
int has_required_params(paramlist* params, char** required_params, int len);
void run_ols(paramlist* params, char* infile, char* outfile);
void run_iwls(paramlist* params, char* infile, char* outfile);
void run_pc(paramlist* params, char* infile, char* outfile);
void run_base(paramlist* params, char* infile, char* outfile);

static char *ols_params[] = {"start_time", "interval_time", "ols_subintervals"};
static char *iwls_params[] = {"iwls_iterations", "start_time", 
			      "interval_time", "iwls_subintervals"};
static char *pc_params[] = {"start_time", "interval_time", "pc_iwls_iterations", 
			    "pc_iwls_subintervals", "pc_max_extension", "pc_max_breakpoints"};
static char *base_params[] = {"start_time", "interval_time", "base_iwls_iterations", 
			      "base_iwls_subintervals", "base_max_extension", "base_max_breakpoints"};

/*
 * Runs the specified estimator using the provided parameter and output files. Performs
 * checks on the required parameters and does not run if they are not specified in the parameter file.
 */
void estimate(char* paramfile, char* infile, char* outfile, char* estimator_type)
{
    paramlist* params = get_parameters(paramfile);
    check_in_out_files(params, infile, outfile);    
    
    if (strcmp("ols", estimator_type) == 0){
	run_ols(params, infile, outfile);
    } else if (strcmp("iwls", estimator_type) == 0){
	run_iwls(params, infile, outfile);
    } else if (strcmp("pc", estimator_type) == 0){
	run_pc(params, infile, outfile);
    } else if (strcmp("base", estimator_type) == 0){
	run_base(params, infile, outfile);
    } else {
	printf(EST_TYPE_ERROR, estimator_type);
    }
    
}

/*
 * Attempts to run OLS estimator after extracting parameters from the given parameter file.
 */
void run_ols(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, ols_params, sizeof(ols_params)/sizeof(char*))){
	int subint = atoi(get_param_val(params, "ols_subintervals"));
	double start = atof(get_param_val(params, "start_time"));
	double end = atof(get_param_val(params, "interval_time")) + start;
	
	estimate_OLS(infile, outfile, start, end, subint);
    } else {
	print_string_array("Some parameters required for OLS estimates are missing. " \
			   "Ensure that your parameter file contains the following entries and try again.",
			   ols_params, sizeof(ols_params)/sizeof(char*));
	exit(1);
    }
}

/*
 * Attempts to run IWLS estimator after extracting parameters from the given parameter file.
 */
void run_iwls(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, iwls_params, sizeof(iwls_params)/sizeof(char*))){
	int subint = atoi(get_param_val(params, "iwls_subintervals"));
	int iterations = atoi(get_param_val(params, "iwls_iterations"));
	double start = atof(get_param_val(params, "start_time"));
	double end = atof(get_param_val(params, "interval_time")) + start;

	estimate_IWLS(infile, outfile, start, end, subint, iterations);
    } else {
	print_string_array("Some parameters required for IWLS estimates are missing. "\
			   "Ensure that your parameter file contains the following entries and try again.",
			   iwls_params, sizeof(iwls_params)/sizeof(char*));
	exit(1);
    }

}

/*
 * Attempts to run piecewise estimator after extracting parameters from the given parameter file.
 */
void run_pc(paramlist* params, char* infile, char* outfile)
{
	if (has_required_params(params, pc_params, sizeof(pc_params)/sizeof(char*))){
	    printf("pc params ok\n");
	} else {
	    print_string_array("Some parameters required for piecewise estimates are missing. "\
			       "Ensure that your parameter file contains the following entries and try again.",
			       pc_params, sizeof(pc_params)/sizeof(char*));
	    exit(1);
	}
}

/*
 * Attempts to run baseline estimator after extracting parameters from the given parameter file.
 */
void run_base(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, base_params, sizeof(base_params)/sizeof(char*))){
	printf("base params ok\n");
    } else {
	print_string_array("Some parameters required for baseline estimates are missing. "\
			   "Ensure that your parameter file contains the following entries and try again.", 
			   base_params, sizeof(base_params)/sizeof(char*));
	exit(1);
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

/*
 * Checks that input and output files are specified either in the provided parameters
 * or in the parameter list. Exits if unspecified.
 */
void check_in_out_files(paramlist* params, char* infile, char* outfile)
{
    if (infile == NULL){
	infile = get_param_val(params, "infile");
    }
    if (outfile == NULL){
	outfile = get_param_val(params, "est_outfile");
    }

    if (infile == NULL || outfile == NULL){
	printf("You must specify input and output files to use. Add \"infile\" and "\
	       "\"est_outfile\" to your parameter file.\n");
	exit(1);
    }
}
