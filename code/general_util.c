#include "generator.h"
#include "estimator.h"

#define DEF_ARR_SIZE 5

/*
 * Frees a pointer array
 */
void free_pointer_arr(void **arr, int length)
{
    int i;
    
    for (i = 0; i < length; ++i){
	free(arr[i]);
    }
    free(arr);
}

/*
 * Splits a string on a separator. The return array will have the number
 * of elements in the array in its first location.
 */
char** string_split(char *string, char separator)
{
    char *dup = strdup(string);
    char *ref = dup; // so we can free later
    
    // Array size will be stored in the first array location
    char **split = malloc(DEF_ARR_SIZE * sizeof(char*));
    
    int max_size = DEF_ARR_SIZE;
    int len = 0;
    int size = 1;
    int exit = 0;
    
    while (!exit){
	if (*dup == separator || *dup == '\0'){
	    if (size >= max_size){ // resize the array if we go over size limit
		split = realloc(split, (max_size *= 2) * sizeof(char*));
	    }
	    split[size] = malloc(len + 1); // allocate space for string and null terminator
	    snprintf(split[size], len + 1, "%s", string);
	    size++;
	    if (*dup == '\0'){
		break;
	    }
	    string += len + 1; // move pointer of string over
	    len = 1; // reset length
	    dup += 2; // skip over the separator
	} else {
	    len++;
	    dup++;
	}
    }

    free(ref);
        
    split = realloc(split, size * sizeof(char*)); // save memory
    split[0] = malloc(20); // size is maximum of 20 chars long (probably overkill)
    snprintf(split[0], 20, "%d", size);
    
    return split;
}
