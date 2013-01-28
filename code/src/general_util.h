#ifndef _GEN_UTIL_H
#define _GEN_UTIL_H

#include "estimator.h"

typedef struct
{
    double* data;
    int len;
} double_arr;

typedef struct
{
    int* data;
    int len;
} int_arr;

typedef struct
{
    char** data;
    int len;
} string_arr;

typedef struct
{
    double* data1;
    double* data2;
    int len;
} double_mult_arr;

void free_pointer_arr(void** arr, int length);
string_arr* string_split(char* string, char separator);
void print_int_arr(int* arr, int len);
void print_double_arr(double* arr, int len);
int interval_valid(double interval_start, double interval_end);
double* get_event_subinterval(double* events, double interval_start, double interval_end);
void print_estimates(est_arr* estimates);
void free_est_arr(est_arr* estimates);
void print_string_array(char* message, char** array, int len);
int create_default_param_file(char* filename);
void put_section_header(FILE* fp, char* heading);
void free_string_arr(string_arr* arr);

#endif
