#include "poisson_random.h"

double run_time = 0;

int main(int argc, char *argv[])
{
    srand48(time(NULL));
    double lambda = 5.0;
    extern double run_time;
    
    while (run_time < 100)
	generate_photon_event(lambda);
    
    return 0;
    
}

/* knuth method */
void generate_photon_event(double lambda)
{
    extern double run_time;
        
    run_time += -log(drand48()) / lambda;
    printf("time: %lf\n", run_time);
}

