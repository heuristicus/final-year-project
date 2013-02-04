#ifndef _EXPERIMENTER_H
#define _EXPERIMENTER_H

#include "general_util.h"
#include "generator.h"
#include "estimator.h"
#include "file_util.h"
#include "defparams.h"
#include "paramlist.h"
#include "general_util.h"

typedef struct
{
    char* subsystem_name;
    string_arr* param_names;
    double_arr** param_values;
} experiment_data;

void run_experiments(char* exp_params, char* def_params);
double_arr* parse_double_range(string_arr* split_param_string);
int_arr* parse_int_range(string_arr* split_param_string);
double_arr* parse_param(paramlist* params, char* param_to_parse);

#endif
