#include "general_util.h"
#include "defparams.h"
#include <stdlib.h>
#include <string.h>

#define DEF_ARR_SIZE 5

/*
 * Frees a pointer array
 */
void free_pointer_arr(void **arr, int length)
{
    int i;
    
    for (i = 0; i < length; ++i){
	free(arr[i]);
    }
    free(arr);
}

void free_string_arr(string_arr* arr)
{
    free_pointer_arr((void**) arr->data, arr->len);
}

/*
 * Splits a string on a separator.
 */
string_arr* string_split(char* string, char separator)
{
    if (string == NULL)
	return NULL;
    
    char *dup = strdup(string);
    char *ref = dup; // so we can free later
    
    char **split = malloc(DEF_ARR_SIZE * sizeof(char*));
    
    int max_size = DEF_ARR_SIZE;
    int len = 0;
    int size = 0;
    int exit = 0;
    
    while (!exit){
	if (*dup == separator || *dup == '\0'){
	    if (size >= max_size){ // resize the array if we go over size limit
		split = realloc(split, (max_size *= 2) * sizeof(char*));
	    }
	    split[size] = malloc(len + 1); // allocate space for string and null terminator
	    snprintf(split[size], len + 1, "%s", string);
	    size++;
	    if (*dup == '\0'){
		break;
	    }
	    string += len + 1; // move pointer of string over
	    len = 0; // reset length
	    dup += 1; // skip over the separator
	} else {
	    len++;
	    dup++;
	}
    }

    free(ref);
        
    split = realloc(split, size * sizeof(char*)); // save memory
    string_arr* ret = malloc(sizeof(string_arr));
    ret->data = split;
    ret->len = size;
        
    return ret;
}

void print_int_arr(int *arr, int len)
{
    int i;
    
    for (i = 0; i < len; ++i) {
	printf("%d\n", arr[i]);
    }

}

void print_double_arr(double *arr, int len)
{
    int i;
    
    for (i = 0; i < len; ++i) {
	printf("%lf\n", arr[i]);
    }

}

/*
 * Checks whether a given interval is valid and returns 1 if so. The start of the interval
 * must be before the end, and both start and end must be positive
 */
int interval_valid(double interval_start, double interval_end)
{
    return (interval_start < interval_end) && interval_start >= 0 && interval_end > 0;
}

/*
 * Gets a subinterval of the given event array. The array provided is not modified.
 * This should be used to prevent excessive reading from file. Very naive - complexity
 * probably O(n^2). Probably marginally better than processing files.
 *
 * Note that an unmodified event array is expected - i.e. one which contains the length in
 * the zeroth location.
 */
double* get_event_subinterval(double *events, double interval_start, double interval_end)
{

    if (!interval_valid(interval_start, interval_end) || events == NULL)
	return NULL;
    
    int count = 1;
    double *start = NULL;
//    double *end = NULL;
    
    int arr_end = events[0];

    int i;
    double cur_time;
    
    //printf("interval start %lf, interval end %lf\n", interval_start, interval_end);
        
    for (i = 1, cur_time = events[i]; (i < arr_end) && (events[i] <= interval_end); ++i, cur_time = events[i]) {
	//printf("cur_time %lf, i %d, i > arr_end %d, i < arr_end %d\n", cur_time, i, i > arr_end, i < arr_end);
	if (cur_time >= interval_start && start == NULL){
	    //printf("interval start detected at %lf\n", cur_time);
	    start = (events + i); // start pointer moves to indicate the start of the interval
	    count++;
	} else if (cur_time > interval_end){
	    //printf("interval end detected at %lf\n", cur_time);
	    count++;
	    break;
	} else if (cur_time > interval_start && cur_time < interval_end){
	    count++;
	}
    }
    
    // If we didn't find any events within the specified interval
    if (count == 1)
	return NULL;

    double *pruned_events = malloc((count + 1) * sizeof(double));
    pruned_events[0] = count;

    //printf("end - start %d\n", end - start);
    
    memcpy((void*)(pruned_events + 1), start, count * sizeof(double));
    
    
    /* for (i = 0; i < count; ++i) { */
    /* 	printf("%lf, %d\n", pruned_events[i], i); */
    /* } */

    //printf("interval start %lf, first event: %lf, interval end %lf, last event %lf, count %d\n", interval_start, *start, interval_end, *(end - 1), count);

    return pruned_events;
}

