#include "estimator.h"

/*
 * Estimates the delay of one function with relation to another by calculating
 * the probability density function at points with a certain distance between them.
 * The minimum of the sum of these probability density functions is taken as the
 * best estimate of the delay. The max_delay parameter specifies the maximum
 * positive or negative delay to consider when attempting to find an estimate.
 * The resolution specifies the step between points on the x-axis which are
 * compared. A high resolution will take longer, but the estimate will be more
 * accurate.
 */
double estimate_delay_pmf(void* f1, void* f2, double_arr* events, double max_delay,
			  double resolution, double step, char* type)
{
    // guess at a time delay
    double best_guess = 0;
    double best_value = INFINITY;
    double guess = 0;

    double start = -max_delay, end = max_delay;
    double current = start;
    
    FILE *fp = fopen("delaycheck_pmf", "w");

    while (current <= end){
	fprintf(fp, "%lf %lf\n", current, guess);
	if (guess < best_value){
	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current);
	    best_value = guess;
	    best_guess = current;
	} else {
	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess);
	}
	current += step;
    }

    fclose(fp);

    return -best_guess;
    // combine the two functions as if it were the real result
    // find log(pmf) at points along x and sum them
    // find the delay where the log(pmf) is maximised

    return 0;
}

/*
 * Estimates the time delay of one function with relation to another by finding
 *  the minimum value of the area between their curves. This is done in a 
 * discrete manner by guessing a time delay and calculating the value,
 * and continuing to make guesses until the resulting area goes below
 * a threshold value. The max_delay parameter specifies the maximum
 * positive or negative delay to consider when attempting to find an estimate.
 * The resolution specifies the step between points on the x-axis which are
 * compared. A high resolution will take longer, but the estimate will be more
 * accurate. The returned value is the estimate of how much the second function
 * is shifted. The value calculated by the function is inverted upon return
 * to give the amount that f2 must be shifted to line it up with f1.
 */
double estimate_delay_area(void* f1, void* f2, double max_delay, double resolution,
			   double step, char* type)
{
    double best_guess = 0;
    double best_value = INFINITY;
    double guess;

    double start = -max_delay, end = max_delay;
    double current = start;
    
    FILE *fp = fopen("delaycheck", "w");

    while (current <= end){
	if (strcmp(type, "gauss") == 0){
	    guess = total_area_estimate((void*)f1, (void*)f2, 0, 100, resolution, current, "gauss");
	} else if (strcmp(type, "base") == 0){
	    guess = total_area_estimate((void*)f1, (void*)f2, 0, 100, resolution, current, "base");
	}
	fprintf(fp, "%lf %lf\n", current, guess);
	if (guess < best_value){
	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current);
	    best_value = guess;
	    best_guess = current;
	} else {
	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess);
	}
	current += step;
    }

    fclose(fp);

    return -best_guess;
}

/*
 * Estimates the total area between the curves f1 and f2 in the interval
 * [start, end], with samples taken at the given resolution and the given
 * delay applied to f2. The higher the resolution given, the higher better the
 * estimate will be, but the computation will also take longer.
 */
double total_area_estimate(void* f1, void* f2, double start,double end,
			   double resolution, double delay, char* type)
{
    if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0)
	return -1;

    double current = start, pt = 0;
    double sum = 0;
    int i = 0;

    while(current <= end){
	if (strcmp(type, "gauss") == 0){
	    pt = area_at_point_gauss((gauss_vector*)f1, (gauss_vector*)f2, current, delay);
	} else if (strcmp(type, "base") == 0){
	    pt = area_at_point_base((est_arr*)f1, (est_arr*)f2, current, delay);
	}
	if (pt != -1){
	    sum += pt;
	    ++i;
	}
	current += resolution;
    }

    return sum/i;
}

/*
 * Estimates the area between points at a given point on the x-axis, where function
 * f2 is delayed by delta. This version is used for calculation with structs
 * produced by the gaussing estimator.
 */
double area_at_point_gauss(gauss_vector* f1, gauss_vector* f2, double x, double delta)
{
    if (f1 == NULL || f2 == NULL)
	return -1;

    double f1sum = sum_gaussians_at_point(x, f1);
    double f2sum = sum_gaussians_at_point(x + delta, f2);

    return pow(f1sum - f2sum, 2);
}

/*
 * Estimates the area at a given point on the x-axis, where function f2 is delayed
 * by delta. This version of the function is used for calculation with
 * arrays produced by the baseline estimator
 */
double area_at_point_base(est_arr* f1, est_arr* f2, double x, double delay)
{
    if (f1 == NULL || f2 == NULL)
	return -1;

    double f1val = estimate_at_point(f1, x);
    double f2val = estimate_at_point(f2, x + delay);

    return pow(f2val - f1val, 2);
}

/* /\* */
/*  * Estimates the time delay of one function with relation to another by finding */
/*  *  the minimum value of the area between their curves. This is done in a  */
/*  * discrete manner by guessing a time delay and calculating the value, */
/*  * and continuing to make guesses until the resulting area goes below */
/*  * a threshold value. The max_delay parameter specifies the maximum */
/*  * positive or negative delay to consider when attempting to find an estimate. */
/*  * The resolution specifies the step between points on the x-axis which are */
/*  * compared. A high resolution will take longer, but the estimate will be more */
/*  * accurate. The returned value is the estimate of how much the second function */
/*  * is shifted. The value calculated by the function is inverted upon return */
/*  * to give the amount that f2 must be shifted to line it up with f1. */
/*  *\/ */
/* double estimate_delay_area_gauss(gauss_vector* f1, gauss_vector* f2,  */
/* 				 double max_delay, double resolution) */
/* { */
/*     double best_guess = 0; */
/*     double best_value = INFINITY; */
/*     double guess; */

