#ifndef _STRUCT_H
#define _STRUCT_H

// Make representing gaussians easier.
typedef struct
{
    double mean;
    double stdev;
} gaussian;

// Struct to store a set of gaussians.
typedef struct
{
    int len;
    gaussian** gaussians;
    double* w;
} gauss_vector;

typedef struct
{
    double est_a;
    double est_b;
    double start;
    double end;
} est_data;

typedef struct
{
    est_data** estimates;
    int len;
} est_arr;

struct paramlist 
{
    char *par;
    char *val;
    struct paramlist *next;
};
typedef struct paramlist paramlist;

typedef struct
{
    double* data;
    int len;
} double_arr;


#endif
