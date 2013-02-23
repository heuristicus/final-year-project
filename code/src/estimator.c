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
est_arr* estimate(char* paramfile, char* infile, char* outfile, char* estimator_type)
{
    paramlist* params = get_parameters(paramfile);
    est_arr* result = _estimate(params, infile, outfile, estimator_type);
    return result;
}

est_arr* _estimate(paramlist* params, char* infile, char* outfile, char* estimator_type)
{
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

/*
 * Estimate a series of streams. Constructs the file to read data from by using
 * parameters used to output the data from the generator, and then runs estimators
 * on each file. Data is then stored and once all estimates have been made the data
 * is combined to make a single estimate.
 */
void multi_estimate(char* paramfile, char* infile, char* outfile, int nstreams, int output_switch, char* estimator_type)
{
    paramlist* params = get_parameters(paramfile);
    _multi_estimate(params, infile, outfile, nstreams, output_switch, estimator_type);
    free_list(params);
}

void _multi_estimate(paramlist* params, char* infile, char* outfile, int nstreams, int output_switch, char* estimator_type)
{
    char* fname = get_string_param(params, "outfile"); // default generator output filename
    char* pref = get_string_param(params, "stream_ext"); // default extension
    double step = get_double_param(params, "output_step");
    char* est_delta = get_string_param(params, "estimate_delta");
    double start = get_double_param(params, "est_start_time");
    double_arr* delays = NULL;// Will be used to store time delays
    int gauss = strcmp(estimator_type, "gauss") == 0;
    
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

    printf("running estimator %s for %d streams\n", estimator_type, nstreams);
    
    char* infname = malloc(strlen(fname) + strlen(pref) + strlen(".dat") + 5);
    char* outname = malloc(strlen(outfile) + strlen(".dat") + strlen("_combined") + 5);
    int i;

    void** estimates;
    
    if (gauss)
	estimates = malloc(sizeof(gauss_vector*) * nstreams);
    else
	estimates = malloc(sizeof(est_arr*) * nstreams);

    for (i = 0; i < nstreams; ++i) {
	sprintf(infname, "%s%s%d.dat", fname, pref, i);
	if (outfile != NULL && output_switch >= 1)
	    sprintf(outname, "%s_%d", outfile, i);
	
	if (gauss)
	    estimates[i] = estimate_gaussian_raw(params, infname, outname);
	else
	    estimates[i] = _estimate(params, infname, outname, estimator_type);
    }

    if (strcmp(est_delta, "yes") == 0){
	char* delta_method = get_string_param(params, "delta_est_method");
	char* hierarchical = get_string_param(params, "delta_est_hierarchical");
	delays = init_double_arr(nstreams);

	// The delay estimates take the first function as a base, so we assume that
	// it has no delay - the delays of the other functions will be calculated
	// relative to this.
	delays->data[0] = 0;

	// The input file for the base function is used for all delay estimates, so
	// just read it once.
	sprintf(infname, "%s%s0.dat", fname, pref);
	double_arr* base_stream_events = get_event_data_all(infname);
	double normaliser = 1;

	if (strcmp(delta_method, "pmf") == 0 && gauss){
	    // Find a normaliser to apply to the values of the estimated function.
	    // In its raw form, the estimated function is not on the same scale as
	    // the original if the gaussian estimator was used because of the way
	    // that gaussians are summed.
	    normaliser = find_normaliser(params, estimates[0], base_stream_events,
					 estimator_type);
	}

	for (i = 1; i < nstreams; ++i) {
	    printf("Estimating delta for stream 0 and stream %d\n", i);

	    // Read sets of events for the two streams from different files.
	    sprintf(infname, "%s%s%d.dat", fname, pref, i);
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
	final_estimate = combine_gauss_vectors((gauss_vector**)estimates, delays, start, interval_time, step, nstreams);
    } else {
	final_estimate = combine_functions((est_arr**)estimates, delays, start, interval_time, step, nstreams);
    }

    if (outfile != NULL && output_switch >= 1){
	sprintf(outname, "%s_combined.dat", outfile);
	output_double_multi_arr(outname, "w", final_estimate);
    }
    
    free(infname);
    free(outname);

    free_double_multi_arr(final_estimate);
    free_double_arr(delays);
    for (i = 0; i < nstreams; ++i) {
	if (gauss)
	    free_gauss_vector((gauss_vector*)estimates[i]);
	else
	    free_est_arr((est_arr*)estimates[i]);
    }
    free(estimates);
}
