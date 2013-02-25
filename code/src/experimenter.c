#include "experimenter.h"

void print_exp_set(exp_set* set);
void roll_experiment(exp_tuple_arr* exps, int level);
int increment_experiment(exp_tuple_arr* exp);
void update_parameters(exp_tuple_arr* exp, paramlist* params);
int parameters_coherent(paramlist* experiment, paramlist* def, char** check, int check_len);
exp_set* experiment_setup(paramlist* exp_params, paramlist* def_params);
void execute_experiments(paramlist* exp_params, paramlist* def_params, char* in_dir, char* out_dir, exp_set* experiments);

void run_experiments(char* exp_paramfile, char* def_paramfile, char* indir, char* outdir)
{
    paramlist* exp_list = get_parameters(exp_paramfile);
    paramlist* def_list = get_parameters(def_paramfile);
    if (indir == NULL)
	indir = get_string_param(exp_list, "input_dir");
    if (outdir == NULL)
	outdir = get_string_param(exp_list, "output_dir");

    if (indir == NULL || outdir == NULL){
	printf("You must provide directories to read from and output to either"\
	       " by using the -i and -o switches, or by specifying input_dir and"\
	       "output_dir in the experiment parameter file.\n");
	exit(1);
    }

    printf("Checking input directory %s is readable...\n", indir);
    if (access(indir, R_OK)){
	perror("Error reading input directory.");
	exit(1);
    }
    printf("OK\n");

    printf("Checking that output directory %s exists and is writable...\n", outdir);
    if (!dir_exists(outdir)){
	printf("Directory does not exist. Creating output directory %s.\n", outdir);
	if (create_dir(outdir) == 0){
	    perror("Could not create directory.");
	    exit(1);
	}
    } else {
	printf("The directory already exists. Proceeding may overwrite all data inside it."\
	       "Continue? (y(es) or n(o))\n");
	char read = getchar();
	if (read == 'y'){
	    printf("Continuing...\n");
	} else {
	    printf("Aborted.\n");
	    exit(1);
	}
    }

    exp_set* experiments = experiment_setup(exp_list, def_list);
    execute_experiments(exp_list, def_list, indir, outdir, experiments);

    free_list(exp_list);
    free_list(def_list);
    free_exp_set(experiments);
}

