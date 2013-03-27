#ifndef _GEN_UTIL_H
#define _GEN_UTIL_H

#include "struct.h"
#include "defparams.h"
#include "paramlist.h"
#include "file_util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void free_pointer_arr(void** arr, int length);
string_arr* string_split(char* string, char separator);
void print_int_arr(int* arr, int len);
void print_double_arr(double* arr, int len);
int interval_valid(double interval_start, double interval_end);
double_arr* get_event_subinterval(double_arr* events, double interval_start, double interval_end);
void print_estimates(est_arr* estimates);
void free_est_arr(est_arr* estimates);
void print_string_array(char* message, char** array, int len);
int create_default_param_file(char* filename, char* type);
void put_section_header(FILE* fp, char* heading);
void print_gauss_vector(gauss_vector* G);
void free_gauss_vector(gauss_vector* G);
void free_double_multi_arr(double_multi_arr* arr);
void free_string_arr(string_arr* arr);
int has_required_params(paramlist* params, char** required_params, int len);
est_data* data_at_point(est_arr* estimate, double check_time);
double estimate_at_point(est_arr* estimate, double time);
double_arr* estimate_at_points(est_arr* estimate, double* points, int len);
double_multi_arr* init_multi_array(int num_arrays, int array_length);
double_arr* init_double_arr(int len);
void free_double_arr(double_arr* arr);
void free_exp_set(exp_set* es);
void free_tuple_arr(exp_tuple_arr* ta);
void free_exp_tuple(exp_tuple* t);
void free_tdelta_result(tdelta_result* res);
void print_double_multi_arr(double_multi_arr* arr);

#endif
