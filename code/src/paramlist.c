#include "paramlist.h"

/* initialises the list and returns a pointer to the start of the list */
paramlist* init_list(char *param, char *val)
{
    paramlist *head = malloc(sizeof(paramlist));
    char *p = malloc(strlen(param) + 1);
    char *v = malloc(strlen(val) + 1);
    
    p = strcpy(p, param);
    v = strcpy(v, val);
    
    head->par = p;
    head->val = v;
    head->next = NULL;
        
    return head;
}

paramlist* add(paramlist *head, char *param, char *value)
{
    // Check if the same item is already present, and do not add it if so.
    // error if the same param is passed in with different values.
    paramlist* c = get_param(head, param);
    if (c != NULL && strcmp(c->par, param) == 0 && strcmp(c->val, value) != 0){
	printf("WARNING: There are two parameters with the same name (%s) "\
	       "but different values. Please check your param file.\n", param);
	exit(1);
    }
    if (c != NULL && strcmp(c->val, value) == 0){
	printf("WARNING: Duplicate parameter detected.\n");
	return head;
    }

    paramlist *new = malloc(sizeof(paramlist));
    char *pname = malloc(strlen(param) + 1);
    char *pval = malloc(strlen(value) + 1);

    strcpy(pname, param);
    strcpy(pval, value);

    new->par = pname;
    new->val = pval;
    new->next = head;

    return new;
}

/* Print the given list to stdout. */
void print_list(paramlist *head)
{
    paramlist *node = head;
    
    while (node != NULL){
	printf("%s = %s\n", node->par, node->val);
	node = node->next;
    }
}

/*
 * Return the value of the specified parameter as an integer.
 * Returns 0 if the value of the parameter is not an integer, or -1 if the parameter does not exist.
 */
int get_int_param(paramlist* head, char* param_name)
{
    paramlist* ret;
    
    if ((ret = get_param(head, param_name)) == NULL)
	return -1;
    else
	return atoi(get_param(head, param_name)->val);
}

/*
 * Return the value of the specified parameter as a double.
 * Returns 0.0 if the value of the parameter is not an integer, or -1 if the parameter does not exist.
 */
double get_double_param(paramlist* head, char* param_name)
{
    paramlist* ret;
    
    if ((ret = get_param(head, param_name)) == NULL)
	return -1;
    else
	return atof(get_param(head, param_name)->val);
}

/*
 * Return the value of the specified parameter as a string.
 * Returns a null pointer if the parameter does not exist.
 */
char* get_string_param(paramlist* head, char* param_name)
{
    paramlist *ret;
    
    if ((ret = get_param(head, param_name)) == NULL)
	return NULL;

    return ret->val;
}

/*
 * Gets the pointer to the struct whose name is the given string.
 * Returns null if there is no such struct.
 */
paramlist* get_param(paramlist *head, char *param_name)
{
    paramlist *cur = head;
    
    while(cur != NULL){
	if (strcmp(cur->par, param_name) == 0)
	    return cur;
	cur = cur->next;
    }
    
    return NULL;
}

/* Goes through the list, freeing all allocated memory. */
void free_list(paramlist *head)
{
    paramlist *cur = head;
    paramlist *prev;
    
    while (cur != NULL){
	prev = cur;
	cur = cur->next;
	free(prev->par);
	free(prev->val);
	free(prev);
    }

}

/* Returns the length of the list.  */
int length(paramlist* head)
{
    int len;

    for (len = 0; head != NULL; ++len, head = head->next);
    
    return len;
}