exp_set* experiment_setup(paramlist* exp_list, paramlist* def_list)
{

    char* ename = get_string_param(exp_list, "experiment_names");
    string_arr* base_strings = string_split(ename, ',');
    string_arr* experiment_params = malloc(sizeof(string_arr));
    experiment_params->data = malloc(base_strings->len * 4 * sizeof(char*));
    experiment_params->len = base_strings->len * 4;
    
    int i;
    char* req_param = malloc(200);
    for (i = 0; i < base_strings->len; ++i) {
	// Create the strings which indicate which parameters are required by the experimenter
	// in order to do experiments
	sprintf(req_param, "test_%s", base_strings->data[i]);
	experiment_params->data[i*4] = strdup(req_param);
	sprintf(req_param, "%s_params", base_strings->data[i]);
	experiment_params->data[i*4 + 1] = strdup(req_param);
	sprintf(req_param, "%s_estimator", base_strings->data[i]);
	experiment_params->data[i*4 + 2] = strdup(req_param);
	sprintf(req_param, "%s_type", base_strings->data[i]);
	experiment_params->data[i*4 + 3] = strdup(req_param);
    }

    free(req_param);
    
    // Make sure that the parameters which specify what parameters will be experimented on
    // are present in the experiment parameter list.
    if (!has_required_params(exp_list, experiment_params->data, experiment_params->len)){
	print_string_array("Some parameters required for experiments are missing. " \
			   "Ensure that your file contains the following parameters,\nand"\
			   " that you're using the right file. Pass the experiment parameter file"\
			   " to -x, and the default file to -p.",
			   experiment_params->data, experiment_params->len);
	exit(1);
    }

    int j;
    int missing = 0;
    int req = 0;
    int baselen = base_strings->len;

    exp_set* exval = malloc(sizeof(exp_set));
    exval->exp_names = malloc(baselen * sizeof(char*));
    exval->exps = malloc(baselen * sizeof(exp_tuple_arr*));
    exval->len = baselen;

    for (i = 0; i < baselen; ++i) {
	char teststr[30];
	char paramstr[30];

	sprintf(teststr, "test_%s", base_strings->data[i]);
	// Read parameter data from this string
	sprintf(paramstr, "%s_params", base_strings->data[i]);
	exval->exp_names[i] = strdup(base_strings->data[i]);

	if (get_string_param(exp_list, teststr) == NULL){
	    printf("%s not defined in paramfile. Skipping.\n", teststr);
	    continue;
	} else if (strcmp(get_string_param(exp_list, teststr), "yes") == 0){
	    printf("%s requested. Checking that experimental values are defined in %s.\n", teststr, paramstr);
	    char* p = get_string_param(exp_list, paramstr);
	    if (p == NULL){
		printf("Expected parameter %s not defined. Skipping.\n", paramstr);
		continue;
	    }

	    string_arr* testparams = string_split(p, ',');

	    // Check whether parameters being modified exist in both parameter files.
	    if (!parameters_coherent(exp_list, def_list, testparams->data, testparams->len)){
	    	missing = 1;
	    } else {
	    	printf("OK\n");
	    }
	    printf("allocating\n");
	    // Allocate memory and define lengths for the tuple array which is used
	    // to store parameters being experimented on.
	    exval->exps[i] = malloc(sizeof(exp_tuple_arr));
	    exval->exps[i]->data = malloc(sizeof(exp_tuple*) * testparams->len);
	    exval->exps[i]->num_params = testparams->len;
	    exval->exps[i]->param_ind = calloc(testparams->len, sizeof(int));
	    
	    for (j = 0; j < testparams->len; ++j) {
		// Parse the experimental values for each parameter into an
		// individual tuple and add it to the experiment set.
		exp_tuple* etup = malloc(sizeof(exp_tuple));
		etup->param_name = strdup(testparams->data[j]);
		etup->param_vals = parse_param(exp_list, etup->param_name);
		exval->exps[i]->data[j] = etup;
	    }
	    req = 1;
	    free_string_arr(testparams);
	} else {
	    exval->exps[i] = NULL;
	}
    }

    if (missing){
	printf("Some parameters you were trying to experiment on were not defined"\
	       " in one of the parameter files. Please add them and try again.\n");
	exit(1);
    }
    if (req == 0){
	printf("No experiments requested. Exiting.\n");
	exit(1);
    }

    //    print_exp_set(exval);
    free_string_arr(base_strings);
    free_string_arr(experiment_params);
    
    return exval;
}

/*
 * Executes the experiments defined in the experiment set provided. If the run_separately
 * parameter is set to yes, then each parameter is experimented on independently of the
 * others in that set. Otherwise, all possible parameter combinations are experimented on.
 */
