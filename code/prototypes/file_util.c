#include "file_util.h"
#include <string.h>
#include <time.h>

#define MAX_DATE_LENGTH 26
#define MAX_PARAM_STRING_LENGTH 100
#define MAX_LINE_LENGTH 100
#define DEFAULT_ARR_SIZE 100
#define PARAM_SEPARATOR " "

int main(int argc, char *argv[])
{
    /* paramlist *p; */
    
    /* p = get_parameters("params.txt"); */
        
    /* print_list(p); */
    /* free_list(p); */

    get_event_data(argv[1]);
    
    return 0;
}


/* 
 * Creates a filename to use for data output.
 */
char* generate_outfile()
{
    char* prefix = "output_poisson";
    char* datetime = malloc(MAX_DATE_LENGTH * sizeof(char));
    char* fname = malloc((MAX_DATE_LENGTH + strlen(prefix)) * sizeof(char));
    time_t timer = time(NULL);
    
    strftime(datetime, MAX_DATE_LENGTH, "%d-%m-%Y_%H:%M:%S", localtime(&timer));
    sprintf(fname, "%s_%s.dat", prefix, datetime);

    free(datetime);
    
    return fname;
        
}


/* 
 * Gets parameters from a file. The first line of the parameter file should be an
 * integer representing the number of parameters to be read in.
 */
paramlist* get_parameters(char* filename)
{
    FILE *fp = fopen(filename, "r");
    
    char *line = malloc(MAX_PARAM_STRING_LENGTH * sizeof(char));
    char *lp = line; // strtok messes around with pointers so keep reference to line.
    char *param;
    char *value;

    paramlist *plist = NULL;

    while ((line = fgets(line, MAX_PARAM_STRING_LENGTH, fp)) != NULL){
	// WARNING: Do not use strtok on literals!
	if (!valid_param(line)){
	    printf("Invalid parameter: %s\n", line);
	    continue;
	}

	param = strtok(line, " ");
	value = strtok(NULL, "\n");
	
	if (plist == NULL)
	    plist = init_list(param, value);
	else
	    plist = add(plist, param, value);

    }
        
    free(lp);
    fclose(fp);

    return plist;
                
}

/*
 * Gets event data from the specified file. The file is assumed to contain only
 * data on event times. The size of the array will be stored in the zeroth index - this
 * does not include that value itself. For example, if there are 10 events, the actual
 * length will be 11, because of the length in index 0, but the value of the length will be 10.
 */
double* get_event_data(char *filename)
{
    FILE *fp = fopen(filename, "r");
    
    char *line = malloc(MAX_LINE_LENGTH);
    double *event_times = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    
    int i = 1, max_size = DEFAULT_ARR_SIZE;
    while ((line = fgets(line, MAX_LINE_LENGTH, fp)) != NULL){
	if (strcmp(line, "\n") == 0)
	    break; // A newline indicates the end of the data.
	if (i == max_size){
	    event_times = realloc(event_times, max_size * 2 * sizeof(double));
	    max_size *= 2;
	}
	event_times[i] = atof(line);
	i++;
    }

    event_times[0] = i + 1;// Store the length at the start of the array.
    event_times = realloc(event_times, i * sizeof(double)); // Potentially save memory?
    
    fclose(fp);
    free(line);
    
    return event_times;
}

/*
 * Checks whether a line received from the parameter file is valid.
 */
int valid_param(char *pname)
{
    int spacecount;
    
    for (spacecount = 0; *pname != '\0'; ++pname){
	// 10 is the backspace character - seems to come up from time to time
	if ((32 < *pname && 126 > *pname) || *pname == 10)
	    continue;
	else if (*pname == 32)
	    spacecount++;
	else
	    printf("invalid char %d\n", *pname);
	
	if (spacecount > 1)
	    return 0;
    }
    
    if (spacecount != 1)
	return 0;
    
    return 1;
    
}

/*
 * Writes two arrays of doubles to the specified file.
 */
void mult_double_to_file(char* filename, char* mode, double* arr1, double* arr2, int len)
{

    FILE *fp;
        
    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file.");
	exit(1);
    }
    
    int i;

    for (i = 0; i < len; ++i){
	fprintf(fp, "%lf %lf\n", arr1[i], arr2[i]);
    }

    fprintf(fp, "\n\n");

    fclose(fp);
    
}

/*
 * Writes two arrays of integers to the specified file.
 */
void mult_int_to_file(char* filename, char* mode, int* arr1, int* arr2, int len)
{
    FILE *fp;
    
    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file.");
	exit(1);
    }
    
    int i;

    for (i = 0; i < len; ++i){
	fprintf(fp, "%d %d\n", arr1[i], arr2[i]);
    }

    fprintf(fp, "\n\n");

    fclose(fp);
    
}

void double_to_file(char *filename, char *mode, double *arr, int len)
{
    FILE *fp;
    
    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file.");
	exit(1);
    }
    
    int i;

    for (i = 0; i < len; ++i){
	fprintf(fp, "%lf\n", arr[i]);
    }

    fprintf(fp, "\n\n");

    fclose(fp);
}

/* /\* Prints the given array to the specified file. Two newlines will be added to */
/*  * the end to allow for indexing in gnuplot.  */
/*  * The format string should contain the correct  */
/*  *\/ */
/* void arr_to_file(char* filename, void* arr, int len, char* format_string) */
/* { */
/*     FILE *fp; */
    
/*     if ((fp = fopen(filename, "w")) == NULL){ */
/* 	perror("Could not open file."); */
/* 	exit(1); */
/*     } */
    
/*     int i; */

/*     for (i = 0; i < len; ++i){ */
/* 	fprintf(fp, format_string, arr[i]); */
/*     } */

/*     fprintf(fp, "\n\n"); */

/*     fclose(fp); */
        
/* } */

