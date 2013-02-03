#ifndef _DEFPARAMS_H
#define _DEFPARAMS_H

// output
#define DEFAULT_OUTFILE "output"
#define DEFAULT_EST_OUTFILE "est_out"
#define DEFAULT_FUNC_OUTFILE "random_function"
#define DEFAULT_RAW_FUNC_OUTFILE "random_function_raw"
#define DEFAULT_GAUSS_EVENT_FUNC_OUTFILE "gauss_stream_function"
#define DEFAULT_GAUSS_EVENT_RAW_FUNC_OUTFILE "gauss_stream_function_raw"
#define DEFAULT_OLS_OUT "ols_out"
#define DEFAULT_IWLS_OUT "iwls_out"
#define DEFAULT_PC_OUT "pc_out"
#define DEFAULT_BASE_OUT "base_out"
#define DEFAULT_GAUSS_GEN_OUT "gauss_out"
#define DEFAULT_GAUSS_EST_OUT "gauss_est_out"

// input
#define DEFAULT_INFILE "output_stream_0.dat"

// Default parameters for generation
#define DEFAULT_EXTENSION "_stream_"
#define DEFAULT_FUNCTION_EXT "_func_"
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

// gaussian generation params
#define DEFAULT_STDEV 10.0
#define DEFAULT_GEN_STEP 10.0
#define DEFAULT_GAUSS_RESOLUTION 0.5
#define DEFAULT_GAUSS_MULTIPLIER 5.0
#define DEFAULT_GAUSSIANS 40

// Default parameters for estimators
#define DEFAULT_ESTIMATOR "base"
#define DEFAULT_SUBINTERVALS 5
#define DEFAULT_IWLS_ITERATIONS 3
#define DEFAULT_MAX_EXTENSION 15.0
#define DEFAULT_MAX_BREAKPOINTS 5
#define DEFAULT_MIN_INTERVAL_PROP 0.1
#define DEFAULT_PMF_THRESHOLD 0.02
#define DEFAULT_PMF_SUM_THRESHOLD 0.85
#define DEFAULT_GAUSS_EST_STDEV 5.0
#define DEFAULT_GAUSS_EST_RESOLUTION 0.3

#endif