void execute_experiments(paramlist* exp_list, paramlist* def_list, char* in_dir, char* out_dir, exp_set* experiments)
{
    int i, j, k;
    int expcount = 0;
    char* tmp = NULL;
    int multiple = 0;
    int num_functions = 10;
    int num_streams = 2;
    char* function_fname = get_string_param(def_list, "function_outfile");
    char* fname = get_string_param(def_list, "outfile"); // default generator output filename
    char* pref = get_string_param(def_list, "stream_ext"); // default extension
    
    for (i = 0; i < experiments->len; ++i) {
	printf("Running experiments for %s\n", experiments->exp_names[i]);
	if (experiments->exps[i] == NULL){
	    printf("No experiment on %s\n", experiments->exp_names[i]);
	    continue;
	}
	tmp = malloc(strlen(experiments->exp_names[i]) + strlen("_estimator") + 5);
	sprintf(tmp, "%s_type", experiments->exp_names[i]);
	if (strcmp(get_string_param(exp_list, tmp), "delay") == 0){
	    multiple = 1;
	} else if (strcmp(get_string_param(exp_list, tmp), "function") == 0){
	    multiple = 0;
	} else {
	    printf("Unknown type %s for experiment %s. Use delay or function\n",
		   get_string_param(exp_list, tmp), experiments->exp_names[i]);
	    continue;
	}
	// Read the estimator type to use from the parameter file
	sprintf(tmp, "%s_estimator", experiments->exp_names[i]);
	char* est_type = get_string_param(exp_list, tmp);
	printf("Using estimator %s\n", est_type);
	exp_tuple_arr* current_exp = experiments->exps[i];
	
	char* output_directory = malloc(strlen(experiments->exp_names[i]) + strlen(out_dir) + 5);
	sprintf(output_directory, "%s/%s", out_dir, experiments->exp_names[i]);
	if (!create_dir(output_directory)){
	    printf("Something went wrong when creating directory %s.\n", output_directory);
	    perror("Error");
	    exit(1);
	}
	printf("output directory is %s\n", output_directory);
	
	if (strcmp(get_string_param(exp_list, "run_separately"), "yes") == 0){
	    for (j = 0; j < current_exp->num_params; ++j) {
		printf("Parameter %d: %s\nValues:\n", j, current_exp->data[j]->param_name);
		for (k = 0; k < current_exp->data[j]->param_vals->len; ++k) {
		    printf("%lf\n", current_exp->data[j]->param_vals->data[k]);
		    expcount++;
		}
	    }
	} else {
	    int sepcount = 0;
	    char* output_file = malloc(strlen(output_directory) + strlen("test") + 5);

	    // Go through all parameter combinations, running the estimator with each combination
	    do {
		sprintf(output_file, "%s/%s", output_directory, "test");
		update_parameters(current_exp, def_list);
		if (multiple){
		    printf("Estimating time delay.\n");
		    _multi_estimate(def_list, in_dir, output_file, num_streams, num_functions, 1, est_type);
		} else {
		    printf("Estimating functions.\n");
		    char* infname = infname = malloc(strlen(in_dir) + strlen(function_fname) + strlen(fname) + strlen(pref) + strlen(".dat") + 5);
		    int i;
		    
		    for (i = 0; i < num_functions; ++i) {
			sprintf(infname, "%s/%s_%d_%s%s_%d.dat", in_dir, function_fname, i, fname, pref, 0);
			free_est_arr(_estimate(def_list, in_dir, output_file, est_type));
		    }
		    free(infname);
		}
		printf("Exp %d complete\n", expcount);
		expcount++;
		sepcount++;
	    } while (increment_experiment(current_exp));
	    printf("Completed %d experiments for %s.\n", sepcount, experiments->exp_names[i]);
	    sepcount = 0;
	    free(output_file);
	}
	free(output_directory);
    }
	
    free(tmp);
    
    printf("total experiments: %d\n", expcount);
}

/*
 * Updates the given parameter list with the values indicated by the pointers
 * in the tuple array. The integers in the param_ind array indicate the index
 * of the value in the set of values stored in the tuple array to experiment on.
 */
void update_parameters(exp_tuple_arr* exp, paramlist* params)
{
    int i;
    
    for (i = 0; i < exp->num_params; ++i) {
	set_double_param(params, exp->data[i]->param_name, 
			 exp->data[i]->param_vals->data[exp->param_ind[i]]);
    }
}

/*
 * Increments the values inside the param_ind array of the given exp_tuple_arr
 * to move the pointers to indicate the parameters to use for the next
 * experiment. Returns zero if all values in the pointer array are zero after
 * incrementing, indicating that all experiments have been completed.
 */
int increment_experiment(exp_tuple_arr* exp)
{
    roll_experiment(exp, exp->num_params - 1);
    
    int i;
    int allzero = 1;
    
    for (i = 0; i < exp->num_params; ++i) {
	allzero = allzero && exp->param_ind[i] == 0;
    }

    return !allzero;
}

/*
 * Recursively modifies the values in the param_ind array of the given tuple_arr.
 * The value of the array at the given level is incremented, and if it exceeds the
 * number values for the given parameter, it is reset to zero, and the function is
 * called again to modify the indicator of the parameter on the previous level.
 */
void roll_experiment(exp_tuple_arr* exp, int level)
{
    if (level == -1)
	return;
    exp->param_ind[level]++;
    if (exp->param_ind[level] == exp->data[level]->param_vals->len){
	exp->param_ind[level] = 0;
	roll_experiment(exp, level - 1);
    }
}

/*
 * Checks whether the strings in the check array are contained in both parameter lists provided.
 */
int parameters_coherent(paramlist* experiment, paramlist* def, char** check, int check_len)
{
    int i;
    int ok = 1;
    
    for (i = 0; i < check_len; ++i) {
	if (get_param(experiment, check[i]) == NULL){
	    printf("Experiment parameter file is missing parameter %s\n", check[i]);
	    ok = 0;
	}
	if (get_param(def, check[i]) == NULL){
	    printf("Standard parameter file is missing parameter %s\n", check[i]);
	    ok = 0;
	}
    }

    return ok;
}

/*
 * Prints an experiment set, displaying all the data contained within it.
 */
