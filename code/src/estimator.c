#include "estimator.h"
#include "paramlist.h"
#include "general_util.h"
#include "file_util.h"
#include "general_util.h"

static char* ols_params[] = {"est_start_time", "est_interval_time", "ols_subintervals"};
static char* iwls_params[] = {"iwls_iterations", "est_start_time", 
			      "est_interval_time", "iwls_subintervals"};
static char* pc_params[] = {"est_start_time", "est_interval_time", "pc_iwls_iterations", 
			    "pc_iwls_subintervals", "pc_max_extension", "pc_max_breakpoints"};
static char* base_params[] = {"est_start_time", "est_interval_time", "base_iwls_iterations", 
			      "base_iwls_subintervals", "base_max_extension", "base_max_breakpoints"};
static char *gauss_params[] = {"est_start_time", "est_interval_time", "gauss_stdev", "gauss_resolution"};

/*
 * Runs the specified estimator using the provided parameter and output files. Performs
 * checks on the required parameters and does not run if they are not specified in the parameter file.
 */
est_arr* estimate(char* paramfile, char* infile, char* outfile, char* estimator_type)
{
    paramlist* params = get_parameters(paramfile);
    est_arr* result = NULL;

    if (infile == NULL){
	infile = get_string_param(params, "infile");
    }
    if (outfile == NULL){
	if (strcmp(estimator_type, "gauss") == 0)
	    outfile = get_string_param(params, "gauss_est_outfile");
	else 
	    outfile = get_string_param(params, "est_outfile");
    }

    if (infile == NULL || outfile == NULL){
	printf("You must specify input and output files to use. Add \"infile\" and "\
	       "\"est_outfile\" to your parameter file.\n");
	exit(1);
    }
    printf("outputting to %s\n", outfile);
    printf("infile is %s\n", infile);
    if (strcmp("ols", estimator_type) == 0){
	result = run_ols(params, infile, outfile);
    } else if (strcmp("iwls", estimator_type) == 0){
	result = run_iwls(params, infile, outfile);
    } else if (strcmp("pc", estimator_type) == 0){
	result = run_pc(params, infile, outfile);
    } else if (strcmp("base", estimator_type) == 0){
	result = run_base(params, infile, outfile);
    } else if (strcmp("gauss", estimator_type) == 0){
	run_gauss(params, infile, outfile);
    } else {
	printf(EST_TYPE_ERROR, estimator_type);
    }
    free_list(params);
    
    return result;
}

/*
 * Attempts to run OLS estimator after extracting parameters from the given parameter file.
 */
est_arr* run_ols(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, ols_params, sizeof(ols_params)/sizeof(char*))){
	return estimate_OLS(params, infile, outfile);
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
est_arr* run_iwls(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, iwls_params, sizeof(iwls_params)/sizeof(char*))){
	return estimate_IWLS(params, infile, outfile);
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
est_arr* run_pc(paramlist* params, char* infile, char* outfile)
{
	if (has_required_params(params, pc_params, sizeof(pc_params)/sizeof(char*))){
	    return estimate_piecewise(params, infile, outfile);
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
est_arr* run_base(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, base_params, sizeof(base_params)/sizeof(char*))){
	return estimate_baseline(params, infile, outfile);
    } else {
	print_string_array("Some parameters required for baseline estimates are missing. "\
			   "Ensure that your parameter file contains the following entries and try again.", 
			   base_params, sizeof(base_params)/sizeof(char*));
	exit(1);
    }
}

double_multi_arr* run_gauss(paramlist* params, char* infile, char* outfile)
{
    if (has_required_params(params, gauss_params, sizeof(gauss_params)/sizeof(char*))){
	return estimate_gaussian(params, infile, outfile);
    } else {
	print_string_array("Some parameters required for gaussian estimates are missing. " \
			   "Ensure that your parameter file contains the following entries and try again.",
			   gauss_params, sizeof(gauss_params)/sizeof(char*));
	exit(1);
    }
}
