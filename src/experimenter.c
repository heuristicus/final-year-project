#include "experimenter.h"

void print_exp_set(exp_set* set);
void roll_experiment(exp_tuple_arr* exps, int level);
int increment_experiment(exp_tuple_arr* exp);
void update_parameters(exp_tuple_arr* exp, paramlist* params);
int parameters_coherent(paramlist* experiment, paramlist* def, char** check,
			int check_len);
exp_set* experiment_setup(paramlist* exp_params, paramlist* def_params);
void execute_experiments(paramlist* exp_params, paramlist* def_params,
			 char* in_dir, char* out_dir, exp_set* experiments,
			 int num_streams, int num_functions,
			 int output_switch, int rfunc);
void analyse_multi(char* outfile, char* in_dir, paramlist* params,
		   tdelta_result** results, int num_streams, int num_functions,
		   double_arr* time_delays, int rfunc);
void _stutter_stream(char* outfile, double_arr* events, double_arr* intervals);
double compare_stuttered_bins(paramlist* def_list, paramlist* exp_list, char* infile,
			    void* estimate, int gauss, char* est_type, double_arr* stutter_intervals);
double_arr* compute_stutter_intervals(double stutter_step, double stutter_interval,
				      double start_time, double interval_time);

void run_experiments(char* exp_paramfile, char* def_paramfile, char* indir,
		     char* outdir, int num_streams, int num_functions,
		     int output_switch, int rfunc)
{
    paramlist* exp_list = get_parameters(exp_paramfile);
    paramlist* def_list = get_parameters(def_paramfile);

    if (num_streams == 1 && num_functions == 1){
	num_streams = get_int_param(exp_list, "num_streams");
	num_functions = get_int_param(exp_list, "num_functions");
    }
    printf("num streams is %d, funcs is %d\n", num_streams, num_functions);

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
	/* printf("The directory already exists. Proceeding may overwrite all data inside it."\ */
	/*        "Continue? (y(es) or n(o))\n"); */
	/* char read = getchar(); */
	/* if (read == 'y'){ */
	/*     printf("Continuing...\n"); */
	/* } else { */
	/*     printf("Aborted.\n"); */
	/*     exit(1); */
	/* } */
    }

    exp_set* experiments = experiment_setup(exp_list, def_list);
    execute_experiments(exp_list, def_list, indir, outdir, experiments,
			num_streams, num_functions, output_switch, rfunc);

    free_list(exp_list);
    free_list(def_list);
    free_exp_set(experiments);
}