/*
 * Prints the estimates contained within the given estimate array
 */
void print_estimates(est_arr* estimates)
{
    int len = estimates->len;
    
    int i;
    
    for (i = 0; i < len && estimates->estimates[i] != NULL; ++i) {
	printf("Interval %d estimate is:\na: %lf\nb: %lf\nstart: %lf\nend: %lf\n\n", i, estimates->estimates[i]->est_a, estimates->estimates[i]->est_b, estimates->estimates[i]->start, estimates->estimates[i]->end);
    }

}

/*
 * Frees an est_arr struct and all the pointers that it contains.
 */
void free_est_arr(est_arr *estimates)
{
    int len = estimates->len;
    
    int i;
    
    for (i = 0; i < len && estimates->estimates[i] != NULL; ++i) {
	free(estimates->estimates[i]);
    }
    
    free(estimates->estimates);
    free(estimates);
}

/*
 * Prints an array of strings with a message before it.
 */
void print_string_array(char* message, char** array, int len)
{
    int i;
    
    printf("%s\n", message);
    
    for (i = 0; i < len; ++i) {
	printf("%s\n", array[i]);
    }

}

/*
 * Checks that the paramlist provided contains parameters with names corresponding to the
 * strings provided in the required_params array. Returns 1 if the parameters are present,
 * 0 otherwise.
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
 * Creates a parameter file with default parameters and comments.
 * Returns 1 if the file could not be opened, 0 if successful
 */
