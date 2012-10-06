#include "poisson.h"
#include <time.h>

#define MAX_DATE_LENGTH 26

/* int main(int argc, char *argv[]) */
/* { */
    
/*     double a[3] = {1.0,2.0,3.0}; */
/*     double b[3] = {2,3,4}; */
    
/*     double_to_file("asdf", a, b, 3); */
    
/*     return 0; */
/* } */


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

