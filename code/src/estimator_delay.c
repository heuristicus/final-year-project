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
double estimate_delay_pdf(gauss_vector* f1, gauss_vector* f2, double max_delay,
			  double resolution)
{
    
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
 * accurate.
 */
double estimate_delay_discrete_area(gauss_vector* f1, gauss_vector* f2,
				    double max_delay, double resolution)
{
    
}


/*
 * Estimates the total area between the curves f1 and f2 in the interval
 * [start, end], with samples taken at the given resolution and the given
 * delay applied to f2. The higher the resolution given, the higher better the
 * estimate will be, but the computation will also take longer.
 */
double total_area_estimate(gauss_vector* f1, gauss_vector* f2, double start, double end,
		  double resolution, double delay)
{
    if (f1 == NULL || f2 == NULL || !interval_valid(start, end) || resolution <= 0)
	return -1;
    
    double current = start;
    double sum = 0;
    int i = 0;
        
    while(current <= end){
	sum += area_at_point(f1, f2, current, delay);
	current += resolution;
	++i;
    }

    return sum/i;
}

/*
 * Estimates the area between points at a given point on the x-axis, where function
 * f2 is delayed by the given amount.
 */
double area_at_point_estimate(gauss_vector* f1, gauss_vector* f2, double x, double delay)
{
    if (f1 == NULL || f2 == NULL)
	return -1;

    double f1sum = sum_gaussians_at_point(x, f1);
    double f2sum = sum_gaussians_at_point(x + delay, f2);
    
    return pow(f1sum - f2sum, 2);
}
