#include "experimenter.h"

void experiment(char* paramfile)
{
    //paramlist* params = get_parameters(paramfile);
}

/*
 * Parses an array of strings in the form ["1.0", "2.0", "...", "5.0"] into a 
 * double array. In this case, the returned array would be 
 * [1.0, 2.0, 3.0, 4.0, 5.0]. The "..." is expanded to get the numbers that would
 * come between 2.0 and 5.0 with a step of 2.0-1.0
 */
double_arr* parse_double_range(char** param_string)
{
    return NULL;
}

/*
 * Parses an array of strings in the form ["1", "2", "...", "5"] into a double array
 * in this case, the returned array would be [1, 2, 3, 4, 5]. The "..." is 
 * expanded to get the numbers that would come between 2 and 5 with a step of 2-1
 */
int_arr* parse_int_range(char** param_string)
{
    return NULL;
}
