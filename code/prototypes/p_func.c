#include "poisson_random.h"

#define MAX_EQN_LEN 30 // equation length max 30 chars

int main(int argc, char *argv[])
{

    int i;
    int max = 200;
    
    double event_times[max];
    double lambda_vals[max];

    muParserHandle_t hparser = mupCreate(0);

    char* eqn = "log(t)";
    
    mupSetExpr(hparser, eqn);
        
    generate_event_times_non_homogenous(hparser, 1000, 20000, max, event_times, lambda_vals);
    
    for (i = 0; i < max && event_times[i] > 0; ++i){
	printf("%lf %lf\n", event_times[i], lambda_vals[i]);
    }
    
    return 0;
}


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
double non_homogenous_time(double time)
{
    return log(time);
}

/* generates time for events in a homogenous poisson process until time is exceeded. Puts event times into the array passed in the parameters. Puts a -1 in the array location after the last event*/
void generate_event_times_homogenous(double lambda, double time, int max_events, double* event_times)
{
    init_rand();
        
    double run_time = 0;
    int i = 0;
    
    while ((run_time += homogenous_time(lambda)) < time && i < max_events){
	event_times[i] = run_time;
	++i;
    }

    if (i < max_events)
	event_times[i] = -1.0;
    
}

/* THIS IS TEMPORARY homogenous lambda must be greater than the value of the non-homogenous function lambda(t) for all t <= time*/
void generate_event_times_non_homogenous(muParserHandle_t hparser, double lambda, double time, int max_events, double* event_times, double* lambda_vals)
{
    init_rand();
        
    double run_time = 0.0, rand, nht, lm;
    int i = 0;
    mupDefineVar(hparser, "t", &run_time);
    
    while ((run_time += homogenous_time(lambda)) < time && i < max_events){
	nht = mupEval(hparser);
	lm = nht / lambda;
	
	if ((rand = drand48()) <= (lambda_vals[i] = lm)){
	    event_times[i] = run_time;
	    ++i;
	}
    }

    if (i < max_events)
	event_times[i] = -1.0;
}

/* calculates the probabilty of there being k events between time t_start and t_end. (for homogenous processes)*/
double prob_num_events_in_time_span(double t_start, double t_end, double lambda, int k)
{
    double tau = t_end - t_start;
    return (pow(M_E, -lambda * tau) * pow(lambda * tau, k)) / fact(k);
}
