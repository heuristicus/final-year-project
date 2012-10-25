#include "math_util.h"

int rand_initialised = 0;

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

/* Outputs the sum of events in a specific time period to the array provided. The contents of the array will
 * be wiped. Returns the last array location that contains a value.
 */
int rolling_window(double *event_times, int num_events, double start_time, double timespan, int *output_array)
{
    int i, spanmult = 0;
    double time;

    for (i = 0, time = event_times[i]; i < num_events; ++i, time = event_times[i]){
	while (time > timespan * (spanmult + 1)){
	    spanmult++;
	}
	
	//printf("span %d, val %lf\n", spanmult, event_times[i]);
	//printf("output arr %d\n", output_array[spanmult]);
	output_array[spanmult]++;
	
    }

    return spanmult;
    
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
