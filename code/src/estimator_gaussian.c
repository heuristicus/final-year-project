#include "estimator.h"

double_multi_arr* estimate_gaussian(paramlist* params, char* infile, char* outfile)
{
    double stdev = get_double_param(params, "gauss_est_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");
    double resolution = get_double_param(params, "gauss_est_resolution");

    if (outfile == NULL){
	outfile = get_string_param(params, "gauss_est_out");
    }

   return _estimate_gaussian(infile, outfile, start, interval, stdev, resolution);
}

double_multi_arr* _estimate_gaussian(char* infile, char* outfile, double start,
			double interval_length, double stdev, double resolution)
{
    double* ev = get_event_data_all(infile);

    gauss_vector* G = gen_gaussian_vector_from_array(ev + 1, ev[0] - 1, stdev, 1, 0);
    
    double_multi_arr* T = gauss_transform(G, start, start + interval_length, resolution);


    /* double min = find_min_value(T->data[1], T->lengths[1]); */
    /* double shift = 0; */
    /* if (min <= 0){ */
    /* 	shift = -min; */
    /* } */
    
    /* output_gauss_transform(outfile, "w", T->data, shift, T->lengths[0], stdev + 6); */
    /* output_gaussian_contributions("contrib", "w", G, start, start + interval_length, resolution, 0); */
    /* mult_double_to_file("kdetest", "w", T[0], T[1], interval_length/resolution); */

    free_gauss_vector(G);
    free(ev);

    return T;
}

gauss_vector* n_estimate_gaussian(paramlist* params, char* infile, char* outfile)
{
    double stdev = get_double_param(params, "gauss_est_stdev");
    double start = get_double_param(params, "start_time");
    double interval = get_double_param(params, "interval_time");

    if (outfile == NULL){
	outfile = get_string_param(params, "gauss_est_out");
    }
    
    
   return _n_estimate_gaussian(infile, outfile, start, interval, stdev);
}

gauss_vector* _n_estimate_gaussian(char* infile, char* outfile, double start,
			double interval_length, double stdev)
{
    double* ev = get_event_data_all(infile);

    gauss_vector* G = gen_gaussian_vector_from_array(ev + 1, ev[0] - 1, stdev, 1, 0);

    double_multi_arr* T = gauss_transform(G, start, start + interval_length, 0.5);
    output_gauss_transform(outfile, "w", T->data, 0, T->lengths[0], 1);

    free_double_multi_arr(T);
    free(ev);
    
    return G;
}
