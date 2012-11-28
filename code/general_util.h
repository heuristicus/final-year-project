#ifndef _GEN_UTIL_H
#define _GEN_UTIL_H

void free_pointer_arr(void **arr, int length);
char** string_split(char *string, char separator);
void print_int_arr(int *arr, int len);
void print_double_arr(double *arr, int len);
int interval_check(double interval_start, double interval_end);

#endif