exp_set* experiment_setup(paramlist* exp_list, paramlist* def_list)
{
    char* ename = get_string_param(exp_list, "experiment_names");
    if (ename == NULL){
	printf("No parameter experiment_names specified. Please add this to"\
	       " the parameter file and define some experiments\n");
	exit(1);
    }
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
		printf("Some parameters you were trying to experiment on were not defined" \
	       " in one of the parameter files. Please add them and try again.\n");
		exit(1);

	    } else {
	    	printf("OK\n");
	    }
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
void execute_experiments(paramlist* exp_list, paramlist* def_list, char* in_dir,
			 char* out_dir, exp_set* experiments, int num_streams,
			 int num_functions, int output_switch, int rfunc)
{
    int i, j, k;
    int expcount = 0;
    char* tmp = NULL;
    int multiple = 0; // Are we estimating the time delay or just a single stream (function)
    char* fname = get_string_param(def_list, "outfile"); // default generator output filename
    char* pref = get_string_param(def_list, "stream_ext"); // default extension
    int stuttered = strcmp(get_string_param(exp_list, "run_stuttered"), "yes") == 0;
    double_arr* time_delays = NULL;

    char* function_fname;

    if (rfunc){
	function_fname = get_string_param(def_list, "function_outfile");
    } else {
	function_fname = get_string_param(def_list, "expression_outfile");
    }

    
    // Go through all of the experiments that are in the set received
    for (i = 0; i < experiments->len; ++i) {
	printf("Running experiments for %s\n", experiments->exp_names[i]);
	if (experiments->exps[i] == NULL){
	    printf("No experiment on %s\n", experiments->exp_names[i]);
	    continue;
	}

	// Some parameters we need to read from the paramfile vary depending on the
	// estimator name, so use this to get them.
	tmp = malloc(strlen(experiments->exp_names[i]) + strlen("_estimator") + 10);
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
	
	if (multiple){
	    // Use this later to compare estimated values to true values
	    time_delays = get_double_list_param(exp_list, "timedelta");
	}

	// Read the estimator type to use from the parameter file
	sprintf(tmp, "%s_estimator", experiments->exp_names[i]);
	char* est_type = get_string_param(exp_list, tmp);
	int gauss = strcmp(est_type, "gauss") == 0;
	printf("Using estimator %s\n", est_type);
	// Store the reference to the current experiment to reduce clutter.
	exp_tuple_arr* current_exp = experiments->exps[i];
	
	// This is the top level directory to which we will output data from
	// this set of experiments
	char* output_directory = malloc(strlen(experiments->exp_names[i]) + strlen(out_dir) + strlen("experiment") + 10);
	sprintf(output_directory, "%s/%s", out_dir, experiments->exp_names[i]);
	if (!create_dir(output_directory)){
	    printf("Something went wrong when creating directory %s.\n", output_directory);
	    perror("Error");
	    exit(1);
	}
	printf("output directory is %s\n", output_directory);
	
	// The way we do experiments depends on whether the parameters are to be
	// co-varied - we split here.
	if (strcmp(get_string_param(exp_list, "run_separately"), "yes") == 0){
	    for (j = 0; j < current_exp->num_params; ++j) {
//		printf("Parameter %d: %s\nValues:\n", j, current_exp->data[j]->param_name);
		for (k = 0; k < current_exp->data[j]->param_vals->len; ++k) {
//		    printf("%lf\n", current_exp->data[j]->param_vals->data[k]);
		    expcount++;
		}
	    }
	} else {
	    int sepcount = 0; // separate count for each experiment
	    
	    // This is the directory in which data for an experiment with the same set of parameters.
	    char* experiment_directory = malloc(strlen(output_directory) + \
						strlen("experiment_") + 8);
	    double_arr* stutter_intervals = NULL;
	    if (!multiple){
		if (strcmp(get_string_param(exp_list, "uniform_stuttering"), "yes") == 0){
		    double start = get_double_param(def_list, "est_start_time");
		    double end = start + get_double_param(def_list, "est_interval_time");
		    
		    stutter_intervals = compute_stutter_intervals(get_double_param(exp_list, "stutter_step"),
					      get_double_param(exp_list, "stutter_interval"),
					      start, end);
		} else {
		    stutter_intervals = get_double_list_param(exp_list, "stutter_intervals");
		}
	    }
	    
	    // Use this to store the best goodness value when working with stuttered data
	    int best_goodness_exp = -1;
	    double best_goodness_value = -INFINITY;
	    FILE *fp1 = NULL;
	    double_multi_arr* function_bests = NULL;
	    if (stuttered){
	    	char* infname = malloc(strlen(output_directory) + strlen("avg_goodness.txt") + 10);
	    	sprintf(infname, "%s/avg_goodness.txt", output_directory);
	    	fp1 = fopen(infname, "w");
	    	free(infname);

		// In this array we will store the current best values for each individual function
		// The first array is the best experiment number, the second the goodness value for
		// that experiment
		function_bests = init_multi_array(2, num_functions);
		int i;
		
		for (i = 0; i < function_bests->lengths[0]; ++i) {
		    function_bests->data[0][i] = -1;
		    function_bests->data[1][i] = -INFINITY;
		}

	    }

	    char* tname = malloc(strlen(output_directory) + strlen("exp_params.txt") + 5);
	    sprintf(tname, "%s/exp_params.txt", output_directory);
	    list_to_file(tname, "w", exp_list);
	    free(tname);

	    /* double_arr** exp_delays = NULL; */
	    /* if (multiple){ */
	    /* 	exp_delays = malloc(sizeof(double_arr*) * num_functions); */
	    /* } */
	    
	    // Go through all parameter combinations, running the estimator with each combination
	    do {
		// Update the experiment directory with the current experiment number, and create it.
		sprintf(experiment_directory, "%s/experiment_%d", output_directory, sepcount);
		if (!create_dir(experiment_directory)){
		    printf("Something went wrong when attempting to create directory %s.\n",
			   experiment_directory);
		    perror("Error");
		    exit(1);
		}
		char* output_file = malloc(strlen(experiment_directory) + 5 + strlen("experiment_"));
			    
		// Set the parameters in the parameter list to those which are to be
		// used for the current experiment
		update_parameters(current_exp, def_list);
		// Output the parameter settings to the experiment directory so
		// that they can be checked later.
		sprintf(output_file, "%s/%s", experiment_directory, "parameters.txt");
		list_to_file(output_file, "w", def_list);
		// The data is output to this file - prepend the experiment directory location
		sprintf(output_file, "%s/%s", experiment_directory, "exp");
		if (multiple){
		    printf("Estimating time delay.\n");
		    tdelta_result** results = _multi_estimate(def_list, in_dir,
							      output_file, num_streams,
							      num_functions, 1, est_type,
							      stuttered, rfunc);
		    sprintf(output_file, "%s/results.txt", experiment_directory);
		    // Analyse the results and output them to a file in the experiment directory
		    analyse_multi(output_file, in_dir, def_list, results, num_streams,
				  num_functions, time_delays, rfunc);
		    int i;
		    
		    for (i = 0; i < num_functions; ++i) {
			free_tdelta_result(results[i]);
		    }

		    free(results);
		} else {
		    printf("Estimating functions.\n");
		    
		    char* infname = malloc(strlen(in_dir) + strlen(function_fname) +
					   strlen(fname) + strlen(pref) + strlen(".dat") +
					   strlen(experiment_directory) + 
					   strlen("goodness.txt") + 10);
		    int i;
		
		    FILE *fp;

		    double total_goodness = 0;

		    // Perform the experiment once for each function in the set.
		    if (stuttered){
			sprintf(infname, "%s/goodness.txt", experiment_directory);
			fp = fopen(infname, "w");
			fprintf(fp, "experiment_%d\n", expcount);
			
		    }
		    for (i = 0; i < num_functions; ++i) {
			// Filenames are different if the data is stuttered
			sprintf(output_file, "%s/exp_func_%d", experiment_directory, i);
			if (stuttered)
			    sprintf(infname, "%s/%s_%d_%s%s%d_stuttered.dat", in_dir, function_fname, i, fname, pref, 0);
			else
			    sprintf(infname, "%s/%s_%d_%s%s%d.dat", in_dir, function_fname, i, fname, pref, 0);

			void* est_res = _estimate(def_list, infname, output_file, est_type, output_switch, rfunc, 1);

			sprintf(infname, "%s/%s_%d_%s%s%d", in_dir, function_fname, i, fname, pref, 0);
			if (stuttered){
			    double goodness = compare_stuttered_bins(def_list, exp_list, infname, est_res, gauss, est_type, stutter_intervals);
			    if (goodness > function_bests->data[1][i]){
				function_bests->data[0][i] = sepcount;
				function_bests->data[1][i] = goodness;
			    }
			    total_goodness += goodness;
			    printf("Goodness value for this parameter set is %lf\n", goodness);
			    fprintf(fp, "f%d: %lf\n", i, goodness);
			}
			
			if (gauss){
			    free_gauss_vector(est_res);
			} else {
			    free_est_arr(est_res);
			}
		    }
		    if (stuttered){
			double avg_goodness = total_goodness/num_functions;
			if (avg_goodness > best_goodness_value){
			    best_goodness_value = avg_goodness;
			    best_goodness_exp = sepcount;
			}

			printf("Average goodness for experiment %d: %lf\n", sepcount, avg_goodness);
			fprintf(fp, "avg: %lf\n", avg_goodness);
			fprintf(fp1, "%d %lf\n", sepcount, avg_goodness);
			fclose(fp);
		    }
		
		    free(infname);
		}
		printf("Exp %d complete\n", expcount);
		expcount++;
		sepcount++;
		free(output_file);
	    } while (increment_experiment(current_exp));
	    printf("Completed %d experiments for %s.\n", sepcount, experiments->exp_names[i]);
	    sepcount = 0;
	    free(experiment_directory);
	    if (stuttered){	    	
		char* infname = malloc(strlen(output_directory) + strlen("function_bests.txt") + 10);
		sprintf(infname, "%s/function_bests.txt", output_directory);
		FILE* fp2 = fopen(infname, "w");
		int i;

		for (i = 0; i < function_bests->lengths[0]; ++i) {
		    fprintf(fp2, "%d %d %lf\n", i, (int)function_bests->data[0][i], function_bests->data[1][i]);
		}

		fclose(fp2);
			
		printf("best goodness: %lf from experiment %d\n", best_goodness_value, best_goodness_exp);
		fprintf(fp1, "\n\nbest: %lf from experiment %d\n", best_goodness_value, best_goodness_exp);
		fclose(fp1);
		free_double_multi_arr(function_bests);
	    }

	    if (!multiple)
		free_double_arr(stutter_intervals);
	}
	free(output_directory);
    }
	
    if (multiple){
	free_double_arr(time_delays);
    }
    
    free(tmp);
    
    printf("total experiments: %d on %d functions = %d\n", expcount, num_functions, expcount * num_functions);
}

