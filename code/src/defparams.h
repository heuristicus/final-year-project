#ifndef _DEFPARAMS_H
#define _DEFPARAMS_H

// output
#define DEFAULT_OUTFILE "output"
#define DEFAULT_EST_OUTFILE "est_out"
#define DEFAULT_FUNC_OUTFILE "random_function"
#define DEFAULT_STREAM_FUNC_OUTFILE "gauss_stream_function"
#define DEFAULT_OLS_OUT "ols_est_out"
#define DEFAULT_IWLS_OUT "iwls_est_out"
#define DEFAULT_PC_OUT "pc_est_out"
#define DEFAULT_BASE_OUT "base_est_out"
#define DEFAULT_GAUSS_GEN_OUT "gauss_out"
#define DEFAULT_GAUSS_EST_OUT "gauss_est_out"
#define DEFAULT_STEP 1.0

// input
#define DEFAULT_INFILE "output_stream_0.dat"

// Default parameters for generation
#define DEFAULT_EXTENSION "_stream_"
#define DEFAULT_FUNCTION_EXT "_func_"
//#define DEFAULT_VERBOSITY 3
#define DEFAULT_START 0.0
#define DEFAULT_NSTREAMS 1
#define DEFAULT_TIMEDELTA "0.0,10.0,20.0"
#define DEFAULT_LAMBDA 1000
#define DEFAULT_INTERVAL 100.0
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
#define DEFAULT_STDEV_ALPHA 3.0
#define DEFAULT_SIMPLE_STDEV "yes"

// Default parameters for estimators
#define DEFAULT_ESTIMATOR "base"
#define DEFAULT_SUBINTERVALS 5
#define DEFAULT_IWLS_ITERATIONS 3
#define DEFAULT_MAX_EXTENSION 15.0
#define DEFAULT_MAX_BREAKPOINTS 10
#define DEFAULT_MIN_INTERVAL_PROP 0.01
#define DEFAULT_PMF_THRESHOLD 0.05
#define DEFAULT_PMF_SUM_THRESHOLD 0.85

// Gaussian estimation
#define DEFAULT_GAUSS_EST_STDEV 5.0
#define DEFAULT_GAUSS_EST_RESOLUTION 0.3

// Delta estimation
#define DEFAULT_DO_DELTA_EST "yes"
#define DEFAULT_DELTA_EST_METHOD "area"
#define DEFAULT_DELTA_EST_HIERARCHICAL "yes"
#define DEFAULT_DELTA_EST_AREA_RESOLUTION 5.0
#define DEFAULT_DELTA_EST_PMF_RESOLUTION 1.0
#define DEFAULT_DELTA_EST_STEP 2.0
#define DEFAULT_DELTA_EST_STEP_FINE 0.2
#define DEFAULT_DELTA_EST_FINE_RANGE 3.0
#define DEFAULT_MAX_DELTA 40.0
#define DEFAULT_DELTA_EST_BINS 50

// Normalisation constant
#define DEFAULT_NORMALISER_INITIAL 1.0
#define DEFAULT_NORMALISER_MAX 20.0
#define DEFAULT_NORMALISER_STEP 1.0
#define DEFAULT_NORMALISER_SUBINTERVALS 100

// Experiment params
#define DEFAULT_EXP_INDIR "event_streams"
#define DEFAULT_EXP_OUTDIR "experiment_output"
#define DEFAULT_EXP_OUTFILE "exp"
#define DEFAULT_EXP_STUTTER_STEP 10.0
#define DEFAULT_EXP_STUTTER_INTERVAL 5.0
#define DEFAULT_EXP_RUN_SEP "no"
#define DEFAULT_EXP_NUM_STREAMS 2
#define DEFAULT_EXP_NUM_FUNCS 10
#define DEFAULT_EXP_NAMES "baseline,gaussian"
#define DEFAULT_EXP_TEST_BASELINE "yes"
#define DEFAULT_EXP_TEST_GAUSSIAN "yes"
#define DEFAULT_EXP_BASELINE_PARAMS "base_max_breakpoints"
#define DEFAULT_EXP_GAUSSIAN_PARAMS "gauss_est_stdev"
#define DEFAULT_EXP_BASELINE_TYPE "delay"
#define DEFAULT_EXP_GAUSSIAN_TYPE "delay"
#define DEFAULT_EXP_BASE_MAX_BREAK 15
#define DEFAULT_EXP_GAUSS_EST_STDEV 1
#define DEFAULT_EXP_TDELTA "0.0,-10.0,20.0"

#endif
