#ifndef _LAUNCHER_H
#define _LAUNCHER_H

#include "estimator.h"
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
    "\t\t Estimate the underlying function from a given set of photon stream data."\
    " Requires parameter file.\n\n"					\
    "\t -a, --estimator\n"						\
    "\t\t The estimation algorithm to use. Options are IWLS (iwls), OLS (ols),"	\
    " Piecewise (pc), Baseline (base). Used with the -e switch.\n\n"	\
    "Options related to generation.\n\n"\
    "\t -g, --generate\n"						\
    "\t\t Used to generate event streams or gaussian data.\n\n"       \
    "\t -t, --outtype\n"\
    "\t\t Used to specify the output form of generated gaussians.\nWith a value of 0, "\
    "the mean, standard deviation and weight of gaussians will"\
    " be written to file. \n1 will output the contribution of the gaussians at points"\
    " along the x-axis.\n2 will output the contribution of gaussians in one file"\
    " and the data for each individial gaussian in another.\n\n"	\
    "\t -r\n"\
    "\t\t Used to generate random functions via gaussians or when used with the -i"\
    " switch, create gaussians with means centred on the points specified in the given"\
    " file, which should contain one dimensional data.\n\n"\
    "\t -f\n"\
    "\t\t Used to specify the function to use when generating event"\
    " streams. rand will generate a random function using gaussians and"\
    " use that to generate events. When this switch is not passed, or mup"\
    " is given to it, it uses the expression specified in the parameter file"\
    " to generate.\n\n"\
    "Options related to experimentation.\n\n"\
    "\t -x, --experiment\n"						\
    "\t\t Run an experiment. Requires parameter file.\n\n"\
    "General options.\n\n"\
    "\t -d, --defparam\n"						\
    "\t\t Creates a default parameter file with the given name\n\n"	\
    "\t -h, --help\n"							\
    "\t\t Display this message.\n\n"					\
    "\t -n, --nstreams\n"						\
    "\t\t Number of times to run generation. Use this to generate multiple streams."\
    " Also specifies how many functions to estimate when using the -e switch..\n\n"\
    "\t -i, --infile\n"						\
    "\t\t Specify the file to use as input to the subsystem. Usually specifies a file"\
    "containing event data.\n\n"								\
    "\t -o, --outfile\n"						\
    "\t\t Data will be output to this file.\n\n"			\
    "\t -p, --paramfile\n"						\
    "\t\t The file containing parameters to use. This can be used to specify a "\
    "large number of options.\n\n"
#define VERSION "poissonstream alpha v0.5"
#define BUGREPORT "Report bugs to mxs968@cs.bham.ac.uk"

void run_requested_operations(launcher_args* args, char* paramfile, char* infile,
			      char* outfile, char* estimator_type, char* generator_type);
int exists_in_arr(char** arr, int len, char* name);
void multi_estimate(char* paramfile, char* infile, char* outfile, char* estimator_type, int nstreams);
void multi_est_gauss(paramlist* params, char* infile, char* outfile, int nstreams);
void multi_est_default(char* paramfile, char* infile, char* outfile, char* estimator_type, int nstreams);
launcher_args* make_arg_struct();

#endif
