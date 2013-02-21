#include "experimenter.h"

static char* experiment_params[] = {"generator_params", "iwls_params",
				    "ols_params", "piecewise_params", 
				    "baseline_params", "test_generator", 
				    "test_ols", "test_iwls", "test_piecewise",
				    "test_baseline", "test_gaussian"};

static char* base_strings[] = {"generator", "iwls", "ols", "baseline", "piecewise", "gaussian"};

typedef struct
{
    char* param_name;
    double_arr* param_vals;
} exp_tuple;

typedef struct
{
    exp_tuple** data;
    int len;
} exp_tuple_arr;

typedef struct
{
    exp_tuple_arr** exps;
    char** exp_names;
    int len;
} exp_set;

void print_exp_set(exp_set* set);

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

    int i,j;
    int missing = 0;
    int req = 0;
    int baselen = sizeof(base_strings)/sizeof(char*);

    exp_set* experiment_values = malloc(sizeof(exp_set));
    experiment_values->exp_names = malloc(baselen * sizeof(char*));
    experiment_values->exps = malloc(baselen * sizeof(exp_tuple_arr*));
    experiment_values->len = baselen;

    for (i = 0; i < baselen; ++i) {
	char teststr[30];
	char paramstr[30];

	sprintf(teststr, "test_%s", base_strings[i]);
	sprintf(paramstr, "%s_params", base_strings[i]);
	experiment_values->exp_names[i] = strdup(teststr);
		
	if (strcmp(get_string_param(exp_list, teststr), "yes") == 0){
	    printf("%s requested. Checking that experimental values are defined.\n", teststr);
	    char* p = get_string_param(exp_list, paramstr);
	    if (p == NULL){
		printf("Expected parameter %s not defined. Skipping.\n", paramstr);
		continue;
	    }

	    string_arr* testparams = string_split(p, ',');

	    if (!has_required_params(exp_list, testparams->data, testparams->len)){
	    	missing = 1;
	    } else {
	    	printf("OK\n");
	    }

	    experiment_values->exps[i] = malloc(sizeof(exp_tuple_arr));
	    experiment_values->exps[i]->data = malloc(sizeof(exp_tuple*) * testparams->len);
	    experiment_values->exps[i]->len = testparams->len;
	    
	    for (j = 0; j < testparams->len; ++j) {
		exp_tuple* etup = malloc(sizeof(exp_tuple));
		etup->param_name = testparams->data[j];
		etup->param_vals = parse_param(exp_list, etup->param_name);
		experiment_values->exps[i]->data[j] = etup;
	    }
	    req = 1;
	} else {
	    experiment_values->exps[i] = NULL;
	}
    }

    if (missing){
	printf("Some parameters you were trying to experiment on were not defined"\
	       " in %s. Please add them and try again.\n", exp_paramfile);
	exit(1);
    }
    if (req == 0){
	printf("No experiments requested. Exiting.\n");
	exit(1);
    }

    print_exp_set(experiment_values);
}

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
	for (j = 0; j < set->exps[i]->len; ++j) {
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
