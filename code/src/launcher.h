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
#define OPT_INFO "OPTIONS\n"						\
    "\t -e or --estimate\n"						\
    "\t\t Estimate the underlying function from a given set of photon stream data."\
    " Requires parameter file.\n\n"					\
    "\t -a or --estimator\n"						\
    "\t\t The estimation algorithm to use. Options are IWLS (iwls), OLS (ols),"\
    " Piecewise (pc), Baseline (base)\n\n"				\
    "\t -d or --defparam\n"						\
    "\t\t Creates a default parameter file with the given name\n\n"	\
    "\t -g or --generate\n"						\
    "\t\t Generate a photon stream. Requires parameter file.\n\n"       \
    "\t -h or --help\n"							\
    "\t\t Display this message.\n\n"					\
    "\t -n or --nstreams\n"						\
    "\t\t Number of times to run generation. Use this to generate multiple streams."\
    " Also specifies the number of stream estimates to combine when using the -l switch.\n\n"								\
    "\t -l or --estall\n"						\
    "\t\t Estimates the functions of multiple streams and then combines the functions.\n\n" \
    "\t -i or --infile\n"						\
    "\t\t Specify the file to use as input to the estimator.\n\n"	\
    "\t -o or --outfile\n"						\
    "\t\t Data will be output to this file.\n\n"			\
    "\t -p or --paramfile\n"						\
    "\t\t The file containing parameters to use. This can be used to specify a "\
    "large number of options.\n\n"					\
    "\t -x or --experiment\n"						\
    "\t\t Run an experiment. Requires parameter file.\n\n"
#define VERSION "poissonstream alpha v0.3"
#define BUGREPORT "Report bugs to mxs968@cs.bham.ac.uk"

void run_requested_operations(int generator, int estimator, int experiment, char* paramfile, char* infile, char* outfile, int nruns, char* estimator_type, int estall);
int estimator_valid(char* name);
void multi_estimate(char* paramfile, char* infile, char* outfile, char* estimator_type, int nstreams);

#endif
