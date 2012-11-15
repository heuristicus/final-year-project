#include "paramlist.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef _FUTIL_H
#define _FUTIL_H

void standard_out(char filename, void *arr1, void *arr2, int len, char *arrtype);
void mult_double_to_file(char *filename, char *mode, double *arr1, double *arr2, int len);
void mult_int_to_file(char *filename, char *mode, int *arr1, int *arr2, int len);
char *generate_outfile();
paramlist *get_parameters(char *filename);
int valid_param(char *pname);
char* select_output_file(char* cur_out, char *param_out);
void arr_to_file(char* filename, void* arr, int len, char* format_string);
void double_to_file(char *filename, char *mode, double *arr, int len);
void int_to_file(char *filename, char *mode, int *arr, int len);
double* get_event_data_interval(double start_time, double end_time, char *filename);
double* get_event_data_all(char *filename);
void int_dbl_to_file(char* filename, char* mode, double* arr1, int* arr2, int len);


#endif
