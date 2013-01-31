#include "file_util.h"

#define MAX_DATE_LENGTH 32
#define MAX_PARAM_STRING_LENGTH 500
#define MAX_LINE_LENGTH 100
#define DEFAULT_ARR_SIZE 100
#define PARAM_SEPARATOR " "

/* 
 * Creates a filename to use for data output with the format:
 * prefix_dd-mm-yyyy_hr:min:sec_usec.dat
 */
char* generate_outfile()
{
    char* prefix = "output_poisson";
    char* datetime = malloc(MAX_DATE_LENGTH * sizeof(char));
    char* fname = malloc((MAX_DATE_LENGTH + strlen(prefix)) * sizeof(char));
    time_t timer = time(NULL);
    struct timeval t;

    gettimeofday(&t, NULL);

    strftime(datetime, MAX_DATE_LENGTH, "%d-%m-%Y_%H:%M:%S", localtime(&timer));
    
    sprintf(fname, "%s_%s_%d.dat", prefix, datetime, (int) t.tv_usec);

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

/*
 * Outputs a 2d array created by the gaussian transform function to a file. The 
 * zeroth index contains the point on the x-axis, and the first contains the 
 * total contribution of gaussians at that point. The mode of addition to the file
 * is specified by the mode parameter. The data will be shifted along the y-axis
 * according to the shift parameter.
 */
void output_gauss_transform(char* filename, char* mode, double** T, double shift, int len)
{
    FILE* fp;
    
    printf("Outputting gauss transform to %s.\n", filename);

    if ((fp = fopen(filename, mode)) == NULL){
	perror("Could not open file");
	return;
    }

    int i;

    for (i = 0; i < len; ++i) {
	fprintf(fp, "%lf %lf\n", T[0][i], T[1][i] + shift);
    }

    if (fclose(fp)){
	printf("error closing\n");
    }
}

/*
 * Outputs the given vector of gaussians to a file. The apply_weight parameter
 * specifies whether the weights in the vector should be applied or if the 
 * gaussians should be output in their raw form - a non-zero value applies them.
 * The contribution of gaussians is checked in the interval [start, end], with the
 * given resolution
 */
void output_gaussian_contributions(char* filename, char* mode, gauss_vector* G, double start,
				   double end, double resolution, int apply_weight)
{
    if (!interval_valid(start, end)){
	printf("Invalid interval [%lf, %lf] when outputting gaussians to %s.\n",
	       start, end, filename);
	return;
    }

    FILE *fp = fopen(filename, mode);

    printf("Outputting gaussian data to %s.\n", filename);
    
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
void output_gaussian_vector(char* filename, gauss_vector* V, char* mode)
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
    
    FILE *fp = fopen(filename, "r");
    int i = 0;
    
    char *line = malloc(MAX_PARAM_STRING_LENGTH * sizeof(char));

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
	    printf("%lf %lf %lf\n", G[i]->mean, G[i]->stdev, wts[i]);
	}
	i++;
	if (i > memsize){
	    memsize *= 2;
	    G = realloc(G, sizeof(gaussian*) * memsize);
	    wts = realloc(wts, sizeof(double) * memsize);
	}
    }

    
    ret->gaussians = G;
    ret->len = i;
    ret->w = wts;

    printf("out %lf %lf %lf\n", ret->gaussians[0]->mean, ret->gaussians[0]->stdev, ret->w[0]);
    // realloc to get the correct memory size.
    G = realloc(G, sizeof(gaussian*) * i);
    wts = realloc(wts, sizeof(double) * i);

    fclose(fp);

    printf("%lf %lf %lf\n", ret->gaussians[0]->mean, ret->gaussians[0]->stdev, ret->w[0]);

    return ret;
}
