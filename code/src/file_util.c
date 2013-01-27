#include "file_util.h"
#include "general_util.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_DATE_LENGTH 32
#define MAX_PARAM_STRING_LENGTH 500
#define MAX_LINE_LENGTH 100
#define DEFAULT_ARR_SIZE 100
#define PARAM_SEPARATOR " "

/* int main(int argc, char *argv[]) */
/* { */
/*     /\* paramlist *p; *\/ */
    
/*     /\* p = get_parameters("params.txt"); *\/ */
        
/*     /\* print_list(p); *\/ */
/*     /\* free_list(p); *\/ */

/*     double *events = get_event_data_interval(25.0, 50, argv[1]); */
    
/*     int i; */
    
/*     for (i = 0; i < (int) events[0]; ++i) { */
/* 	printf("%lf\n", events[i]); */
/*     } */


/*     return 0; */
/* } */

/* 
 * Creates a filename to use for data output with the format:
 * prefix_dd-mm-yyyy_hr:min:sec_usec.dat
 * the usec parameters specifies whether to add nanoseconds (0 is no)
 */
char* generate_outfile(char* prefix, int usec)
{
    char* datetime = malloc(MAX_DATE_LENGTH * sizeof(char));
    char* fname = malloc((MAX_DATE_LENGTH + strlen(prefix)) * sizeof(char));
    time_t timer = time(NULL);
    struct timeval t;

    gettimeofday(&t, NULL);

    strftime(datetime, MAX_DATE_LENGTH, "%d-%m-%Y_%H:%M:%S", localtime(&timer));
    
    if (usec){
	sprintf(fname, "%s_%s_%d.dat", prefix, datetime, (int) t.tv_usec);
    } else {
	sprintf(fname, "%s_%s.dat", prefix, datetime);
    }
    
    free(datetime);
    
    return fname;
        
}


/* 
 * Gets parameters from a file. The first line of the parameter file should be an
 * integer representing the number of parameters to be read in. Will ignore lines
 * which start with # (comments), and newlines. Note that inline comments are not
 * supported.
 */
paramlist* get_parameters(char* filename)
{
    FILE *fp;
    
    printf("Retrieving parameters from file %s\n", filename);
    if ((fp = fopen(filename, "r")) == NULL){
	perror("Could not access specified file");
	exit(1);
    }
    
    char *line = malloc(MAX_PARAM_STRING_LENGTH * sizeof(char));
    char *lp = line; // strtok messes around with pointers so keep reference to line.
    char *param;
    char *value;

    paramlist *plist = NULL;

    while ((line = fgets(line, MAX_PARAM_STRING_LENGTH, fp)) != NULL){
	// WARNING: Do not use strtok on literals!
	if (*line == '#' || *line == '\n' || isspace(*line)) // ignore comments and newlines, and blank lines
	    continue;
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
 * passing duplicate values into start_time and end_time will return all values.
 */
double* get_event_data_interval(double start_time, double end_time, char *filename)
{
    FILE *fp;

    printf("Retrieving event data from file %s\n", filename);
    //printf("%lf, %lf\n", start_time, end_time);


    if ((fp = fopen(filename, "r")) == NULL){
	perror("Could not access specified file");
	exit(1);
    }
#ifdef DEBUG
    printf("Getting event data for interval [%lf, %lf]\n", start_time, end_time);
#endif
    char *line = malloc(MAX_LINE_LENGTH);
    double *event_times = malloc(DEFAULT_ARR_SIZE * sizeof(double));
    int all = start_time == end_time; // Whether we want to get all data or not.

    if (!all && !interval_valid(start_time, end_time)){
	printf("ERROR: Interval passed to get_event_data_interval is invalid: start %lf, end %lf\n", start_time, end_time);
	return NULL;
    }
    
    int i = 1, max_size = DEFAULT_ARR_SIZE;
    while ((line = fgets(line, MAX_LINE_LENGTH, fp)) != NULL){
	if (strcmp(line, "\n") == 0)
	    break; // A newline indicates the end of the data.
	if (i == max_size){
	    event_times = realloc(event_times, max_size * 2 * sizeof(double));
	    max_size *= 2;
	}

	if (!all){
	    // Don't add the event time if it is before the time we have designated as the start time
	    if (atof(line) < start_time)
		continue; 
	    if (atof(line) > end_time)
		break; // Don't add data past the specified end time.
	}

	event_times[i] = atof(line);
	i++;
    }

    event_times[0] = i;// Store the length at the start of the array.
    event_times = realloc(event_times, i * sizeof(double)); // Potentially save memory?
    
    fclose(fp);
    free(line);

    return event_times;
}

/*
 * Gets all event data from the specified file.
 */
double* get_event_data_all(char *filename)
{
    return get_event_data_interval(0.0, 0.0, filename);
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
	fprintf(fp, "%.15lf %.15lf\n", arr1[i], arr2[i]);
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
/*
 * Writes two arrays of integers to the specified file.
 */
void int_dbl_to_file(char* filename, char* mode, double* arr1, int* arr2, int len)
{
    FILE *fp;
    
    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file.");
	exit(1);
    }
    
    int i;

    for (i = 0; i < len; ++i){
	fprintf(fp, "%.15lf %d\n", arr1[i], arr2[i]);
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
	fprintf(fp, "%.15lf\n", arr[i]);
    }

    fprintf(fp, "\n\n");

    fclose(fp);
}

void double_mult_dim_to_file(char* filename, char* mode, double_mult_arr* arr)
{
    FILE *fp;
    
    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file.");
	exit(1);
    }
    
    int i;

    for (i = 0; i < arr->len; ++i){
	fprintf(fp, "%.15lf, %.15lf\n", arr->data1[i], arr->data2[i]);
    }

    fprintf(fp, "\n\n");

    fclose(fp);
}

/*
 * Outputs a series of estimates to separate files.
 */
void output_estimates(char *filename, est_data **estimates, int len)
{
    int i;
    //char *f;
        
    for (i = 0; i < len; ++i) {
	//f = malloc(strlen(filename) + 4);
	//sprintf(f, "%s_%d", filename, i);
	//estimate_to_file(filename, estimates[i], "w");
	
	// There might be fewer lines than expected due to extension
	if (estimates[i] == NULL){
	    break;
	}
	// Write to file if it's the first run, otherwise append
	estimate_to_file(filename, estimates[i], i > 0 ? "a" : "w");
    }
}

/*
 * Prints a single set of estimates to an output file. This will calculate
 * the value of the function for each second within the interval. The mode
 * specifies the mode to open the file with, as for the fopen function.
 */
void estimate_to_file(char *filename, est_data *estimate, char *mode)
{

    if (estimate == NULL)
	return;
        
    double a = estimate->est_a;
    double b = estimate->est_b;
    double start = estimate->start;
    double end = estimate->end;
    
    // We will subtract from this value to make sure we cover the whole
    // length of the interval.
    double counter = start;
    
    printf("int end %lf, int start %lf\n", end, start);
    
    FILE *fp = fopen(filename, mode);
        
    while(counter < end){
	fprintf(fp, "%lf %lf\n", counter, a + counter * b);
#ifdef DEBUG
	printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
#endif
	if (end - counter <= 1){
	    counter += end - counter;
#ifdef DEBUG
	    printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
#endif
	    fprintf(fp, "%lf %lf\n", counter, a + counter * b);
	} else {
	    counter += 1;
	}
#ifdef DEBUG
	printf("counter: %lf, end %lf\n", counter, end);
#endif
    }

    fprintf(fp, "\n\n");
    
    fclose(fp);
}
