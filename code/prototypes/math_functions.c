#include "poisson_random.h"

long double fact(int i)
{
    if (i == 0)
	return 1;
    else
	return i * fact(i - 1);
}
