#include "file_util.h"

#define MAX_DATE_LENGTH 32
#define MAX_PARAM_STRING_LENGTH 500
#define MAX_LINE_LENGTH 100
#define DEFAULT_ARR_SIZE 100
#define PARAM_SEPARATOR " "

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
	
	if (plist == NULL) {

	    if (value == NULL){
		printf("No value found for %s.\n", param);
		plist = init_list(param, "<empty>");
	    } else {
		plist = init_list(param, value);	
	    }
	} else {
	    if (value == NULL){
		printf("No value found for %s.\n", param);
		plist = add(plist, param, "<empty>");
	    } else {
		plist = add(plist, param, value);
	    }
	}
    }
        
    free(lp);
    fclose(fp);

    return plist;
}

/*
 * Gets event data from the specified file. The file is assumed to contain only
 * data on event times.
 * passing duplicate values into start_time and end_time will return all values.
 */
double_arr* get_event_data_interval(double start_time, double end_time, char *filename)
{
    FILE *fp;

    printf("Retrieving event data from file %s\n", filename);
    //printf("%lf, %lf\n", start_time, end_time);


    if ((fp = fopen(filename, "r")) == NULL){
	perror("Could not access specified file");
	exit(1);
    }
#ifdef VERBOSE
    printf("Getting event data for interval [%lf, %lf]\n", start_time, end_time);
#endif
    char* line = malloc(MAX_LINE_LENGTH);
    char* lref = line;
    double_arr* event_times = init_double_arr(DEFAULT_ARR_SIZE);
    int all = start_time == end_time; // Whether we want to get all data or not.

    if (!all && !interval_valid(start_time, end_time)){
	printf("ERROR: Interval passed to get_event_data_interval is invalid: start %lf, end %lf\n", start_time, end_time);
	free(lref);
	fclose(fp);
	return NULL;
    }
    
    int i = 0, max_size = DEFAULT_ARR_SIZE;
    while ((line = fgets(line, MAX_LINE_LENGTH, fp)) != NULL){
	if (strcmp(line, "\n") == 0)
	    break; // A newline indicates the end of the data.
	if (i == max_size){
	    event_times->data = realloc(event_times->data, max_size * 2 * sizeof(double));
	    max_size *= 2;
	}

	if (!all){
	    // Don't add the event time if it is before the time we have designated as the start time
	    if (atof(line) < start_time)
		continue; 
	    if (atof(line) > end_time)
		break; // Don't add data past the specified end time.
	}

	event_times->data[i] = atof(line);
	i++;
    }

    if (i == 0){// we found no events in the given interval
	free_double_arr(event_times);
	free(lref);
	fclose(fp);
	return NULL;
    }
    event_times->data = realloc(event_times->data, i * sizeof(double)); // Potentially save memory?
    event_times->len = i;
    fclose(fp);
    free(lref);

    return event_times;
}

/*
 * Gets all event data from the specified file.
 */
double_arr* get_event_data_all(char *filename)
{
    return get_event_data_interval(0.0, 0.0, filename);
}


/*
 * Checks whether a line received from the parameter file is valid.
 */
