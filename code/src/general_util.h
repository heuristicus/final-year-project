#include "estimator.h"

#ifndef _GEN_UTIL_H
#define _GEN_UTIL_H

void free_pointer_arr(void **arr, int length);
char** string_split(char *string, char separator);
void print_int_arr(int *arr, int len);
void print_double_arr(double *arr, int len);
int interval_valid(double interval_start, double interval_end);
double* get_event_subinterval(double *events, double interval_start, double interval_end);
void print_estimates(est_arr *estimates);
void free_est_arr(est_arr *estimates);
void print_string_array(char* message, char** array, int len);

#endif
