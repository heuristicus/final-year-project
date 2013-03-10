#ifndef _FUTIL_H
#define _FUTIL_H

#include "paramlist.h"
#include "estimator.h"
#include "math_util.h"
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
#include <dirent.h>

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
void double_mult_dim_to_file(char* filename, char* mode, double_multi_arr* arr);
double_arr* get_event_data_interval(double start_time, double end_time, char* filename);
double_arr* get_event_data_all(char* filename);
void int_dbl_to_file(char* filename, char* mode, double* arr1, int* arr2, int len);
void estimate_to_file(char* filename, est_data* estimate, char* mode);
void output_estimates(char* filename, est_data** estimates, int len);
void output_gauss_transform(char* filename, char* mode, double** T, double shift,
			    int len, double normaliser);
void output_gaussian_contributions(char* filename, char* mode, gauss_vector* G, double start,
				   double end, double resolution, int apply_weight);
void output_gaussian_vector(char* filename, char* mode, gauss_vector* V);
gauss_vector* read_gauss_vector(char* filename);
void output_double_multi_arr(char* filename, char* mode, double_multi_arr* arr);
void output_double_multi_arr_numbered(char* filename, char* mode, double_multi_arr* arr);
int create_file_in_dir(char* filename, char* dirname);
int file_exists(char* filename);
int dir_exists(char* dirname);
int create_dir(char* dirname);
void list_to_file(char* filename, char* mode, paramlist* param);

#endif