int valid_param(char* pname)
{
    int spacecount;
    
    for (spacecount = 0; *pname != '\0'; ++pname){
	// 10 is the backspace character - seems to come up from time to time
	if ((32 < *pname && 126 > *pname) || *pname == 10) {
	    continue;
	} else if (*pname == ' '){
	    spacecount++;
	} else {
	    printf("invalid char %c\n", *pname);
	    return 0;
	}
	
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
    
#ifdef VERBOSE 
    printf("int end %lf, int start %lf\n", end, start);
#endif
    
    FILE *fp = fopen(filename, mode);
        
    while(counter < end){
	fprintf(fp, "%lf %lf\n", counter, a + counter * b);
#ifdef VERBOSE3
	printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
#endif
	if (end - counter <= 1){
	    counter += end - counter;
#ifdef VERBOSE3
	    printf("%lf + %lf * %lf = %lf\n", a, counter, b, a + counter * b);
#endif
	    fprintf(fp, "%lf %lf\n", counter, a + counter * b);
	} else {
	    counter += 1;
	}
#ifdef VERBOSE3
	printf("counter: %lf, end %lf\n", counter, end);
#endif
    }

    fprintf(fp, "\n\n");
    
    fclose(fp);
}

/*
 * Outputs a 2d array created by the gaussian transform function to a file. The 
 * zeroth index contains the point on the x-axis, and the first contains the 
 * total contribution of gaussians at that point. The mode of addition to the file
 * is specified by the mode parameter. The data will be shifted along the y-axis
 * according to the shift parameter. The contribution at each point can be normalised
 * by providing a normalising constant.
 */
void output_gauss_transform(char* filename, char* mode, double** T, double shift,
			    int len, double normaliser)
{
    FILE* fp;
    
    if (normaliser == 0){
	printf("Received normalising constant of 0. Setting to 1.\n");
	normaliser = 1;
    }
    
    printf("Outputting gauss transform to %s.\n", filename);

    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file");
	return;
    }

    int i;

    for (i = 0; i < len; ++i) {
	fprintf(fp, "%lf %lf\n", T[0][i], (T[1][i] + shift)/normaliser);
    }

    if (fclose(fp)){
	printf("error closing\n");
    }
}

/*
 * Outputs the given vector of gaussians to a file. The apply_weight nparameter
 * specifies whether the weights in the vector should be applied or if the 
 * gaussians should be output in their raw form - a non-zero value applies them.
 * The contribution of gaussians is checked in the interval [start, end], with the
 * given resolution
 */
void output_gaussian_contributions(char* filename, char* mode, gauss_vector* G, double start,
				   double end, double resolution, int apply_weight)
{
    /* if (!interval_valid(start, end)){ */
    /* 	printf("Invalid interval [%lf, %lf] when outputting gaussians to %s.\n", */
    /* 	       start, end, filename); */
    /* 	return; */
    /* } */

    FILE *fp = fopen(filename, mode);

    int i;
    double current;
    
    for (i = 0; i < G->len; ++i) {
	for (current = start; current <= end; current += resolution) {
	    if (apply_weight){
		fprintf(fp, "%lf %lf\n", current, gaussian_contribution_at_point(current, G->gaussians[i], G->w[i]));
	    } else {
		fprintf(fp, "%lf %lf\n", current, gaussian_contribution_at_point(current, G->gaussians[i], 1));
	    }
	    
	}
	fprintf(fp, "\n\n");
    }

    fclose(fp);
}

/*
 * Writes a vector of gaussians to file. Data looks like
 * xpos stdev weight
 */
void output_gaussian_vector(char* filename, char* mode, gauss_vector* V)
{
    int i;
    FILE *fp = fopen(filename, mode);
    
    for (i = 0; i < V->len; ++i) {
	fprintf(fp, "%lf %lf %lf\n", V->gaussians[i]->mean, V->gaussians[i]->stdev, V->w[i]);
    }

    fclose(fp);
}

/*
 * Reads data in structured in the following way into a vector of gaussians
 * xpos stdev weight
 */