int create_default_param_file(char* filename)
{
    printf("Writing default parameter file to %s...\n", filename);
    
    if (file_exists(filename)){
	printf("The file already exists. Overwrite? (y(es) or n(o))\n");
	char read = getchar();
	if (read == 'y'){
	    printf("Overwriting...");
	} else {
	    printf("Aborted.\n");
	    exit(1);
	}
    }

    FILE* fp = fopen(filename, "w");

    if (fp == NULL){
	perror("Could not open file");
	return 1;
    }

//    fprintf(fp, "%s\n%s %s\n\n", "", "",);

    // i/o files and stuff
    fprintf(fp, "%s\n\n", "# Inline comments are not supported!");
    // data output
    put_section_header(fp, "Data Output");
    fprintf(fp, "%s\n%s %s\n\n", "# file to which generator outputs generated event "\
	    "data","outfile", DEFAULT_OUTFILE);
    fprintf(fp, "%s\n%s %s\n\n", "# Files to which functions generated using"\
	    " gaussians are output. \n# File for gaussians in their raw form"\
	    " (xpos stdev weight). Additional files may\n# be generated when"\
	    " generating functions with gaussians - these files will have\n"\
	    "# \"_sum\" and \"_contrib\" appended to the end of the value provided below.",
	    "function_outfile", DEFAULT_FUNC_OUTFILE);
    fprintf(fp, "%s\n%s %s\n\n", "# File for gaussians generated from event data."\
	    " Additional files may be generated\n# depending on the switches"\
	    " provided to the program. These files will have \"_sum\"\n# and"\
	    " \"_contrib\" appended to the value provided below.",
	    "stream_function_outfile", DEFAULT_STREAM_FUNC_OUTFILE);
    fprintf(fp, "%s\n%s %s\n\n", "# this will be appended to the output file for each separate"	\
	    " stream. The stream\n# number will be added at the end.", "stream_ext",
	    DEFAULT_EXTENSION);
    fprintf(fp, "%s\n%s %s\n\n", "# Estimators output data to this file if no "\
	    "output file is specified for the\n# estimator being used", "est_outfile",
	    DEFAULT_EST_OUTFILE);
    fprintf(fp, "%s\n%s %s\n", "# Individual output files for specific estimators.", 
	    "# ols_output", DEFAULT_OLS_OUT);
    fprintf(fp, "%s %s\n", "# iwls_output", DEFAULT_IWLS_OUT);
    fprintf(fp, "%s %s\n", "# pc_output", DEFAULT_PC_OUT);
    fprintf(fp, "%s\n%s %lf\n\n", "# Used to determine the granularity of data in"\
	    " output files. With a step of 1,\n# data in interval [0,5] is gathered"\
	    " at 0, 1, 2, 3, 4 and 5 along the axis.", "output_step",
	    DEFAULT_STEP);
    fprintf(fp, "%s %s\n", "# base_output", DEFAULT_BASE_OUT);
    fprintf(fp, "%s\n%s %s\n\n", "# File to output gaussian estimate data to.", 
	    "gauss_est_outfile", DEFAULT_GAUSS_EST_OUT);
    fprintf(fp, "%s\n%s %d\n\n", "# define the verbosity of output to data files.\n"\
	    "# 0 output only event data for each run\n# 1 output event times and"\
	    " lambda values for each run\n# 2 output event time and lambda values"\
	    " for each run, as well as midpoints and\n# bin counts, to a single"\
	    " file\n# 3 output the above, but also save a file with only event"\
	    " data in it (all data\n# is saved as {filename}_ad, events as "\
	    "{filename}_ev)", "verbosity", DEFAULT_VERBOSITY);
    // data input
    put_section_header(fp, "Data Input");
    fprintf(fp, "%s\n%s %s\n\n", "# Default input file used for estimation",
	    "infile", DEFAULT_INFILE);
    // generator parameters
    put_section_header(fp, "Generation Parameters");
    fprintf(fp, "%s\n%s %lf\n\n", "# The time at which to start generation.",
	    "start_time", DEFAULT_START);
    fprintf(fp, "%s\n%s %d\n\n", "# The number of streams to generate.", "nstreams",
	    DEFAULT_NSTREAMS);
    fprintf(fp, "%s\n%s %s\n\n", "# The delay between zero and each stream. If the"\
	    " delay is greater than zero, then\n# when generating events, instead"\
	    " of using lambda(t) to generate, lambda(t+delta)\n# is used.",
	    "timedelta", DEFAULT_TIMEDELTA);
    fprintf(fp, "%s\n%s %d\n\n", "# The lambda value to use when generating events."\
	    " This value is used during thinning\n# to calculate whether an event"\
	    " should be kept. lambda must be > lambda(t) for all\n# times start_time"\
	    " <= k <= start_time + interval_time ", "lambda", DEFAULT_LAMBDA);
    fprintf(fp, "%s\n%s %lf\n\n", "# The length of the interval in which events should be"\
	    " generated.", "interval_time", DEFAULT_INTERVAL);
    fprintf(fp, "%s\n%s %d\n\n", "# The seed to use when generating events. Currently"\
	    " unused.", "seed", DEFAULT_SEED);
    fprintf(fp, "%s\n%s %s\n", "# expression parameters\n# The equation must"\
	    " contain the variable 't'. The value of the variable will be set"\
	    " by\n# the program. Do not set it here.", "expression",
	    DEFAULT_EXPRESSION);
    fprintf(fp, "%s %d\n", "a", DEFAULT_A);
    fprintf(fp, "%s %d\n", "b", DEFAULT_B);
    fprintf(fp, "%s %.10lf\n\n", "alpha", DEFAULT_ALPHA);
    // gaussian parameters
    put_section_header(fp, "Gaussian Function Generator");
    fprintf(fp, "%s\n%s %lf\n\n", "# Standard deviation to"\
	    " apply to generated gaussians", "gauss_stdev", DEFAULT_STDEV);
        fprintf(fp, "%s\n%s %s\n\n", "# Specifies whether to disregard the standard"\
	    " deviation specified above and\n# instead calculate the standard"\
	    " deviation to apply based on the function\n# \\alpha * \\Delta t. "\
	    "This allows the standard deviation to be specified as\n# a function"\
	    " of the step between the kernels. A yes value will use the above\n"\
	    "# standard deviation.", "simple_stdev", DEFAULT_SIMPLE_STDEV);
    fprintf(fp, "%s\n%s %lf\n\n", "# Specifies the alpha parameter which is used"\
	    " to calculate the standard deviation of\n# kernels when generating"\
	    " functions from gaussians. Used when simple_stdev is set to no.\n# The "\
	    "value of this parameter is multiplied with the gauss_generation_step"\
	    " parameter\n# to calculate the standard deviation.",
	    "stdev_alpha", DEFAULT_STDEV_ALPHA);
    fprintf(fp, "%s\n%s %lf\n\n", "# Specifies the distance on the x-axis between" \
	    " each gaussian when generating.", "gauss_generation_step", DEFAULT_GEN_STEP);
    fprintf(fp, "%s\n%s %lf\n\n", "# Specifies the distance between points at"\
	    " which the gaussian is sampled when summing\n# or outputting data."\
	    " A high value for this will result in fast computation but loss\n#"\
	    " of detail, and a low value will give more granularity but summing"\
	    " gaussians will take longer. ","gauss_resolution",
	    DEFAULT_GAUSS_RESOLUTION);
    fprintf(fp, "%s\n%s %lf\n\n", "# Multiplier to apply to weights of gaussians when"\
	    " generating random functions.\n# The standard gaussian weight is ~"\
	    " N(0,1). The standard functions have on average\n# vary between -5"\
	    " and 5, which may not be appropriate. Multiplying the weight\n# "\
	    "values gives a greater range of values but preserves the shape of"\
	    " the function.", "gauss_func_multiplier", DEFAULT_GAUSS_MULTIPLIER);
    // estimator parameters
    put_section_header(fp, "Estimator Parameters");
    fprintf(fp, "%s\n%s %s\n\n", "# specifies the type of estimator to use. Options are"\
	    " ols, iwls, piecewise, baseline", "est_type", DEFAULT_ESTIMATOR);
    fprintf(fp, "%s\n%s %lf\n\n", "# Specify the time from which to start the"\
	    " estimation. In general, this should be\n# the same as the start_time"\
	    " given above, but it can be used if you only need\n# to estimate a certain"\
	    " portion of your data.", "est_start_time", DEFAULT_START);
    fprintf(fp, "%s\n%s %lf\n\n", "# The length of the interval that you want to estimate."\
	    " The interval estimated\n# will be [est_start_time, est_start_time +"\
	    " est_interval_time].", "est_interval_time", DEFAULT_INTERVAL);
    // ols
    put_section_header(fp, "ols parameters");
    fprintf(fp, "%s %d\n\n", "ols_subintervals", DEFAULT_SUBINTERVALS);
    // iwls
    put_section_header(fp, "IWLS Estimator Parameters");
    fprintf(fp, "%s %d\n", "iwls_iterations", DEFAULT_IWLS_ITERATIONS);
    fprintf(fp, "%s %d\n\n", "iwls_subintervals", DEFAULT_SUBINTERVALS);

    // piecewise
    put_section_header(fp, "Piecewise Estimator Parameters");
    fprintf(fp, "%s\n%s %d\n\n", "# Number of times to iterate the IWLS estimator. Recommended"\
	    " value is between 2 and 5.", "pc_iwls_iterations",DEFAULT_IWLS_ITERATIONS);
    fprintf(fp, "%s\n%s %d\n\n", "# Number of subintervals to use for the IWLS estimator."\
	    " The photon stream data will be\n# put into this many bins and the estimate will run on that.",
	    "pc_iwls_subintervals", DEFAULT_SUBINTERVALS);
    fprintf(fp, "%s\n%s %lf\n\n", "# Maximum amount to extend the lines estimated for each"\
	    " subinterval. Longer lines will\n# be more likely to be rejected if the data"\
	    " varies a lot. If the first attempt fails,\n# further attempts will be made "\
	    "with fractional values of the original.", "pc_max_extension", DEFAULT_MAX_EXTENSION);
    fprintf(fp, "%s\n%s %d\n\n", "# The maximum number of breakpoints used by the piecewise"\
	    " estimate. The estimator will\n# use at most this number of lines to estimate"\
	    " the underlying function. If extensions\n# succeed or the data is quite linear,"\
	    " fewer subintervals will be required to make a\n# good estimate, so the actual "\
	    "number of breakpoints in the estimate may be lower.", "pc_max_breakpoints", 
	    DEFAULT_MAX_BREAKPOINTS);
    fprintf(fp, "%s\n%s %lf\n\n", "# Proportion of the interval time that the minimum possible"\
	    " subinterval length should\n# be. Very short subintervals often result in very"\
	    " bad estimates, especially if the\n# data has large variations.", 
	    "pc_min_interval_proportion", DEFAULT_MIN_INTERVAL_PROP);
    fprintf(fp, "%s\n%s %lf\n\n", "# Threshold at which to reject an extension attempt. This"\
	    " threshold limits the value\n# of the probability mass function calculated at"\
	    " each point on the extended line,\n# checking how likely is it that the point"\
	    " is actually part of the data set. A lower\n# value means that extension will"\
	    " be continued even if the probability is very low.", "pc_pmf_threshold", DEFAULT_PMF_THRESHOLD);
    fprintf(fp, "%s\n%s %lf\n\n", "# A different threshold for summation of probability mass"\
	    " functions. Used when summing\n# the probability mass functions at each point.", 
	    "pc_pmf_sum_threshold", DEFAULT_PMF_SUM_THRESHOLD);
    // baseline
    put_section_header(fp, "Baseline Estimator Parameters");
    fprintf(fp, "%s %d\n", "base_iwls_iterations", DEFAULT_IWLS_ITERATIONS);
    fprintf(fp, "%s %d\n", "base_iwls_subintervals", DEFAULT_SUBINTERVALS);
    fprintf(fp, "%s %lf\n", "base_max_extension", DEFAULT_MAX_EXTENSION);
    fprintf(fp, "%s %d\n", "base_max_breakpoints", DEFAULT_MAX_BREAKPOINTS);
    fprintf(fp, "%s %lf\n", "base_min_interval_proportion", DEFAULT_MIN_INTERVAL_PROP);
    fprintf(fp, "%s %lf\n", "base_pmf_threshold", DEFAULT_PMF_THRESHOLD);
    fprintf(fp, "%s %lf\n\n", "base_pmf_sum_threshold", DEFAULT_PMF_SUM_THRESHOLD);
    // gaussian
    put_section_header(fp, "Gaussian Estimator Parameters");
    fprintf(fp, "%s\n%s %lf\n\n", "# Standard deviation to apply to gaussian"\
	    " kernels used to estimate functions\n# Changing this value will"\
	    " change the normalisation constant required to get\n# the"\
	    " correct function estimate.","gauss_est_stdev",DEFAULT_GAUSS_EST_STDEV);
    fprintf(fp, "%s\n%s %lf\n\n", "# Specify resolution of kernels used to estimate"\
	    " gaussians. A small value will\n# give higher precision but take longer"
	    ,"gauss_est_resolution", DEFAULT_GAUSS_EST_RESOLUTION);
    put_section_header(fp, "Time Delta Estimator Parameters");
    fprintf(fp, "%s\n%s %s\n\n", "# Whether to estimate the time delay. If"\
	    " this is set to no, the timedelta will be\n# read from the"\
	    " parameter file and used to combine functions if multiple streams\n"\
	    "# are being estimated.", "estimate_delta", DEFAULT_DO_DELTA_EST);
    fprintf(fp, "%s\n%s %s\n\n", "# Method to use to estimate the time delay."\
	    " Set to either pdf or area. The pdf\n# method uses the probability"\
	    " mass function to calculate likelihood of two\n# streams sharing"\
	    " the same underlying functions, given the bin counts from the\n"\
	    "# event stream. The time delay is the point at which the sum of"\
	    " the probability\n# mass functions across the whole space is"\
	    " highest. The area method is\n# simpler, estimating the delay"\
	    " based on the area between the curves of two\n# functions. The"\
	    " point at which the area is smallest is most likely the actual\n"\
	    "# time delay.", "delta_est_method", DEFAULT_DELTA_EST_METHOD);
    fprintf(fp, "%s\n%s %lf\n\n", "# This parameter defines the step by which the"\
	    " guess at the value of delta increases.\n# The value of delta starts"\
	    " at a negative value and moves towards a positive value\n# in steps"\
	    " of this value. A low value will provide more granularity on data,"\
	    " but it\n# will take longer to find an estimate.", "delta_est_step",
	    DEFAULT_DELTA_EST_STEP);
    fprintf(fp, "%s\n%s %lf\n\n", "# The estimator will check delta values of"\
	    " between + and - this value. A lower value\n# will mean that the"\
	    " estimation is quicker, but may result in the true delta not being\n"\
	    "# found if it is not within the range provided. The program starts"\
	    " at the negative\n# of the value and iterates, increasing by"\
	    " delta_est_step until it reaches the positive.", "delta_est_max_delta",
	    DEFAULT_MAX_DELTA);
    put_section_header(fp, "Area Method");
    fprintf(fp, "%s\n%s %lf\n\n", "# Point at which to start approximating the"\
	    " area between functions.", "delta_est_area_start", DEFAULT_START);
    fprintf(fp, "%s\n%s %lf\n\n", "# Length of the interval to use when approximating"\
	    " area between functions.", "delta_est_area_interval", DEFAULT_INTERVAL);
    fprintf(fp, "%s\n%s %lf\n\n", "# This parameter is used to define the step"\
	    " taken when finding estimates. In the\n# case of the area method,"\
	    " a sample of the area is taken at uniform intervals\n# according to"\
	    " this value. A smaller value may provide more accurate\n# estimates,"\
	    " but the calculation will take longer.","delta_est_area_resolution",
	    DEFAULT_DELTA_EST_AREA_RESOLUTION);
    put_section_header(fp, "PMF Method");
    fprintf(fp, "%s\n%s %lf\n\n", "# Defines the point at which to start combining"\
	    " functions in the pmf estimator.\n# This should usually be the same"\
	    " as the interval specified in the generator\n# parameters if you are"\
	    " using a generated function. It may also be useful for\n# getting"\
	    " better estimates on functions that vary greatly at some points,\n"\
	    "# and are smooth at others, removing the large variation from"\
	    " consideration in\n# the time delta estimation and perhaps improving"\
	    " the result.", "delta_est_combine_start", DEFAULT_START);
    fprintf(fp, "%s\n%s %lf\n\n", "# Defines the length of the interval to use"\
	    " when combining two functions during\n# the pmf delta estimation.",
	    "delta_est_combine_interval", DEFAULT_INTERVAL);
    fprintf(fp, "%s\n%s %lf\n\n", "# This parameter is used to define the step"\
	    " taken when finding estimates.\n# In the case of the pmf method, the"\
	    " points at which the value of the pmf\n# is calculated depend on this"\
	    " value. A smaller value may provide more\n# accurate estimates, but"\
	    " the calculation will take longer.", "delta_est_pmf_resolution",
	    DEFAULT_DELTA_EST_PMF_RESOLUTION);
    fprintf(fp, "%s\n%s %d\n\n", "# Defines the number of bins used to split"\
	    " the interval. If the bin length\n# resulting from this value"\
	    " differs from the length of time which each lambda\n# represents,"\
	    " then a normalisation constant is required to find the original\n"\
	    "# function at its original magnitude. If lambda represents arrivals"\
	    " per second,\n# then a bin length of 1 second is ideal because then"\
	    " each bin can be represented\n# with a single value of lambda."\
	    " Otherwise, it is necessary to sum the lambda\n# values in the bin"\
	    " interval to see whether the number of events received\n# is close"\
	    " to the number expected given the value of lambda.", 
	    "delta_est_num_bins", DEFAULT_DELTA_EST_BINS);
    // normaliser
    put_section_header(fp, "Normaliser Parameters");
    fprintf(fp, "%s\n%s %lf\n\n", "# Defines the initial value checked when finding"\
	    " the normalisation constant,\n# used to normalise the function so that"\
	    " it is as close to the original function\n# as possible. This is"\
	    " necessary when using the gaussian estimation method, because\n# the"\
	    " value of the function at each point depends on the number of"\
	    " gaussians, as\n# well as on their standard deviation.",
	    "normaliser_est_initial", DEFAULT_NORMALISER_INITIAL);
    fprintf(fp, "%s\n%s %lf\n\n", "# Maximum value to check when finding the "\
	    "normalisation constant.", "normaliser_est_max", DEFAULT_NORMALISER_MAX);
    fprintf(fp, "%s\n%s %lf\n\n", "# Value by which to increase the checked value of"\
	    " the normaliser in each iteration.", "normaliser_est_step",
	    DEFAULT_NORMALISER_STEP);
    fprintf(fp, "%s\n%s %d", "# Number of bins into which to split the interval"\
	    " being estimated into to find the\n# normaliser. Generally, this value"\
	    " should be equal to the length of the interval\n# being estimated"\
	    " divided by the time period for which the lambda value applies.\n# For"\
	    " example, if your lambda represents the number of events per second,"\
	    " and you\n# have an interval time of 100 seconds, you should use 100"\
	    " subintervals in order\n# for the number of events counted in each bin"\
	    " to be of a similar magnitude to the\n# lambda value that the function"\
	    " estimates should produce if you want to get back\n# a function estimate"\
	    " that is on the same scale as the generating function. This\n# applies"\
	    " in particular to estimates with gaussians, where the standard deviation\n"\
	    "# of the gaussians being used affects the height of the estimated"\
	    " function at each\n# point.", "normaliser_est_subintervals",
	    DEFAULT_NORMALISER_SUBINTERVALS);
    


//    fprintf(fp, "%s\n%s %s\n\n", "", "", );
    
    fclose(fp);

    printf("done\n");

    return 0;
}

