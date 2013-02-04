#include "experimenter.h"

static char* experiment_params[] = {"generator_params", "iwls_params",
				    "ols_params", "piecewise_params", 
				    "baseline_params", "test_generator", 
				    "test_ols", "test_iwls", "test_piecewise",
				    "test_baseline"};

static char* base_strings[] = {"generator", "iwls", "ols", "baseline", "piecewise"};

void run_experiments(char* exp_paramfile, char* def_paramfile)
{
    paramlist* exp_list = get_parameters(exp_paramfile);
//    paramlist* def_list = get_parameters(def_paramfile);

    if (!has_required_params(exp_list, experiment_params,\
			     sizeof(experiment_params)/sizeof(char*))){
	print_string_array("Some parameters required for experiments are missing. " \
			   "Ensure that your file contains the following parameters, and"\
			   " that you're using the right file. Pass the experiment parameters"\
			   " to -x, and the default ones to -p.",
			   experiment_params, sizeof(experiment_params)/sizeof(char*));
	exit(1);
    }

    int i;
    int missing = 0;
    
    
    for (i = 0; i < sizeof(base_strings)/sizeof(char*); ++i) {
	char teststr[30];
	char paramstr[30];

	sprintf(teststr, "test_%s", base_strings[i]);
	sprintf(paramstr, "%s_params", base_strings[i]);
		
	if (strcmp(get_string_param(exp_list, teststr), "yes") == 0){
	    printf("%s requested. Checking that experimental values are defined.\n", teststr);
	    string_arr* testparams = string_split(get_string_param(exp_list, paramstr), ',');
	
	    if (has_missing_parameters(testparams, exp_list)){
		missing = 1;
	    }
	}
    }

    if (missing){
	printf("Some parameters you were trying to experiment on were not defined"\
	       " in %s. Please add them and try again.\n", exp_paramfile);
	exit(1);
    }

    /* for (i = 0; i < exp_vars->len; ++i) { */
    /* 	double_arr* a = parse_param(exp_list, exp_vars->data[i]); */
    /* 	exp_var_data[i] = a; */
    /* } */
    
    
    /* if (strcmp(get_string_param(exp_list, "test_generator"), "yes") == 0){ */
    /* 	printf("testinggen\n"); */
    /* 	string_arr* gen_params = string_split(get_string_param(exp_list,"generator_params"), ','); */
	
    /* 	if (has_missing_parameters(gen_params, exp_list)){ */
    /* 	} */

    /* } */

    /* if (strcmp(get_string_param(exp_list, "test_ols"), "yes") == 0){ */
    /* 	printf("testingols\n"); */
    /* 	string_arr* ols_params = string_split(get_string_param(exp_list,"ols_params"), ','); */
    /* 		int i; */
	
    /* 	for (i = 0; i < ols_params->len; ++i) { */
    /* 	    printf("%s\n", ols_params->data[i]); */
    /* 	} */
    /* } */

    /* if (strcmp(get_string_param(exp_list, "test_iwls"), "yes") == 0){ */
    /* 	printf("testingiwls\n"); */
    /* 	string_arr* iwls_params = string_split(get_string_param(exp_list,"iwls_params"), ','); */
    /* 	int i; */
	
    /* 	for (i = 0; i < iwls_params->len; ++i) { */
    /* 	    printf("%s\n", iwls_params->data[i]); */
    /* 	} */

    /* } */

    /* if (strcmp(get_string_param(exp_list, "test_piecewise"), "yes") == 0){ */
    /* 	printf("testingpiece\n"); */
    /* 	string_arr* piece_params = string_split(get_string_param(exp_list,"piecewise_params"), ','); */
    /* 	int i; */
	
    /* 	for (i = 0; i < piece_params->len; ++i) { */
    /* 	    printf("%s\n", piece_params->data[i]); */
    /* 	} */
	
    /* } */

    /* if (strcmp(get_string_param(exp_list, "test_baseline"), "yes") == 0){ */
    /* 	printf("testingbase\n"); */
    /* 	string_arr* base_params = string_split(get_string_param(exp_list,"baseline_params"), ','); */
    /* 	int i; */
	
    /* 	for (i = 0; i < base_params->len; ++i) { */
    /* 	    printf("%s\n", base_params->data[i]); */
    /* 	} */
	
    /* } */

    
    
    
    /* int i; */
    /* double_arr** exp_var_data = malloc(sizeof(double_arr*) * exp_vars->len); */

    /* for (i = 0; i < exp_vars->len; ++i) { */
    /* 	double_arr* a = parse_param(exp_list, exp_vars->data[i]); */
    /* 	exp_var_data[i] = a; */
    /* } */

    /* int_arr* exp_counts = malloc(sizeof(int_arr)); */
    /* exp_counts->len = exp_vars->len; */
    /* exp_counts->data = calloc(exp_vars->len, sizeof(int)); */
    
    /* for (i = 0; i < exp_counts->len; ++i) { */
    /* 	printf("%d\n", exp_counts->data[i]); */
    /* } */
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
    double first = atof(param_string->data[0]);
    double step = atof(param_string->data[1]) - first;
    
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
	double_arr* ret = malloc(sizeof(double_arr));
	double last = atof(param_string->data[param_string->len - 1]);
	
	int len = (last - first) / step + 1;
	int special = 0;
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
