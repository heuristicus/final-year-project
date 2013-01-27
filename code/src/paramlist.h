#ifndef _PARAMLIST_H
#define _PARAMLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct paramlist 
{
    char *par;
    char *val;
    struct paramlist *next;
};
typedef struct paramlist paramlist;
paramlist *init_list(char *param, char *val);
paramlist *add(paramlist *head, char *pname, char *pval);
void print_list(paramlist *head);
void free_list(paramlist *head);
int length(paramlist *head);
paramlist* get_param(paramlist *head, char *param_name);
char* get_param_val(paramlist *head, char *param_name);
char* get_string_param(paramlist* head, char* param_name);
double get_double_param(paramlist* head, char* param_name);
int get_int_param(paramlist* head, char* param_name);

#endif
