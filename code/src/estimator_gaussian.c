#include "estimator.h"

/*
 * Estimates the given set of events using gaussian kernels, returning the
 * gaussian transform of the kernels.
 */
double_multi_arr* estimate_gaussian(paramlist* params, char* infile, char* outfile)
{
    double stdev = get_double_param(params, "gauss_est_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double resolution = get_double_param(params, "gauss_est_resolution");

    gauss_vector* G = _estimate_gaussian_raw(infile, outfile, start, interval, stdev, resolution);
    double_multi_arr* T = gauss_transform(G, start, start + interval, resolution);

    double min = find_min_value(T->data[1], T->lengths[1]);
    double shift = 0;
    if (min <= 0){
    	shift = -min;
    }
    
    if (outfile != NULL){
	printf("outputting stuff\n");
	char* out = malloc(strlen(outfile) + strlen("_contrib.dat") + 3);
	sprintf(out, "%s_contrib.dat", outfile);
	output_gaussian_contributions(out, "w", G, start, start + interval, resolution, 0);
	sprintf(out, "%s_sum.dat", outfile);
	output_gauss_transform(out, "w", T->data, shift, T->lengths[0], stdev + 6);
    }

    free_gauss_vector(G);

    return T;
}


/*
 * Estimates the given set of events using gaussian kernels, returning the kernels
 * in their raw form.
 */
gauss_vector* estimate_gaussian_raw(paramlist* params, char* infile, char* outfile)
{
    double stdev = get_double_param(params, "gauss_est_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double resolution = get_double_param(params, "gauss_est_resolution");

    gauss_vector* G = _estimate_gaussian_raw(infile, outfile, start, interval, stdev, resolution);
 
    if (outfile != NULL){
	char* out = malloc(strlen(outfile) + strlen("_contrib.dat") + 3);
	sprintf(out, "%s_contrib.dat", outfile);
	output_gaussian_contributions(out, "w", G, start, start + interval, resolution, 0);
	free(out);
    }

    return G;
}

gauss_vector* _estimate_gaussian_raw(char* infile, char* outfile, double start,
				     double interval_length, double stdev, double resolution)
{
    double* ev = get_event_data_all(infile);

    gauss_vector* G = gen_gaussian_vector_from_array(ev + 1, ev[0] - 1, stdev, 1, 0);
    

    free(ev);
    
    return G;
}
