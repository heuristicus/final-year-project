#include "poisson.h"
#include <time.h>

#define MAX_DATE_LENGTH 26
#define MAX_PARAM_STRING_LENGTH 100

int main(int argc, char *argv[])
{

    get_parameters("params.txt");
        
    return 0;
}


char* generate_outfile()
{
    char* prefix = "poisson_output";
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
void get_parameters(char* filename)
{
    char *line = malloc(MAX_PARAM_STRING_LENGTH * sizeof(char));
    FILE *fp = fopen(filename, "r");
    int pnamelen, pvallen, i, arrsize;

    line = fgets(line, 10, fp);
    arrsize = atoi(line);

    if (arrsize == 0){
	printf("Number of parameters in parameter file not provided. The first line of the file should be a single integer representing the number of parameters in the file.");
	exit(1);
    }
    
    char **params = malloc(arrsize * sizeof(char*));
    char **values = malloc(arrsize * sizeof(char*));
        
    for (i = 0; (line = fgets(line, MAX_PARAM_STRING_LENGTH, fp)) != NULL; ++i){
	// Attempt at dynamic allocation of parameter lists. Try again later?
	/* if (i >= arrsize){ */
	/*     char** tmp = malloc(arrsize * sizeof(char)); */
	/*     char** tmp2 = malloc(arrsize * sizeof(char)); */
	    	    
	/*     for (j = 0; j < arrsize; ++j){ */
	/* 	tmp[j] = malloc(strlen(params[j])); */
	/* 	//tmp2[j] = malloc(strlen(values[j])); */

	/* 	tmp[j] = params[j]; */
	/* 	//tmp2[j] = values[j]; */
	/*     } */
	    
	/*     params = realloc(params, arrsize * 2); */
	/*     values = realloc(values, arrsize * 2); */
	
	/*     for (j = 0; j < arrsize; ++j){ */
	/* 	params[j] = tmp[j]; */
	/* 	//values[j] = tmp2[j]; */
	/*     } */
	/*     arrsize *= 2; */
	/*     params = tmp; */
	/*     values = tmp2; */
	    
	/*     free(tmp); */
	/*     free(tmp2); */
	/* } */
	pnamelen = strchr(line, ' ') - line;
	params[i] = malloc(pnamelen * sizeof(char));
	pvallen = strchr(line, '\0') - strchr(line, ' ');
	values[i] = malloc(pvallen * sizeof(char));
	
	strncpy(params[i], line, pnamelen);
	strncpy(values[i], line + pnamelen + 1, pvallen);
	
	printf("%s\n", params[i]);
	printf("%s\n", values[i]);
    }

    free(params);
    free(values);
    free(line);
        	
}

void double_to_file(char* filename, char* mode, double* arr1, double* arr2, int len)
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

void int_to_file(char* filename, char* mode, int* arr1, int* arr2, int len)
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

/* /\* Prints the given arrays to the specified file. Two newlines will be added to */
/*    the end to allow for indexing in gnuplot. The arrays are assumed to be of */
/*    the same length. arr_type should contain two characters indicating the types */
/*    of the arrays. c = char, f = double, s = string, d = int. */
/* *\/ */
/* void standard_out(char* filename, void* arr1, void* arr2, int len, char* arr_type) */
/* { */
/*     FILE *fp; */
    
/*     if ((fp = fopen(filename, "w")) == NULL){ */
/* 	perror("Could not open file."); */
/* 	exit(1); */
/*     } */
    
/*     int i; */
    
/*     char* fstr1 = malloc(2); */
/*     char* fstr2 = malloc(2); */
    
/*     sprintf(fstr1, "%c", arr_type[0]); */
/*     sprintf(fstr2, "%c", arr_type[1]); */
    
/*     printf("%s ", fstr1); */
/*     printf("%s\n", fstr2); */
/*     for (i = 0; i < len; ++i){ */
/* 	fprintf(fp, fstr1, arr1[i]); */
/* 	fprintf(fp, fstr2, arr2[i]) */
/*     } */

/*     fprintf(fp, "\n\n"); */
    
/* } */

