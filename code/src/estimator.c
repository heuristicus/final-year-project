#include "estimator.h"
#include "defparams.h"
#include "paramlist.h"
#include "general_util.h"
#include "file_util.h"
#include "general_util.h"
#include "combinefunction.h"

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
void* estimate(char* paramfile, char* infile, char* outfile, char* estimator_type, int output_switch)
{
    paramlist* params = get_parameters(paramfile);
    void* result = _estimate(params, infile, outfile, estimator_type, output_switch);
    free_list(params);
    return result;
}

void* _estimate(paramlist* params, char* infile, char* outfile, char* estimator_type, int output_switch)
{
    void* result = NULL;
    int alloced = 0;

    if (infile == NULL){
	char* fname = get_string_param(params, "outfile"); // default generator output filename
	char* pref = get_string_param(params, "stream_ext"); // default extension
	char* function_fname = get_string_param(params, "function_outfile");    
	infile = malloc(strlen(function_fname) + strlen(fname) + strlen(pref) + 10);
	sprintf(infile, "%s_0_%s%s0.dat", function_fname, fname, pref);
	alloced = 1;
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
	result = run_gauss(params, infile, outfile, output_switch);
    } else {
	printf(EST_TYPE_ERROR, estimator_type);
    }
    
    if (alloced)
	free(infile);

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

gauss_vector* run_gauss(paramlist* params, char* infile, char* outfile,
			    int output_switch)
{
    if (has_required_params(params, gauss_params, sizeof(gauss_params)/sizeof(char*))){
	return estimate_gaussian_raw(params, infile, outfile, output_switch);
    } else {
	print_string_array("Some parameters required for gaussian estimates are missing. " \
			   "Ensure that your parameter file contains the following entries and try again.",
			   gauss_params, sizeof(gauss_params)/sizeof(char*));
	exit(1);
    }
}

tdelta_result** multi_estimate(char* paramfile, char* in_dir, char* outfile,
			       int nstreams, int nfuncs, int output_switch,
			       char* estimator_type, int stuttered)
{
    paramlist* params = get_parameters(paramfile);
    
    tdelta_result** ret =  _multi_estimate(params, in_dir, outfile, nstreams, nfuncs,
					   output_switch, estimator_type, stuttered);
    free_list(params);
    return ret;
}

/*
 * Estimate a series of streams. Constructs the file to read data from by using
 * parameters used to output the data from the generator, and then runs estimators
 * on each file. Data is then stored and once all estimates have been made the data
 * is combined to make a single estimate. The i
 */
tdelta_result** _multi_estimate(paramlist* params, char* in_dir, char* outfile,
				int nstreams, int nfuncs, int output_switch,
				char* estimator_type, int stuttered)
{
    char* fname = get_string_param(params, "outfile"); // default generator output filename
    char* pref = get_string_param(params, "stream_ext"); // default extension
    double step = get_double_param(params, "output_step");
    char* est_delta = get_string_param(params, "estimate_delta");
    double start = get_double_param(params, "est_start_time");
    int gauss = strcmp(estimator_type, "gauss") == 0;
    char* function_fname = get_string_param(params, "function_outfile");

    if (step <= 0)
	step = DEFAULT_STEP;

    if (outfile == NULL && output_switch != 0){
	if (gauss)
	    outfile = get_string_param(params, "gauss_est_outfile");
	else
	    outfile = get_string_param(params, "est_outfile");
    }
	    
    if (fname == NULL || pref == NULL){
	printf("You must include the parameters \"outfile\" and \"stream_ext\" in"\
	       " your parameter file.\n");
	exit(1);
    } else if (outfile == NULL && output_switch != 0){
	if (gauss)
	    printf("You must include the parameter \"gauss_est_outfile\" in your" \
		   " parameter file, or specify the output file with the -o switch.\n");
	else
	    printf("You must include the parameter \"est_outfile\" in your" \
		   " parameter file,area or specify the output file with the -o switch.\n");
	exit(1);
    }
    
    char* infname;
    char* outname = malloc(strlen(outfile) + strlen("func") + 10);
    if (in_dir != NULL){
	infname = malloc(strlen(in_dir) + strlen(function_fname) 
			 + strlen(fname) + strlen(pref) + strlen(".dat") + 5);
    } else {
	infname = malloc(strlen(function_fname) + strlen(fname) 
			 + strlen(pref) + strlen(".dat") + 5);
    }

    int i;
    
    tdelta_result** results = malloc(sizeof(tdelta_result) * nfuncs);
    for (i = 0; i < nfuncs; ++i) {
	if (in_dir != NULL){
	    sprintf(infname, "%s/%s_%d_%s%s", in_dir, function_fname, i, fname, pref);
	} else {
	    sprintf(infname, "%s_%d_%s%s", function_fname, i, fname, pref);
	}
	sprintf(outname, "%s_func_%d", outfile, i);
		
	results[i] = do_multi_estimate(params, infname, outname, step, start,
				       est_delta, nstreams, output_switch,
				       estimator_type, stuttered);
    }
    free(outname);
    free(infname);
    
    return results;
}

tdelta_result* do_multi_estimate(paramlist* params, char* infile, char* outfile, double step,
		       double start, char* est_delta, int nstreams, int output_switch,
				 char* estimator_type, int stuttered)
{
    double_arr* delays = NULL;// Will be used to store time delays
    int gauss = strcmp(estimator_type, "gauss") == 0;
    printf("running estimator %s for %d streams\n", estimator_type, nstreams);

    char* infname = malloc(strlen(infile) + strlen(".dat") + strlen("_stuttered") + 5);
    char* outname = malloc(strlen(outfile) + strlen(".dat") + strlen("_combined") + 5);
    int i;

    void** estimates;
    
    if (gauss)
	estimates = malloc(sizeof(gauss_vector*) * nstreams);
    else
	estimates = malloc(sizeof(est_arr*) * nstreams);

    for (i = 0; i < nstreams; ++i) {
	if (stuttered)
	    sprintf(infname, "%s%d_stuttered.dat", infile, i);
	else
	    sprintf(infname, "%s%d.dat", infile, i);
	if (outfile != NULL && output_switch >= 1)
	    sprintf(outname, "%s_%d", outfile, i);
	
	// Since we are storing data in a void pointer, we do not need to do anything
	// to mess around using different functions, since the _estimate function returns
	// us a void pointer to the relevant est_arr or gauss_vector struct. We will later
	// cast these to their original types.
	estimates[i] = _estimate(params, infname, outname, estimator_type, output_switch);
    }

    double normaliser = 1;
    // The input file for the base function is used for all delay estimates, so
    // just read it once. We also need this to calculate the normalisation constant.
    sprintf(infname, "%s0.dat", infile);
    double_arr* base_stream_events = get_event_data_all(infname);
    if (gauss){
	// Find a normaliser to apply to the values of the estimated function.
	// In its raw form, the estimated function is not on the same scale as
	// the original if the gaussian estimator was used because of the way
	// that gaussians are summed.
	normaliser = find_normaliser(params, estimates[0], base_stream_events,
				     estimator_type);
    }

    if (strcmp(est_delta, "yes") == 0 && nstreams > 1){
	char* delta_method = get_string_param(params, "delta_est_method");
	char* hierarchical = get_string_param(params, "delta_est_hierarchical");
	delays = init_double_arr(nstreams);

	// The delay estimates take the first function as a base, so we assume that
	// it has no delay - the delays of the other functions will be calculated
	// relative to this.
	delays->data[0] = 0;

	for (i = 1; i < nstreams; ++i) {
	    printf("Estimating delta for stream 0 and stream %d\n", i);

	    // Read sets of events for the two streams from different files.
	    sprintf(infname, "%s%d.dat", infile, i);
	    if (outfile != NULL && output_switch >= 1)
		sprintf(outname, "%s_%d", outfile, i-1);

	    double_arr* f2_events = get_event_data_all(infname);

	    if (strcmp(delta_method, "area") == 0){
		delays->data[i] = estimate_delay_area(params, outname, estimates[0],
						      estimates[i], hierarchical,
						      estimator_type, output_switch);
	    } else if (strcmp(delta_method, "pmf") == 0){
		delays->data[i] = estimate_delay_pmf(params, outname, base_stream_events,
						     f2_events, estimates[0],
						     estimates[i], normaliser, 
						     hierarchical, estimator_type, output_switch);
	    }
	    
	    // This data is not reused, so free it
	    free_double_arr(f2_events);
	}

	for (i = 0; i < delays->len; ++i) {
	    printf("Delay for stream %d: %lf\n", i, delays->data[i]);
	}
	printf("normaliser is %lf\n", normaliser);
	free_double_arr(base_stream_events);
    } else {
	if ((delays = get_double_list_param(params, "timedelta")) == NULL){
	    printf("You must specify the time delay between each stream. "\
		   "Add something like \"timedelta 0,10,20\" to your parameter file\n");
	    exit(1);
	}
    }

    double interval_time = 0;

    if ((interval_time = get_double_param(params, "interval_time")) == 0){
	printf("interval_time not specified.\n");
	exit(1);
    }


    double_multi_arr* final_estimate = NULL;
    if (gauss) {
	final_estimate = combine_gauss_vectors((gauss_vector**)estimates,
					       delays, start, interval_time,
					       step, normaliser, nstreams);
    } else {
	final_estimate = combine_functions((est_arr**)estimates, delays,
					   start, interval_time, step, nstreams);
    }

    if (outfile != NULL && output_switch >= 1){
	sprintf(outname, "%s_combined.dat", outfile);
	output_double_multi_arr(outname, "w", final_estimate);
    }
    
    free(infname);
    free(outname);

    tdelta_result* result = malloc(sizeof(tdelta_result));
    result->final_estimate = final_estimate;
    result->delays = delays;
    result->intermediate_estimates = estimates;
    result->nstreams = nstreams;
    result->type = estimator_type;

    return result;
}