/*
 * Returns a formatted string to be used for section headings
 */
void put_section_header(FILE* fp, char* heading)
{
    int len = 80;
    int midlen = len - strlen(heading);
    int i;
    
    for (i = 0; i < len; ++i) {
	fputc('#', fp);
    }

    fputc('\n', fp);
    fputc('#', fp);

    for (i = 0; i < midlen/2; ++i) {
	fputc(' ', fp);
    }

    fprintf(fp, "%s", heading);


    for (i += strlen(heading); i < len - 2; ++i) {
	fputc(' ', fp);
    }

    fputc('#', fp);
    fputc('\n', fp);

    for (i = 0; i < len; ++i) {
	fputc('#', fp);
    }
    
    fputc('\n', fp);
}

/*
 * Prints a gaussian vector
 */
void print_gauss_vector(gauss_vector* G)
{
    int i;
    
    for (i = 0; i < G->len; ++i) {
	printf("%d: mu = %.5lf, stdev = %.5lf, wt = %.5lf\n", i, 
	       G->gaussians[i]->mean, G->gaussians[i]->stdev, G->w[i]);
    }
}

/*
 * Frees memory allocated to a gaussian vector
 */
void free_gauss_vector(gauss_vector* G)
{
    free(G->w);
    int i;
    
    for (i = 0; i < G->len; ++i) {
	free(G->gaussians[i]);
    }

    free(G->gaussians);
    free(G);
}

