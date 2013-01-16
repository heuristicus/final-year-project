#include "generator.h"
#include "estimator.h"
#include "file_util.h"
#include "defparams.h"
#include "paramlist.h"
#include "general_util.h"

void experiment(char* paramfile)
{
    paramlist* params = get_parameters(paramfile);
}

/*
 * Parses a string in the form 1.0,2.0,...,5.0 into a double array
 * in this case, the pointer copyto would point to
 * [1.0, 2.0, 3.0, 4.0, 5.0]. The "..." is expanded to get the
 * numbers that would come between 2.0 and 5.0 with a step of 2.0-1.0
 * The length of the array copyto is returned.
 */
int parse_double_range(double* copyto, char** param_string)
{
    copyto[0] = atof(param_string[0]);
    copyto[1] = atof(param_string[1]);
    
    double step = copyto[1] - copyto[0];
    
    
}

/*
 * Parses a string in the form 1,2,...,5 into a double array
 * in this case, the pointer copyto would point to
 * [1, 2, 3, 4, 5]. The "..." is expanded to get the
 * numbers that would come between 2 and 5 with a step of 2-1
 * The length of the array in copyto is returned.
 */
int parse_int_range(int* copyto, char* param_string)
{
    
}
