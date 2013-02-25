#ifndef _LAUNCHER_H
#define _LAUNCHER_H

#include "estimator.h"
#include "experimenter.h"
#include "generator.h"
#include "general_util.h"
#include "combinefunction.h"
#include "paramlist.h"
#include "file_util.h"
#include "defparams.h"

#define PROG_DESC "This program simulates photon arrival times using a poisson process."
#define OPT_INFO "OPTIONS\n\n"						\
    "Options related to estimation.\n\n"\
    "\t -e,  --estimate\n"						\
    "\t\tEstimate the underlying function from a given set of photon stream data."\
    "\n\t\tRequires parameter file.\n\n"					\
    "\t -a, --estimator\n"						\
    "\t\tThe estimation algorithm to use. Options are IWLS (iwls), OLS (ols),"	\
    " Piecewise (pc),\n\t\tBaseline (base). Used with the -e switch.\n\n"	\
    "Options related to generation.\n\n"\
    "\t -g, --generate\n"						\
    "\t\t Used to generate event streams or gaussian data.\n\n"       \
    "\t -r\n"\
    "\t\tUsed to generate random functions via gaussians or when used with the -i"\
    " switch,\n\t\tcreate gaussians with means centred on the points specified in the given"\
    " file,\n\t\twhich should contain one dimensional data.\n\n"\
    "\t -f\n"\
    "\t\tUsed to specify the function to use when generating event"\
    " streams.\n\t\trand will generate a random function using gaussians and"\
    " use that to\n\t\tgenerate events. When this switch is not passed, or mup"\
    " is given to it,\n\t\tit uses the expression specified in the parameter file"\
    " to generate.\n\n"\
    "Options related to experimentation.\n\n"\
    "\t -x, --experiment\n"						\
    "\t\t Run an experiment. Requires parameter file.\n\n"\
    "General options.\n\n"\
    "\t -c, --count\n"							\
    "\t\tSpecify the number of functions to generate data from. This can be used\n"\
    "\t\tto generate or estimate streams from multiple functions at the same time.\n"\
    "\t\tDefaults to 1.\n\n"\
    "\t -d, --defparam\n"						\
    "\t\t Creates a default parameter file with the given name\n\n"	\
    "\t -h, --help\n"							\
    "\t\t Display this message.\n\n"					\
    "\t -n, --nstreams\n"						\
    "\t\tUsed to specify how many streams to generate from each function. Defaults to 1.\n\n"\
    "\t -i, --infile\n"						\
    "\t\tSpecify the file to use as input to the subsystem. Usually specifies a file"\
    "\n\t\tcontaining event data.\n\n"								\
    "\t -o, --outfile\n"						\
    "\t\tData will be output to this file.\n\n"			\
    "\t -p or --paramfile\n"						\
    "\t\tSpecify the parameter file in which to find default parameters when experimenting.\n\n"\
    "\t -t, --outtype\n"						\
    "\t\tUsed to specify how much data to ouput to file when estimating or generating."\
    "\n\t\tA value of zero will output no files at all. 1 will output the most"\
    "\n\t\tuseful data for the specific task, and anything above that will output more specific"\
    "\n\t\tinformation. Each level outputs its own data and anything from levels below it."\
    "\n\n\t\tBasic stream generation (each level outputs only that data):"\
    "\n\t\t\t1 - Events only."\
    "\n\t\t\t2 - Events and lambda values."\
    "\n\t\t\t3 - Bin counts only."\
    "\n\t\t\t4 - Events, lambda values to one file, bin counts to another."\
    "\n\n\t\tGaussian function generation:"		     \
    "\n\t\t\t1 - mean, stdev, weight of gaussians."	\
    "\n\t\t\t2 - sum of gaussians at points along x."		\
    "\n\t\t\t3 - contribution of individual gaussians.\n\n"	\
    "\t\tDelta Estimation:"						\
    "\n\t\t\t1 - value computed by the function for each guess at delta" \
    "\n\t\t\t2 - lambda sums at best estimate, bin data.\n\n"
#define VERSION "deltastream alpha v0.6"
#define BUGREPORT "Report bugs to mxs968@cs.bham.ac.uk"

void run_requested_operations(launcher_args* args, char* paramfile, char* extra_paramfile,
			      char* infile, char* outfile, char* estimator_type,
			      char* generator_type);
int exists_in_arr(char** arr, int len, char* name);
launcher_args* make_arg_struct();

#endif