/*
 * Free memory allocated to a double_multi_arr
 */
void free_double_multi_arr(double_multi_arr* arr)
{
    int i;
    
    for (i = 0; i < arr->len; ++i) {
	free(arr->data[i]);
    }

    free(arr->lengths);
    free(arr->data);
    free(arr);
}

/*
 * Checks whether all parameters specified in the checklist are present
 * in the given parameter list.
 */
int has_missing_parameters(string_arr* checklist, paramlist* params)
{
    int i;
    int missing = 0;

    for (i = 0; i < checklist->len; ++i) {
	if (get_param(params, checklist->data[i]) == NULL){
	    printf("Missing %s\n", checklist->data[i]);
	    missing = 1;
	}
    }

    return missing;
}

/*
 * calculates the value of an estimate at a given point in time.
 */
double estimate_at_point(est_arr* estimate, double time)
{
    est_data* idata = data_at_point(estimate, time);

    if (idata == NULL)
	return 0;
    
    return idata->est_a + idata->est_b * time;
}

/*
 * Finds the value of the given estimate at all points in the given array.
 */
double_arr* estimate_at_points(est_arr* estimate, double* points, int len)
{
    if (estimate == NULL || points == NULL || len <= 0)
	return NULL;

    double_arr* r = init_double_arr(len);
    
    int i;

    for (i = 0; i < len; ++i) {
	double est = estimate_at_point(estimate, points[i]);
	if (est < 0){
	    printf("WARNING: Estimate at %lf is less than zero! (%lf). Setting to zero.\n", points[i], r->data[i]);
	    est = 0;
	}
	
	
	r->data[i] = est;
//	printf("Estimate at %lf is %lf\n", points[i], r->data[i]);
    }

    return r;
}