gauss_vector* read_gauss_vector(char* filename)
{
    gauss_vector* ret = malloc(sizeof(gauss_vector));
    
    int memsize = DEFAULT_ARR_SIZE;
    
    gaussian** G = malloc(sizeof(gaussian*) * memsize);
    double* wts = malloc(sizeof(double) * memsize);

    
    printf("Reading vector of gaussians from %s\n", filename);
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
	perror("File does not exist");
	exit(1);
    }
    
    int i = 0;
    
    char* line = malloc(MAX_PARAM_STRING_LENGTH);
    char* d = line; // Need to keep a reference to the line pointer to free later

    double st, wt, mu;
    
    while ((line = fgets(line, MAX_PARAM_STRING_LENGTH, fp)) != NULL){
	int res = sscanf(line, "%lf %lf %lf", &mu, &st, &wt);
	if (res != 3){
	    printf("Error while reading gaussians data from %s. Make sure that"\
		   " the file is in the format \"xpos stdev weight\" and try"\
		   " again.\n", filename);
	    return NULL;
	} else {
	    gaussian* g = malloc(sizeof(gaussian));
	    g->mean = mu;
	    g->stdev = st;
	    wts[i] = wt;
	    G[i] = g;
#ifdef VERBOSE
	    printf("Gaussian %d mean: %lf, stdev: %lf, weight: %lf\n", G[i]->mean, G[i]->stdev, wts[i]);
#endif
	}
	i++;
	if (i > memsize){
	    memsize *= 2;
	    G = realloc(G, sizeof(gaussian*) * memsize);
	    wts = realloc(wts, sizeof(double) * memsize);
	}
    }

    free(d);
    fclose(fp);
    
    ret->gaussians = realloc(G, sizeof(gaussian*) * i);
    ret->len = i;
    ret->w = realloc(wts, sizeof(double) * i);

    return ret;
}

void output_double_multi_arr(char* filename, char* mode, double_multi_arr* arr)
{
    int minlen = find_min_value_int(arr->lengths, arr->len);
    FILE *fp = fopen(filename, mode);
    int i, j;
    
    printf("Outputting to %s.\n", filename);
    
    for (i = 0; i < minlen; ++i) {
	for (j = 0; j < arr->len; ++j) {
	    if (j + 1 == arr->len){
		fprintf(fp, "%lf\n", arr->data[j][i]);
	    } else {
		fprintf(fp, "%lf ", arr->data[j][i]);
	    }
	}
    }
    fprintf(fp, "\n\n");

    fclose(fp);
}

/*
 * Creates a file with the filename specified in the directory given. If the
 * directory does not exist it is created. If all goes well, returns zero, 
 * else returns -1. The directory is created with read, write and execute 
 * permissions for the user.
 */
int create_file_in_dir(char* filename, char* dirname)
{
    int r = mkdir(dirname, S_IRWXU);
    
    if (r == 0 || errno == EEXIST){
	char* fullpath = malloc(strlen(filename) + strlen(dirname) + 1);
	sprintf(fullpath, "%s/%s", dirname, filename);
	int file = open(fullpath, O_CREAT | O_EXCL, S_IRWXU | S_IROTH | S_IRGRP);
	
	if (file == -1)
	    return -1;
	else
	    close(file);
	
	return 0;
    }

    return -1;
}

/*
 * Creates a directory. Returns 1 if the directory already existed and the
 * permissions are valid, or was successfully created, 0 otherwise.
 */
int create_dir(char* dirname)
{
    int r = mkdir(dirname, S_IRWXU);
    DIR* x;
    
    
    if (r == 0 || errno == EEXIST){
	if ((x = opendir(dirname)) == NULL){
	    return 0;
	}
	closedir(x);
	return 1;
    }

    return 0;
}

/*
 * Checks whether a file exists. 1 if it does, 0 if not.
 */
int file_exists(char* filename)
{
    if (access(filename, F_OK) != -1)
	return 1;
    else
	return 0;
}

/*
 * Checks if a directory exists and is accessible. 1 if yes, 0 otherwise.
 */
int dir_exists(char* dirname)
{
    struct stat s;
    
    int err = stat(dirname, &s);
    
    if (err == -1){
	if (errno == ENOENT){
	    return 0;
	} else {
	    perror("Error accessing directory.");
	    return 0;
	}
    } else {
	if (S_ISDIR(s.st_mode)){
	    return 1;
	}
    }
    return 0;
}

/*
 * Ouputs the parameter names and values to a file.
 */
void list_to_file(char* filename, char* mode, paramlist* param)
{
    int i;

    paramlist* l = param;
    
    FILE *fp = fopen(filename, mode);

    if (fp == NULL){
	printf("Could not open file %s.\n", filename);
	perror("Error");
	return;
    }

    for (i = 0; i < list_length(l); ++i) {
	fprintf(fp, "%s %s\n", l->par, l->val);
	l = l->next;
    }

    fclose(fp);
}
