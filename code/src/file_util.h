#ifndef _FUTIL_H
#define _FUTIL_H

#include "paramlist.h"
#include "estimator.h"
#include "general_util.h"
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

void standard_out(char filename, void* arr1, void* arr2, int len, char* arrtype);
void mult_double_to_file(char* filename, char* mode, double* arr1, double* arr2, int len);
void mult_int_to_file(char* filename, char* mode, int* arr1, int* arr2, int len);
char* generate_outfile();
paramlist* get_parameters(char* filename);
int valid_param(char* pname);
char* select_output_file(char* cur_out, char* param_out);
void arr_to_file(char* filename, void* arr, int len, char* format_string);
void double_to_file(char* filename, char* mode, double* arr, int len);
void int_to_file(char* filename, char* mode, int* arr, int len);
void double_mult_dim_to_file(char* filename, char* mode, double_mult_arr* arr);
double* get_event_data_interval(double start_time, double end_time, char* filename);
double* get_event_data_all(char* filename);
void int_dbl_to_file(char* filename, char* mode, double* arr1, int* arr2, int len);
void estimate_to_file(char* filename, est_data* estimate, char* mode);
void output_estimates(char* filename, est_data* *estimates, int len);
int create_file_in_dir(char* filename, char* dirname);

#endif
