#ifndef _EXPERIMENTER_H
#define _EXPERIMENTER_H

#include "general_util.h"
#include "generator.h"
#include "estimator.h"
#include "file_util.h"
#include "defparams.h"
#include "paramlist.h"
#include "general_util.h"

void experiment(char* paramfile);
double_arr* parse_double_range(char** split_param_string);
int_arr* parse_int_range(char** split_param_string);

#endif
