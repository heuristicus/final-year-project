#include "poisson.h"

/* THIS FUNCTION SHOULD ONLY BE CALLED FROM FUNCTIONS IN THIS FILE.
   initialises the list and returns a pointer to the start of the list */
paramlist* init_list(char *param, char *val)
{

    printf("%s %s\n", param, val);
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
    paramlist *new = malloc(sizeof(paramlist));
    char *pname = malloc(strlen(param) + 1);
    char *pval = malloc(strlen(value) + 1);

    strcpy(pname, param);
    strcpy(pval, value);
                
    printf("orginal: %s %s, copied: %s %s\n", param, value, pname, pval);

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

/* Goes through the list, freeing all allocated memory. */
void free_list(paramlist *head)
{
    paramlist *cur = head;
    paramlist *prev;
    
    while (cur != NULL){
	prev = cur;
	cur = cur->next;
	printf("freeing %s, %s\n", prev->par, prev->val);
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
