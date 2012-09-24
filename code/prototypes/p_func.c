#include "poisson_random.h"

/* seeds the random variable to be used. Should only be called once per run (?). */
void init_rand(void)
{
    srand48(time(NULL));
}

/* knuth method. Generates time to next even in a homogenous poisson process. */
double homogenous_time(double lambda)
{
    return -log(drand48()) / lambda;
}

/* generates an event time for a non-homogenous poisson process. This will require solving lambda(t) and passing its result as a parameter. Not sure how to do this or if there is a better way. */
double non_homogenous_time(double eqsolution)
{
    
}

/* generates time for events in a homogenous poisson process until time is exceeded. */
double* generate_event_times_homogenous(double lambda, double time, int max_events)
{
    init_rand();
        
    double e_times[max_events] = {0};
    double run_time = 0, i = 0;
        
    while ((run_time += homogenous_time(lambda)) < time){
	e_times[i] = run_time;
	++i;
    }
}

/* calculates the probabilty of there being k events between time t_start and t_end. (for homogenous processes)*/
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k)
{
    double tau = t_end - t_start;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}
