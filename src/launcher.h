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

#define PROG_DESC "This program simulates photon arrival times using a poisson process,\n"\
    "and can be used to estimate functions from these realisation, as well as the time delay\n"\
    "between them."
#define OPT_INFO "OPTIONS\n\n"						\
    "Options related to estimation.\n\n"\
    "\t -e,  --estimate\n"						\
    "\t\t Estimate the underlying function from a given set of photon stream data.\n"\
    "\t\t Requires parameter file.\n\n"					\
    "\t -a, --estimator\n"						\
    "\t\t The estimation algorithm to use. Options are IWLS (iwls), OLS (ols),\n"\
    "\t\t Piecewise (pc), Baseline (base). Used with the -e switch.\n\n" \
    "Options related to generation.\n\n"\
    "\t -g, --generate\n"						\
    "\t\t Used to generate event streams or gaussian data.\n\n"       \
    "\t -f\n"\
    "\t\t Used to specify the function to use when generating event streams.\n"\
    "\t\t rand will generate a random function using gaussians and use that to\n"\
    "\t\t generate events. When this switch is not passed, or mup is given to it,\n"\
    "\t\t it uses the expression specified in the parameter file to generate.\n\n"\
    "\t -u --duplicate\n"\
    "\t\t If you want to generate multiple sets of streams from the same function, set\n"\
    "\t\t this switch when using the gaussian random generator (-f rand). The default\n"\
    "\t\t generator has this behaviour automatically. Not passing this switch will generate\n"\
    "\t\t the number of distinct functions specified by the -c switch.\n\n"\
    "Options related to experimentation.\n\n"\
    "\t -x, --experiment\n"						\
    "\t\t Run an experiment. Requires parameter file.\n\n"\
    "\t -s, --stutter\n"						\
    "\t\t Modifies streams in the input directory passed to -i, or specified by\n"\
    "\t\t the input_dir parameter in the parameter file so that event data in some\n"\
    "\t\t intervals is removed.\n\n"									\
    "General options.\n\n"\
    "\t -c, --count\n"							\
    "\t\t Specify the number of functions to generate data from. This can be used\n"\
    "\t\t to generate or estimate streams from multiple functions at the same time.\n"\
    "\t\t Defaults to 1.\n\n"\
    "\t -d, --defparam\n"						\
    "\t\t Creates a default parameter file with the given name. To create a\n"\
    "\t\t default experiment parameter file, use something like\n"\
    "\t\t ./launcher -d paramfile -x a\n"				\
    "\t\t The -x switch must take an arbitrary parameter, which will not be used.\n\n"\
    "\t -h, --help\n"							\
    "\t\t Display this message.\n\n"					\
    "\t -n, --nstreams\n"						\
    "\t\t Used to specify how many streams to generate from each function. Defaults to 1.\n\n"\
    "\t -i, --infile\n"						\
    "\t\t Specify the file to use as input to the subsystem. Usually specifies a file\n"\
    "\t\t containing event data.\n\n"								\
    "\t -o, --outfile\n"						\
    "\t\t Data will be output to this file.\n\n"			\
    "\t -p or --paramfile\n"						\
    "\t\t Specify the parameter file in which to find default parameters when experimenting.\n\n"\
    "\t -r\n"\
    "\t\t Switches the system to use randomly generated functions. The generator will produce\n"\
    "\t\t functions randomly generated using gaussians, or if the -i switch is received, will\n"\
    "\t\t read a file containing 1-d data and centre gaussians on those points. Passing this\n"\
    "\t\t switch when using the estimators will set them to read data from files generated\n"\
    "\t\t using the random function generator as opposed to the default one.\n\n"	\
    "\t -S or --seed\n"\
    "\t\t Set the seed to use for random number generation. Relevant for all generation\n"\
    "\t\t functions.\n\n"\
    "\t -t, --outtype\n"						\
    "\t\t Used to specify how much data to ouput to file when estimating or generating.\n"\
    "\t\t A value of zero will output no files at all. 1 will output the most\n"\
    "\t\t useful data for the specific task, and anything above that will output more specific\n"\
    "\t\t information. Each level outputs its own data and anything from levels below it.\n\n"\
    "\t\t Basic stream generation (each level outputs only that data):\n"\
    "\n\t\t\t 1 - Events only."\
    "\n\t\t\t 2 - Events and lambda values."\
    "\n\t\t\t 3 - Bin counts only."\
    "\n\t\t\t 4 - Events, lambda values to one file, bin counts to another."\
    "\n\n\t\t Gaussian function generation:"		     \
    "\n\t\t\t 1 - mean, stdev, weight of gaussians."	\
    "\n\t\t\t 2 - sum of gaussians at points along x."		\
    "\n\t\t\t 3 - contribution of individual gaussians.\n\n"	\
    "\t\t Delta Estimation:"						\
    "\n\t\t\t 1 - value computed by the function for each guess at delta" \
    "\n\t\t\t 2 - lambda sums at best estimate, bin data.\n\n"
#define VERSION "deltastream alpha v0.7"
#define BUGREPORT "Report bugs to mxs968@cs.bham.ac.uk"

void run_requested_operations(launcher_args* args, char* paramfile, char* extra_paramfile,
			      char* infile, char* outfile, char* estimator_type,
			      char* generator_type);
int exists_in_arr(char** arr, int len, char* name);
launcher_args* make_arg_struct();

#endif