/*
 * Compares the actual bin counts in the stuttered intervals (all events deleted) to the
 * lambda value estimated by the function. For each bin, the closer the function is to the
 * actual bin count, the higher the pdf is for that bin. The log probability over all bins is
 * returned, and this can be used to decide how well the parameter setting generalises on
 * functions.
 */
double compare_stuttered_bins(paramlist* def_list, paramlist* exp_list, char* infile,
			    void* estimate, int gauss, char* est_type,
			    double_arr* stutter_intervals)
{
    double start = get_double_param(def_list, "est_start_time");
    double interval = get_double_param(def_list, "est_interval_time");
    double end = interval + start;
    int num_subintervals = 1;
    double normaliser = 1;
    
    if (strcmp(est_type, "ols") == 0){
	num_subintervals = get_int_param(def_list, "ols_subintervals");
    } else if (strcmp(est_type, "iwls") == 0){
	num_subintervals = get_int_param(def_list, "iwls_subintervals");
    } else if (strcmp(est_type, "gauss") != 0){
	char* prname = malloc(strlen("_iwls_subintervals") + 10);
	sprintf(prname, "%s_iwls_subintervals", est_type);
	num_subintervals = get_int_param(def_list, prname);
	sprintf(prname, "%s_max_breakpoints", est_type);
	num_subintervals *= get_int_param(def_list, prname);
	free(prname);
    }
    
    char* infname = malloc(strlen(infile) + strlen("stuttered.dat") + 5);
    sprintf(infname, "%s_stuttered.dat", infile);
    double_arr* st_events = get_event_data_all(infname);
    int* st_bins = sum_events_in_interval(st_events->data, st_events->len, start, end, (int)interval);
    sprintf(infname, "%s.dat", infile);
    double_arr* events = get_event_data_all(infname);
    int* bins = sum_events_in_interval(events->data, events->len, start, end, (int)interval);
    double* midpoints = get_interval_midpoints(start, end, (int)interval);

    
    int i;
    double* sums = malloc(sizeof(double) * (int)interval);
    int interval_num = 0;
    double pdf_sum = 0;
    if (gauss){
	normaliser = find_normaliser(def_list, estimate, events, est_type);
    }
    
    for (i = 0; i < (int)interval; ++i) {
	//		printf("midpoint %lf, Stuttered: %d, normal: %d\n", midpoints[i], st_bins[i], bins[i]);
	if (gauss)
	    sums[i] = sum_gaussians_at_point(midpoints[i], estimate);
	else
	    sums[i] = estimate_at_point(estimate, midpoints[i]);
	//		printf("sum is %lf\n", sums[i]);
	if (midpoints[i] > stutter_intervals->data[interval_num * 2] 
	    && midpoints[i] < stutter_intervals->data[interval_num * 2 + 1]){
	    //	    	    printf("midpoint %d (%lf) is inside a stutter interval\n", i, midpoints[i]);
	    double thispdf = log_pdf(bins[i], sums[i], normaliser);
	    pdf_sum += thispdf;
	    //	    	    printf("bin: %d, sum %lf. logpmf is %lf\n", bins[i], sums[i], thispdf);
	} else if (midpoints[i] > stutter_intervals->data[interval_num * 2 + 1]){
	    interval_num++;
	    if (interval_num >= stutter_intervals->len/2)
		break;
	}
    }

    //    printf("num subintervals is %d\n", num_subintervals);
    free(infname);
    free_double_arr(st_events);
    free_double_arr(events);
    free(midpoints);
    free(bins);
    free(st_bins);
    free(sums);

    return pdf_sum;
}
    
