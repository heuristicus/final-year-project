#ifndef _PARAMLIST_H
#define _PARAMLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

paramlist *init_list(char *param, char *val);
paramlist *add(paramlist *head, char *pname, char *pval);
void print_list(paramlist *head);
void free_list(paramlist *head);
int list_length(paramlist *head);
paramlist* get_param(paramlist *head, char *param_name);
char* get_param_val(paramlist *head, char *param_name);
char* get_string_param(paramlist* head, char* param_name);
double get_double_param(paramlist* head, char* param_name);
int get_int_param(paramlist* head, char* param_name);
double_arr* get_double_list_param(paramlist* head, char* param_name);
int set_double_param(paramlist* head, char* param_name, double new_value);
int set_int_param(paramlist* head, char* param_name, int new_value);
int set_string_param(paramlist* head, char* param_name, char* new_value);

#endif