void print_exp_set(exp_set* set)
{
    int i, j, k;

    printf("%d experiments possible.\n", set->len);
    
    for (i = 0; i < set->len; ++i) {
	if (set->exps[i] == NULL){
	    printf("No experiment on %s\n", set->exp_names[i]);
	    continue;
	}
	printf("Experiment %d: %s\n", i, set->exp_names[i]);
	for (j = 0; j < set->exps[i]->num_params; ++j) {
	    printf("Parameter %d: %s\nValues:\n", j, set->exps[i]->data[j]->param_name);
	    for (k = 0; k < set->exps[i]->data[j]->param_vals->len; ++k) {
		printf("%lf\n", set->exps[i]->data[j]->param_vals->data[k]);
	    }

	}
	printf("\n");
    }
}

/*
 * Parses a numerical parameter into an array of doubles.
 */
double_arr* parse_param(paramlist* params, char* param_to_parse)
{
    char* pdata = get_string_param(params, param_to_parse);
    string_arr* splitparam = string_split(pdata, ',');

    int i;

    double_arr* ret = malloc(sizeof(double_arr));
    double* vals = malloc(sizeof(double) * splitparam->len);
    
    for (i = 0; i < splitparam->len; ++i) {
	// If we find a ..., this indicates a range, so hand over
	// to the other function.
	if (strcmp(splitparam->data[i], "...") == 0){
	    free(vals);
	    free(ret);
	    return parse_double_range(splitparam);
	} else {
	    vals[i] = atof(splitparam->data[i]);
	}
    }

    ret->len = splitparam->len;
    ret->data = vals;
    
    free_string_arr(splitparam);
    return ret;
}

/*
 * Parses an array of strings in the form ["1.0", "2.0", "...", "5.0"] into a 
 * double array. In this case, the returned array would be 
 * [1.0, 2.0, 3.0, 4.0, 5.0]. The "..." is expanded to get the numbers that would
 * come between 2.0 and 5.0 with a step of 2.0-1.0. If you specify something like
 * 1.0,1.3,...,1.7, [1.0, 1.3, 1.6, 1.7] will be returned.
 */
double_arr* parse_double_range(string_arr* param_string)
{
    // The step is the difference between the first two values
    // in the array.
    double first = atof(param_string->data[0]);
    double step = atof(param_string->data[1]) - first;
    
    if (step <= 0)
	return NULL;

    int i;
    int contain = 0;
        
    // ensure that the array received contains a range.
    for (i = 1; i < param_string->len; ++i) {
	if (strcmp(param_string->data[i], "...") == 0 && i + 1 != param_string->len){
	    contain = 1;
	    break;
	}
    }
    
    if (contain){
	double_arr* ret = malloc(sizeof(double_arr));
	double last = atof(param_string->data[param_string->len - 1]);
	
	// The final array will contain this many values once the range
	// has been expanded.
	int len = (last - first) / step + 1;
	int special = 0;
	// if the step calculated does not go cleanly to the end of the
	// range, we will add the range end to the end of the array.
	if (first + (len - 1) * step < last) {
	    special = 1;
	    ret->len = len + 1;
	} else {
	    ret->len = len;
	}
	
	ret->data = malloc(sizeof(double) * ret->len);
	
	for (i = 0; i < len; ++i) {
	    ret->data[i] = first + i * step;
	}
	
	if (special)
	    ret->data[i] = last;
	
	return ret;
    }

    return NULL;
}

/*
 * Parses an array of strings in the form ["1", "2", "...", "5"] into a double array
 * in this case, the returned array would be [1, 2, 3, 4, 5]. The "..." is 
 * expanded to get the numbers that would come between 2 and 5 with a step of 2-1
 */
int_arr* parse_int_range(string_arr* param_string)
{
    int first = atoi(param_string->data[0]);
    int step = atoi(param_string->data[1]) - first;
    
    if (step <= 0)
	return NULL;

    int i;
    int contain = 0;
        
    for (i = 1; i < param_string->len; ++i) {
	if (strcmp(param_string->data[i], "...") == 0 && i + 1 != param_string->len){
	    contain = 1;
	    break;
	}
    }
    
    if (contain){
	int_arr* ret = malloc(sizeof(int_arr));
	int last = atof(param_string->data[param_string->len - 1]);
	
	
	ret->len = (last - first) / step + 1;
	ret->data = malloc(sizeof(int) * ret->len);
	
	for (i = 0; i < ret->len; ++i) {
	    ret->data[i] = first + i * step;
	}
	return ret;
    }

    return NULL;
}