/*     double start = -max_delay, end = max_delay; */
/*     double current = start; */
    
/*     FILE *fp = fopen("delaycheck", "w"); */

/*     while (current <= end){ */
/* 	printf("shifting by %lf\n", current); */
/* 	guess = total_area_estimate_gauss(f1, f2, 0, 100, resolution, current); */
/* 	fprintf(fp, "%lf %lf\n", current, guess); */
/* 	if (guess < best_value){ */
/* 	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current); */
/* 	    best_value = guess; */
/* 	    best_guess = current; */
/* 	} else { */
/* 	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess); */
/* 	} */
/* 	current += 1; // change this to a paramfile value */
/*     } */

/*     fclose(fp); */

/*     return -best_guess; */
/* } */

/* double estimate_delay_area_baseline(est_arr* f1, est_arr* f2, */
/* 				    double max_delay, double resolution) */
/* { */
/*     double best_guess = 0; */
/*     double best_value = INFINITY; */
/*     double guess; */

/*     double start = -max_delay, end = max_delay; */
/*     double current = start; */
    
/*     FILE *fp = fopen("delaycheck", "w"); */

/*     while (current <= end){ */
/* 	guess = total_area_estimate_base(f1, f2, 0, 100, resolution, current); */
/* 	fprintf(fp, "%lf %lf\n", current, guess); */
/* 	if (guess < best_value){ */
/* 	    printf("New value %lf is less than old %lf. guess updated to %lf\n", guess, best_value, current); */
/* 	    best_value = guess; */
/* 	    best_guess = current; */
/* 	} else { */
/* 	    printf("New value %lf is larger than old %lf. guess remains at %lf\n", guess, best_value, best_guess); */
/* 	} */
/* 	current += 1; // change this to a paramfile value */
/*     } */

/*     fclose(fp); */

/*     return -best_guess; */
/* } */

/* double total_area_estimate_base(est_arr* f1, est_arr* f2, double start, */
/* 				double end, double resolution, double delay) */
/* { */
/*     if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0) */
/* 	return -1; */

/*     double current = start; */
/*     printf("start %lf end %lf delay %lf interval overlap %lf\n", start, end, delay, end - start - fabs(delay)); */
/*     double sum = 0; */
/*     int i = 0; */

/*     while(current <= end){ */
/* 	double pt = area_at_point_base(f1, f2, current, delay); */
/* 	if (pt != -1){ */
/* 	    sum += pt; */
/* 	    ++i; */
/* 	} */
/* 	current += resolution; */
/*     } */

/*     return sum/i; */
/* } */


/*  /\* */
/*   * Estimates the total area between the curves f1 and f2 in the interval */
/*   * [start, end], with samples taken at the given resolution and the given */
/*   * delay applied to f2. The higher the resolution given, the higher better the */
/*   * estimate will be, but the computation will also take longer. */
/*   *\/ */
/* double total_area_estimate_gauss(gauss_vector* f1, gauss_vector* f2, double start, double end, */
/* 		   double resolution, double delay) */
/*  { */
/*      if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0) */
/* 	 return -1; */

/*      double current = start; */
/*      double sum = 0; */
/*      int i = 0; */

/*      while(current <= end){ */
/* 	 double pt = area_at_point_gauss(f1, f2, current, delay); */
/* 	 if (pt != -1){ */
/* 	     sum += pt; */
/* 	     ++i; */
/* 	 } */
/* 	current += resolution; */
/*     } */

/*     return sum/i; */
/* } */


/* /\* */
/*  * Estimates the time delay of one function with relation to another by finding */
/*  *  the minimum value of the area between their curves. This is done in a  */
/*  * discrete manner by guessing a time delay and calculating the value, */
/*  * and continuing to make guesses until the resulting area goes below */
/*  * a threshold value. The max_delay parameter specifies the maximum */
/*  * positive or negative delay to consider when attempting to find an estimate. */
/*  *\/ */
/* double estimate_delay_discrete_area(double_multi_arr* f1, double_multi_arr* f2, */
/* 				    double max_delay) */
/* { */
/*     double best_guess = 0; */
/*     double best_value = -INFINITY; */
/*     double guess; */

/*     double start = -max_delay, end = max_delay; */
/*     double current = start; */
    
/*     while (current <= end){ */
/* 	printf("shifting by %lf\n", current); */
/* 	guess = total_area_estimate(f1, f2, current); */
/* 	if (guess > best_value){ */
/* 	    best_value = guess; */
/* 	    best_guess = current; */
/* 	} */
/* 	current += 5; // change this to a paramfile value */
/*     } */

/*     return best_guess; */
/* } */


/* /\* */
/*  * Estimates the total area between the curves f1 and f2 in the interval */
/*  * [start, end], with the given delay applied to f2. The given double arrays */
/*  * are assumed to contain the values of the two functions at the same points */
/*  * on the x-axis. The area is estimated by summing the differences between the */
/*  * two function values for each point. The calculation is only done for points */
/*  * at which both functions have values. */
/*  *\/ */
/* double total_area_estimate(double_multi_arr* f1, double_multi_arr* f2, double delay) */
/* { */
/*     if (f1 == NULL || f2 == NULL) */
/* 	return -1; */
    
/*     double sum = 0; */

/*     double* shift = add_to_arr(f2->data[0], f2->lengths[0], delay); */

/*     int i; */
    
/*     for (i = 0; i < f2->lengths[0]; ++i) { */
/* 	printf("f1 data %lf, f2 data %lf\n", f1->data[0][i], shift[i]); */
/*     } */

/*     return sum/i; */
/* } */