/*
 * Returns the data from the given estimate that can be used to calculate
 * the function value at the specified time.
 */
est_data* data_at_point(est_arr* estimate, double check_time)
{
    if (estimate == NULL)
	return NULL;
    
    est_data* current = NULL;
    est_data* ret = NULL;
        
    int i;

    for (i = 0; i < estimate->len; ++i) {
	current = estimate->estimates[i];
	if (current->start <= check_time && check_time <= current->end){
	    ret = current;
	}
    }
    return ret;
}

/*
 * Initialises a double_multi_arr struct which contains num_arrays arrays,
 * each having the same length, array_length.
 */
double_multi_arr* init_multi_array(int num_arrays, int array_length)
{
    if (array_length <= 0 || num_arrays <= 0)
	return NULL;

    double_multi_arr* res = malloc(sizeof(double_multi_arr));
    res->len = num_arrays;
    res->lengths = malloc(sizeof(int) * num_arrays);
    res->data = malloc(sizeof(double*) * num_arrays);
    
    int i;
    
    for (i = 0; i < num_arrays; ++i) {
	res->data[i] = malloc(array_length * sizeof(double));
	res->lengths[i] = array_length;
    }

    return res;
}

/*
 * Initialises a double_arr struct with an array of the given length
 */
double_arr* init_double_arr(int len)
{
    if (len <= 0)
	return NULL;

    double_arr* r = malloc(sizeof(double_arr));
    r->len = len;
    r->data = malloc(sizeof(double) * r->len);
    
    return r;
}
