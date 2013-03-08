#ifndef _EXPERIMENTER_H
#define _EXPERIMENTER_H

#include "general_util.h"
#include "generator.h"
#include "estimator.h"
#include "file_util.h"
#include "defparams.h"
#include "paramlist.h"
#include "general_util.h"

void run_experiments(char* exp_params, char* def_params, char* indir, char* outdir,
		     int num_streams, int num_functions, int output_switch, int rfunc);
double_arr* parse_double_range(string_arr* split_param_string);
int_arr* parse_int_range(string_arr* split_param_string);
double_arr* parse_param(paramlist* params, char* param_to_parse);
void stutter_stream(char* indir, char* exp_paramfile, char* def_paramfile, int nfuncs, int nstreams);

#endif
