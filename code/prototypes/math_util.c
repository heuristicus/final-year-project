#include "math_util.h"

int rand_initialised = 0;

int main(int argc, char *argv[])
{
    
    return 0;
}

long double fact(int i)
{
    if (i == 0)
	return 1;
    else
	return i * fact(i - 1);
}

/* calculates the probabilty of there being k events between time t_start and t_end. (for homogenous processes)*/
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k)
{
    double tau = t_end - t_start;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}

/* 
 * Outputs the number of events in each interval to the array provided. 
 */
int* sum_events_in_interval(double *event_times, int num_events, double interval_time, int interval_num)
{
    int i = 0, current_interval = 0;
    double event_time;
        
    int *bins = calloc(interval_num, sizeof(int));
    
    for (event_time = event_times[0]; i < num_events; ++i, event_time = event_times[i]){
	while (event_time > interval_time * (current_interval + 1)){
	    //printf("%lf is greater than %lf, interval time: %lf, current_interval %d\n", event_time, interval_time * current_interval + 1, interval_time, current_interval);
	    current_interval++;
	}

	//printf("current time: %lf, interval end: %lf. Adding to loc %d\n", event_time, interval_time * (current_interval + 1), current_interval);
	
	bins[current_interval]++;
	event_time = event_times[i];
    }

    return bins;
    
}

/*
 * From "The Art of Computer Programming", Vol.3. Generates a gaussian random
 * value with mean of 0 and standard deviation of 1. http://c-faq.com/lib/gaussian.html
 */
double rand_gauss()
{

    if(!rand_initialised){
	srand(time(NULL));
	rand_initialised = 1;
    }
    
    static double V1, V2, S;
    static int phase = 0;
    double X;

    if(phase == 0) {
	do {
	    double U1 = (double)rand() / RAND_MAX;
	    double U2 = (double)rand() / RAND_MAX;

	    V1 = 2 * U1 - 1;
	    V2 = 2 * U2 - 1;
	    S = V1 * V1 + V2 * V2;
	} while(S >= 1 || S == 0);

	X = V1 * sqrt(-2 * log(S) / S);
    } else
	X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    return X;
}


/*
 * Gets noise based on a poisson distribution centred around the mean
 * provided. Since the mean and variance of a poisson random variable
 * are the same, the larger the mean, the larger the variance.
 */
double get_poisson_noise(double mean)
{

    return get_gaussian_noise(mean, mean);
    
}

double get_gaussian_noise(double mean, double std_dev)
{
    return (rand_gauss() * std_dev) + mean;
    
}

double* get_interval_midpoints(double total_time, int subintervals)
{
    double *midpoints = malloc(subintervals * sizeof(double));
    
    int i;
    
    for (i = 0; i < subintervals; ++i){
	midpoints[i] = get_interval_midpoint(i + 1, total_time, subintervals);
    }

    return midpoints;
}

/*
 * Get the midpoint of a specified interval. (midpoint(xk)=(k-1/2)*T/N), 1 <= k <= N
 */
double get_interval_midpoint(int interval_number, double total_time, int subintervals)
{
    return (interval_number - 1.0/2.0) * (total_time /subintervals);
}