/*
 * Analyses time delta experiment results and outputs them to a file.
 */
void analyse_multi(char* outfile, char* in_dir, paramlist* params,
		   tdelta_result** results, int num_streams, int num_functions, 
		   double_arr* time_delays, int rfunc)
{
    int i, j;
    FILE *fp = fopen(outfile, "w");
    // Store estimated delta values so that the set of estimates for the delay of each stream are grouped
    double_multi_arr* est_deltas = init_multi_array(num_streams, num_functions);
    double_multi_arr* est_errors = init_multi_array(num_streams, num_functions);
    for (i = 0; i < num_functions; ++i) {
//	fprintf(fp, "Time deltas for function %d:\n", i);
			
	for (j = 0; j < num_streams; ++j) {
	    /* fprintf(fp, "Stream %d\n", j); */
	    /* fprintf(fp, "Estimated: %lf\n", results[i]->delays->data[j]); */
	    /* fprintf(fp, "Actual: %lf\n", time_delays->data[j]); */
	    est_deltas->data[j][i] = results[i]->delays->data[j];
	    est_errors->data[j][i] = results[i]->delays->data[j] - time_delays->data[j];
	}

	/* char* function_fname = NULL; */
	/* if (rfunc){ */
	/*     function_fname = get_string_param(params, "function_outfile"); */
	/* } else { */
	/*     function_fname = get_string_param(params, "expression_outfile"); */
	/* } */
	/* char* orig_name = malloc(strlen(in_dir) + strlen(function_fname) + strlen(".dat") + 5); */
	/* sprintf(orig_name, "%s/%s_%d.dat", in_dir, function_fname, i); */
	/* gauss_vector* orig = read_gauss_vector(orig_name); */
			
	/* fprintf(fp, "Function RSS: %lf\n", get_twofunction_RSS(orig, results[i]->final_estimate)); */
	/* fprintf(fp, "Function TSS: %lf\n", get_twofunction_TSS(orig, results[i]->final_estimate)); */
	/* fprintf(fp, "Function ESS: %lf\n", get_twofunction_ESS(orig, results[i]->final_estimate)); */
	/* fprintf(fp, "Function RMS: %lf\n", get_twofunction_RMS(orig, results[i]->final_estimate)); */

	/* fprintf(fp, "\n\n"); */
    }

//    fprintf(fp, "\n\n");
    for (i = 0; i < num_streams; ++i) {
	fprintf(fp, "Stream %d\n", i);
	fprintf(fp, "Actual: %lf\n", time_delays->data[i]);
	fprintf(fp, "Mean est: %lf\n", avg(est_deltas->data[i], num_functions));
	fprintf(fp, "Est stdev: %lf\n", stdev(est_deltas->data[i], num_functions));
	fprintf(fp, "Mean error: %lf\n", avg(est_errors->data[i], num_functions));
	fprintf(fp, "Error stdev: %lf\n", stdev(est_errors->data[i], num_functions));
    }
    fprintf(fp, "\n\n");
    

    
    fprintf(fp, "# Estimate delta values (Each line is stream data for a single function.):\n");
    fclose(fp);
    output_double_multi_arr_numbered(outfile, "a", est_deltas);

    fp = fopen(outfile, "a");
    fprintf(fp, "# Estimate errors (Each line is stream data for a single function):\n");
    fclose(fp);
    output_double_multi_arr_numbered(outfile, "a", est_errors);


    free_double_multi_arr(est_deltas);
    free_double_multi_arr(est_errors);
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
 * Removes data in intervals specified in the parameter file from data files in the
 * given input directory, and creates new files in the directory containing this
 * stuttered data. This function performs some setup operations.
 */
void stutter_stream(char* indir, char* exp_paramfile, char* def_paramfile, int nfuncs, int nstreams, int rfunc)
{    
    paramlist* def_params = get_parameters(def_paramfile);
    paramlist* exp_params = get_parameters(exp_paramfile);
    
    char* fname = get_string_param(def_params, "outfile"); // default generator output filename
    char* pref = get_string_param(def_params, "stream_ext"); // default extension
    char* function_fname;
    if (rfunc){
	function_fname = get_string_param(def_params, "function_outfile");
    } else {
	function_fname = get_string_param(def_params, "expression_outfile");
    }

    printf("rfunc %d, fname %s\n", rfunc, function_fname);

    double start_time = get_double_param(def_params, "start_time");
    double interval_time = get_double_param(def_params, "interval_time");
    int uniform = strcmp(get_string_param(exp_params, "uniform_stuttering"), "yes") == 0;
    double step = 0;
    double interval = 0;
    double_arr* stutter_intervals = NULL;
    
    if (!uniform){
	stutter_intervals = get_double_list_param(exp_params, "stutter_intervals");
	if (stutter_intervals->len % 2 != 0){
	    printf("stutter_intervals must have an even number of elements.\n");
	}

	int i,j;
	int error = 0;
	
	for (i = 0; i < stutter_intervals->len; ++i) {
	    for (j = 0; j < stutter_intervals->len; ++j) {
		if (j < i && stutter_intervals->data[i] < stutter_intervals->data[j]) {
		    printf("stutter_intervals must be monotonically increasing.\n"\
			   "Index %d (%lf) is greater than index %d (%lf). Please fix this.\n",
			   j, stutter_intervals->data[j], i, stutter_intervals->data[i]);
		    error = 1;
		}
	    }
	}
	if (error){
	    printf("stutter_intervals is non-monotonic. Exiting.\n");
	    exit(1);
	}
    } else {
	step = get_double_param(exp_params, "stutter_step");
	interval = get_double_param(exp_params, "stutter_interval");
    }
    
    if (indir == NULL){
	indir = get_string_param(exp_params, "input_dir");
    }

    printf("Stuttering %d streams for each of %d functions in directory %s\n", nstreams, nfuncs, indir);

    char* outname = malloc(strlen(indir) + strlen(function_fname)
    			   + strlen(fname) + strlen(pref)
    			   + strlen("stuttered") + strlen(".dat") + 10);

    char* infname =  malloc(strlen(indir) + strlen(function_fname)
			    + strlen(fname) + strlen(pref)
			    + strlen(".dat") + 10);
    
    int i, j;
    double_arr* events = NULL;
    
    for (i = 0; i < nfuncs; ++i) {
	for (j = 0; j < nstreams; ++j) {
	    sprintf(infname, "%s/%s_%d_%s%s%d.dat", indir, function_fname, 
		    i, fname, pref, j);
	    sprintf(outname, "%s/%s_%d_%s%s%d_stuttered.dat", indir, function_fname,
		    i, fname, pref, j);
	    events = get_event_data_all(infname);

	    if (uniform)
		stutter_intervals = compute_stutter_intervals(step, interval, start_time, interval_time);

	    _stutter_stream(outname, events, stutter_intervals);

	    if (uniform)
		free_double_arr(stutter_intervals);

	    free_double_arr(events);
	}
    }
    free(infname);
    free(outname);
    free_list(exp_params);
    free_list(def_params);
}

/*
 * Computes the intervals in which event data should be deleted. The stutter
 * interval is the length of the interval in which data is deleted, and the
 * interval time is the total length of the interval in which event generation
 * took place.
 */
double_arr* compute_stutter_intervals(double stutter_step, double stutter_interval,
				      double start_time, double interval_time)
{
    int len = (int)(interval_time/stutter_step);
    double_arr* ret = init_double_arr(len * 2);
    double end = start_time + interval_time;
    double interval_start = start_time + stutter_step;
    int interval_num = 1, i = 0;
    
    while (interval_start * interval_num < end) {
	ret->data[i * 2] = interval_start * interval_num;
	ret->data[i * 2 + 1] = interval_start * interval_num + stutter_interval;
	interval_num++;
	i++;
    }

    return ret;
}

/*
 * Function for reading stream data and producing stuttered data files, where stuttering
 * is done based on the data provided by the stutter_intervals parameter.
 */
void _stutter_stream(char* outfile, double_arr* events, double_arr* intervals)
{
    int interval_num = 0;
    int interval_count = 0;
    int i;
    FILE *fp = fopen(outfile, "w");

    //    printf("start %lf, end %lf\n", intervals->data[interval_num * 2], intervals->data[interval_num * 2 + 1]);
    for (i = 0; i < events->len; ++i) {
	if (interval_num == intervals->len/2) {
	    fprintf(fp, "%lf\n", events->data[i]);
	    continue;
	} else if (events->data[i] >= intervals->data[interval_num * 2] 
	    && events->data[i] < intervals->data[interval_num * 2 + 1]){
	    interval_count++;
//	    printf("inside interval: %lf\n", events->data[i]);
	    // The current event is inside one of the intervals where we delete data.
	    continue;
	} else if (events->data[i] > intervals->data[interval_num * 2 + 1]){
	    //	    printf("Events deleted: %d\n", interval_count);
	    interval_count = 0;
	    //	    printf("Event outside interval: %lf, Moving to interval %d\n", events->data[i], interval_num + 1);
	    // The interval was passed, so move to the next one.
	    interval_num++;
//	    printf("start %lf, end %lf\n", intervals->data[interval_num * 2], intervals->data[interval_num * 2 + 1]);
	}
	fprintf(fp, "%lf\n", events->data[i]);
    }
    fclose(fp);
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
	    ret = parse_double_range(splitparam);
	    free_string_arr(splitparam);
	    return ret;
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
