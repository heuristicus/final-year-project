#ifndef _EXPERIMENTER_H
#define _EXPERIMENTER_H

void experiment(char* paramfile);
int parse_double_range(double* copyto, char* param_string);
int parse_int_range(double* copyto, char* param_string);

#endif
