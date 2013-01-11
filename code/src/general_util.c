#include "estimator.h"
#include <stdlib.h>
#include <string.h>

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
	    len = 0; // reset length
	    dup += 1; // skip over the separator
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

void print_int_arr(int *arr, int len)
{
    int i;
    
    for (i = 0; i < len; ++i) {
	printf("%d\n", arr[i]);
    }

}

void print_double_arr(double *arr, int len)
{
    int i;
    
    for (i = 0; i < len; ++i) {
	printf("%lf\n", arr[i]);
    }

}

/*
 * Checks whether a given interval is valid and returns 1 if so. The start of the interval
 * must be before the end, and both start and end must be positive
 */
int interval_valid(double interval_start, double interval_end)
{
    return (interval_start < interval_end) && interval_start >= 0 && interval_end > 0;
}

/*
 * Gets a subinterval of the given event array. The array provided is not modified.
 * This should be used to prevent excessive reading from file. Very naive - complexity
 * probably O(n^2). Probably marginally better than processing files.
 *
 * Note that an unmodified event array is expected - i.e. one which contains the length in
 * the zeroth location.
 */
double* get_event_subinterval(double *events, double interval_start, double interval_end)
{

    if (!interval_valid(interval_start, interval_end) || events == NULL)
	return NULL;
    
    int count = 1;
    double *start = NULL;
//    double *end = NULL;
    
    int arr_end = events[0];

    int i;
    double cur_time;
    
    //printf("interval start %lf, interval end %lf\n", interval_start, interval_end);
        
    for (i = 1, cur_time = events[i]; (i < arr_end) && (events[i] <= interval_end); ++i, cur_time = events[i]) {
	//printf("cur_time %lf, i %d, i > arr_end %d, i < arr_end %d\n", cur_time, i, i > arr_end, i < arr_end);
	if (cur_time >= interval_start && start == NULL){
	    //printf("interval start detected at %lf\n", cur_time);
	    start = (events + i); // start pointer moves to indicate the start of the interval
	    count++;
	} else if (cur_time > interval_end){
	    //printf("interval end detected at %lf\n", cur_time);
	    count++;
	    break;
	} else if (cur_time > interval_start && cur_time < interval_end){
	    count++;
	}
    }
    
    // If we didn't find any events within the specified interval
    if (count == 1)
	return NULL;

    double *pruned_events = malloc((count + 1) * sizeof(double));
    pruned_events[0] = count;

    //printf("end - start %d\n", end - start);
    
    memcpy((void*)(pruned_events + 1), start, count * sizeof(double));
    
    
    /* for (i = 0; i < count; ++i) { */
    /* 	printf("%lf, %d\n", pruned_events[i], i); */
    /* } */

    //printf("interval start %lf, first event: %lf, interval end %lf, last event %lf, count %d\n", interval_start, *start, interval_end, *(end - 1), count);

    return pruned_events;
}

/*
 * Prints the estimates contained within the given estimate array
 */
void print_estimates(est_arr *estimates)
{
    int len = estimates->len;
    
    int i;
    
    for (i = 0; i < len && estimates->estimates[i] != NULL; ++i) {
	printf("Interval %d estimate is:\na: %lf\nb: %lf\nstart: %lf\nend: %lf\n\n", i, estimates->estimates[i]->est_a, estimates->estimates[i]->est_b, estimates->estimates[i]->start, estimates->estimates[i]->end);
    }

}

/*
 * Frees an est_arr struct and all the pointers that it contains.
 */
void free_est_arr(est_arr *estimates)
{
    int len = estimates->len;
    
    int i;
    
    for (i = 0; i < len && estimates->estimates[i] != NULL; ++i) {
	free(estimates->estimates[i]);
    }
    
    free(estimates->estimates);
    free(estimates);
}

/*
 * Prints an array of strings with a message before it.
 */
void print_string_array(char* message, char** array, int len)
{
    int i;
    
    printf("%s\n", message);
    
    for (i = 0; i < len; ++i) {
	printf("%s\n", array[i]);
    }

}
