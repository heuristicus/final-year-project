#ifndef _DEFPARAMS_H
#define _DEFPARAMS_H

// output
#define DEFAULT_OUTFILE "output.dat"
#define DEFAULT_EST_OUTFILE "est_out.dat"
#define DEFAULT_OLS_OUT "ols_out.dat"
#define DEFAULT_IWLS_OUT "iwls_out.dat"
#define DEFAULT_PC_OUT "pc_out.dat"
#define DEFAULT_BASE_OUT "base_out.dat"

// input
#define DEFAULT_INFILE "output.dat_stream_0_ev"

// Default parameters for generation
#define DEFAULT_EXTENSION "_stream_"
#define DEFAULT_VERBOSITY 3
#define DEFAULT_START 0
#define DEFAULT_NRUNS 1
#define DEFAULT_TIMEDELTA "0.0,10.0"
#define DEFAULT_LAMBDA 1000
#define DEFAULT_INTERVAL 100
#define DEFAULT_SEED 0
#define DEFAULT_EXPRESSION "a-(b*sin(alpha*t))"
#define DEFAULT_A 10
#define DEFAULT_B 5
#define DEFAULT_ALPHA 0.1

// Default parameters for estimators
#define DEFAULT_ESTIMATOR "baseline"
#define DEFAULT_SUBINTERVALS 5
#define DEFAULT_IWLS_ITERATIONS 3
#define DEFAULT_MAX_EXTENSION 15.0
#define DEFAULT_MAX_BREAKPOINTS 5
#define DEFAULT_MIN_INTERVAL_PROP 0.1
#define DEFAULT_PMF_THRESHOLD 0.02
#define DEFAULT_PMF_SUM_THRESHOLD 0.85

#endif
