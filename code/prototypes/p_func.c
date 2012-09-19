#include "poisson_random.h"

/* seeds the random variable to be used. Should only be called once per run (?). */
void init_rand(void)
{
    srand48(time(NULL));
}

/* knuth method */
double generate_event_time(double lambda)
{
    return -log(drand48()) / lambda;
}

/* calculates the probabilty of there being k events between time t_start and t_end. (for homogenous processes)*/
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k)
{
    double tau = t_end - t_start;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}
