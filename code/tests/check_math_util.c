#include <stdlib.h>
#include <check.h>
#include "tests.h"
#include "math_util.h"

START_TEST (test_fact)
{
    fail_if(fact(0) != 1, NULL);
    // Negative values must return 0
    fail_unless(fact(-1) == 0, NULL);

    // Some values to check
    long double f12 = 479001600;
    long double f20 = 2432902008176640000;
    
    fail_unless(fact(12) == f12, NULL);
    fail_unless(fact(20) == f20, NULL);
}
END_TEST

START_TEST (test_prob_num_events_in_time_span)
{
    double lambda = 5.0;
    int k = 3;
    fail_unless(prob_num_events_in_time_span(-1, -1, lambda, k) == -1);
    fail_unless(prob_num_events_in_time_span(-1, 1, lambda, k) == -1);
    fail_unless(prob_num_events_in_time_span(1, 1, lambda, k) == -1);
    fail_unless(prob_num_events_in_time_span(1, -1, lambda, k) == -1);
    fail_unless(prob_num_events_in_time_span(-1, 1, lambda, -k) == -1);
    fail_unless(prob_num_events_in_time_span(-1, 1, -lambda, k) == -1);
    fail_unless(prob_num_events_in_time_span(1, 2, 0, 0) == -1);
    fail_unless(prob_num_events_in_time_span(1, 2, 2, 5) > 0);
}
END_TEST

START_TEST (test_sum_events_in_interval) 
{
    double a1[5] = {0.1, 0.2, 0.4, 0.5, 0.7};
    double a2[1] = {0.4};
    
    // All invalid
    fail_unless(sum_events_in_interval(a1, 0, 0, 0, 0) == NULL, NULL);
    // All negative
    fail_unless(sum_events_in_interval(a1, -1, -1, -1, -1) == NULL, NULL);
    // Invalid subintervals
    fail_unless(sum_events_in_interval(a1, 5, 0, 1, 0) == NULL, NULL);
    // Valid, but fewer events than actually in the array
    int *res1 = sum_events_in_interval(a1, 5, 0, 1, 1);
    fail_unless(res1[0] == 5, NULL);
    free(res1);
    // Can't really do anything in this case - elements after
    // the end of the array could be valid
    /* // Valid, but more events that actually in the array */
    /* int *res2 = sum_events_in_interval(a1, 8, 0, 1, 1); */
    /* fail_unless(res2[0] == 5, NULL); */
    /* free(res2); */
    // Split into two subintervals
    int *res3 = sum_events_in_interval(a1, 5, 0, 1, 2);
    fail_unless(res3[0] == 4 && res3[1] == 1, NULL);
    free(res3);
    // Only a single event
    int *res4 = sum_events_in_interval(a2, 1, 0, 1, 1);
    fail_unless(res4[0] == 1, NULL);
    free(res4);
    // Single event with more than one subinterval
    int *res5 = sum_events_in_interval(a2, 1, 0, 1, 2);
    fail_unless(res5[0] == 1 && res5[1] == 0, NULL);
    free(res5);
}
END_TEST

START_TEST (test_get_interval_midpoints)
{
    fail_unless(get_interval_midpoints(-1, -1, -1) == NULL, NULL);
    fail_unless(get_interval_midpoints(-1, 1, -1) == NULL, NULL);
    fail_unless(get_interval_midpoints(1, 1, -1) == NULL, NULL);
    fail_unless(get_interval_midpoints(0, 1, 0) == NULL, NULL);

    double* res1 = get_interval_midpoints(0, 1, 1);
    fail_unless(res1[0] == 0.5);
    free(res1);
    double* res2 = get_interval_midpoints(0, 1, 2);
    fail_unless(res2[0] == 0.25 && res2[1] == 0.75, NULL);
    free(res2);
    double* res3 = get_interval_midpoints(0, 100, 1);
    fail_unless(res3[0] == 50, NULL);
    free(res3);
    double* res4 = get_interval_midpoints(0, 100, 2);
    fail_unless(res4[0] == 25 && res4[1] == 75, NULL);
    free(res4);
}
END_TEST

START_TEST (test_get_interval_midpoint) 
{
    fail_unless(get_interval_midpoint(0, 0, 0, 0) == -1, NULL);
    fail_unless(get_interval_midpoint(-1, 0, 1, 1) == -1, NULL);
    fail_unless(get_interval_midpoint(1, 0, 1, 0) == -1, NULL);
    // The interval number should not exceed the number of subintervals
    fail_unless(get_interval_midpoint(10, 0, 5, 5) == -1, NULL);
    fail_unless(get_interval_midpoint(1, 0, 5, 1) == 2.5, NULL);
    fail_unless(get_interval_midpoint(1, 0, 5, 5) == 0.5, NULL);
    fail_unless(get_interval_midpoint(5, 0, 5, 5) == 4.5, NULL);
}
END_TEST

START_TEST (test_avg)
{
    double a1[5] = {1, 2, 3, 4, 5};
    double a2[5] = {-1, -2, -3, -4, -5};
    
    fail_unless(avg(a1, 5) == 3, NULL);
    fail_unless(avg(a2, 5) == -3, NULL);
    // Should never really have a negative length
    fail_unless(avg(a2, -1) == 0, NULL);
}
END_TEST

START_TEST (test_sum_double_arr)
{
    double a1[5] = {0.5, 0.1, 0.2, 0.1, 0.2};
    double a2[5] = {-0.5, -0.1, -0.2, -0.1, -0.2};
    double a3[5] = {5, 10, 15, 20, 25};
    double a4[5] = {-5, -10, -15, -20, -25};
  
    double t = sum_double_arr(a1, 5);
    double t1 = sum_double_arr(a2, 5);

    fail_unless(t == 1.1, NULL);
    fail_unless(t1 == -1.1, NULL);
    fail_unless(sum_double_arr(a3, 5) == 75, NULL);
    fail_unless(sum_double_arr(a4, 5) == -75, NULL);
}
END_TEST

START_TEST (test_sum_int_arr)
{
    int a1[5] = {5, 1, 2, 1, 1};
    int a2[5] = {-5, -1, -2, -1, -1};
    
    fail_unless(sum_int_arr(a1, 5) == 10, NULL);
    fail_unless(sum_int_arr(a2, 5) == -10, NULL);
    
}
END_TEST

START_TEST (test_get_gradient)
{
    // Positive gradient
    fail_unless(get_gradient(0, 0, 10, 10) == 1, NULL);
    // Negative gradient
    fail_unless(get_gradient(0, 10, 10, 0) == -1, NULL);
    // What if both points are the same?
    fail_unless(isnan(get_gradient(5, 5, 5, 5)), NULL);
    // How about on opposite sides?
    fail_unless(get_gradient(-5, -5, 5, 5) == 1, NULL);
    fail_unless(get_gradient(5, 5, -5, -5) == 1, NULL);
}
END_TEST

START_TEST (test_get_intercept)
{
    // Zero intercept
    fail_unless(get_intercept(5, 5, 1) == 0, NULL);
    // Zero gradient
    fail_unless(get_intercept(5, 5, 0) == 5, NULL);
    // Nonzero intercept
    fail_unless(get_intercept(5, 5, 0.5) == 2.5, NULL);
    // Negative gradient, point and intercept
    fail_unless(get_intercept(-5, -5, -1) == -10, NULL);
}
END_TEST

START_TEST (test_get_intercept_and_gradient)
{
    // Basic
    double* res1 = get_intercept_and_gradient(0, 0, 5, 5);
    fail_unless(res1[0] == 0 && res1[1] == 1, NULL);
    free(res1);
    // Same two points
    double* res2 = get_intercept_and_gradient(0, 0, 0, 0);
    fail_unless(res2 == NULL);
    free(res2);
    // negative gradient result
    double* res3 = get_intercept_and_gradient(1, 4, 5, 0);
    fail_unless(res3[0] == 5 && res3[1] == -1, NULL);
    free(res3);
}
END_TEST

START_TEST (test_evaluate_function)
{
    fail_unless(evaluate_function(0, 0, 0) == 0, NULL);
    fail_unless(evaluate_function(2, 3, 1) == 5, NULL);
}
END_TEST

START_TEST (test_get_midpoint)
{
    // same value
    fail_unless(get_midpoint(5, 5) == 5, NULL);
    fail_unless(get_midpoint(0, 5) == 2.5, NULL);
    fail_unless(get_midpoint(-5, 5) == 0, NULL);
    fail_unless(get_midpoint(0, -5) == -2.5, NULL);
    fail_unless(get_midpoint(-10, -5) == -7.5, NULL);
}
END_TEST

START_TEST(test_make_gaussian)
{
    gaussian* g = make_gaussian(2, 3);
    gaussian* h = make_gaussian(2, 0);
    gaussian* i = make_gaussian(2, -1);
    
    fail_unless(g->mean == 2, NULL);
    fail_unless(g->stdev == 3, NULL);
    
    fail_unless(h == NULL, "Zero stdev non-null");
    fail_unless(i == NULL, "Negative stdev non-null");
}
END_TEST

START_TEST(test_random_vector)
{
    fail_unless(random_vector(0) == NULL, "Zero length non-null");
    fail_unless(random_vector(-1) == NULL, "Negative length non-null");
}
END_TEST

START_TEST(test_gaussian_contribution_at_point)
{
    gaussian* g = make_gaussian(2, 3);
    
    fail_unless(gaussian_contribution_at_point(2, g, 1) == 1, NULL);
    
    const double res_1 = 0.89483931681;
    
    fail_unless(gaussian_contribution_at_point(1, g, 1) - res_1 < 0.0000001, NULL);
    
}
END_TEST

START_TEST(test_sum_gaussians_at_point)
{
    gaussian* g1 = make_gaussian(2, 3);
    gaussian* g2 = make_gaussian(1, 3);
    gauss_vector* G = malloc(sizeof(gauss_vector));
    gaussian** gs = malloc(2 * sizeof(gaussian*));
    gs[0] = g1;
    gs[1] = g2;
        
    G->gaussians = gs;
    G->len = 2;
    G->w = malloc(2 * sizeof(double));
    G->w[0] = 1;
    G->w[1] = 1;

    double res = 1.89483931681;

    fail_unless(sum_gaussians_at_point(1, G) - res < 0.0000001, "Return does not match expected value.");
    fail_unless(sum_gaussians_at_point(3, G) - res < 0.0000001, "Return does not match expected value.");
    fail_unless(sum_gaussians_at_point(3, NULL) == 0, "Null pointer returns nonzero");
}
END_TEST

START_TEST(test_gaussian_contribution)
{
    gaussian* g = make_gaussian(3, 3);
    
    double** res = gaussian_contribution(g, 0, 6, 1, 1);

    double** a = gaussian_contribution(g, 0, 0, 1, 1);
    double** b = gaussian_contribution(g, 0, -1, 1, 1);
    double** c = gaussian_contribution(g, 0, 6, -1, 1);
    double** d = gaussian_contribution(g, 0, 6, 0, 1);
    double** e = gaussian_contribution(NULL, 0, 6, 1, 1);

    fail_unless(a == NULL, "Invalid interval not null");
    fail_unless(b == NULL, "Negative interval not null");
    fail_unless(c == NULL, "Negative step not null");
    fail_unless(d == NULL, "Zero step not null");
    fail_unless(e == NULL, "Null pointer return non-null");

    const double correct[] = {0.367879441, 0.641180388, 0.984839316, 1.0, 0.984839316, 0.641180388, 0.367879441};

    int i;
    
    for (i = 0; i < 7; ++i) {
	fail_unless(res[0][i] == i, "Expected position of contribution check does not match.");
	fail_unless(res[1][i] - correct[i] < 0.0000001, "Contribution at point does not match expected contribution.");
    }
}
END_TEST

START_TEST(test_gauss_transform)
{
    gaussian* g1 = make_gaussian(2, 3);
    gaussian* g2 = make_gaussian(3, 3);
    gauss_vector* G = malloc(sizeof(gauss_vector));
    gaussian** gs = malloc(2 * sizeof(gaussian*));
    gs[0] = g1;
    gs[1] = g2;
        
    G->gaussians = gs;
    G->len = 2;
    G->w = malloc(2 * sizeof(double));
    G->w[0] = 1;
    G->w[1] = 1;

    const double correct[] = {1.009059829, 1.536019705, 1.894839316, 1.894839316, 1.536019705, 1.009059829};

    double** res = gauss_transform(G, 0, 6, 1);
    
    int i;
    
    for (i = 0; i < 6; ++i) {
	fail_unless(res[0][i] == i, "Data gathering point does not match expected.");
	fail_unless(res[1][i] - correct[i] < 0.0000001, "Transform at point does not match expected value.");
    }

    double** zerostep = gauss_transform(G, 0, 6, 0);
    double** negstep = gauss_transform(G, 0, 6, -1);
    double** zeroint = gauss_transform(G, 0, 0, -1);
    double** negint = gauss_transform(G, -4, 0, -1);
    
    fail_unless(zerostep == NULL, "Zero step size non-null return");
    fail_unless(negstep == NULL, "Negative step size non-null return");
    fail_unless(zeroint == NULL, "Zero interval non-null return");
    fail_unless(negint == NULL, "Negative interval non-null return");
}
END_TEST

START_TEST(test_gen_gaussian_vector_uniform)
{
    // invalid intervals and step
    gauss_vector* a = gen_gaussian_vector_uniform(3, 10, 0, 2);
    gauss_vector* b = gen_gaussian_vector_uniform(3, -10, 0, 2);
    gauss_vector* c = gen_gaussian_vector_uniform(3, 0, 10, -2);
    gauss_vector* d = gen_gaussian_vector_uniform(3, 0, 10, 0);

    fail_unless(a == NULL, "Invalid interval non-null return");
    fail_unless(b == NULL, "Negative interval non-null return");
    fail_unless(c == NULL, "Negative step non-null return");
    fail_unless(d == NULL, "Zero step non-null return");

    // Interval which results in a range that is evenly divisible by the number of gaussians
    gauss_vector* res = gen_gaussian_vector_uniform(3, 0, 10, 2);
    fail_unless(res->len == 6, "Length of vector not expected value");
     
    double expected1[] = {0, 2, 4, 6, 8, 10};

    int i;
    
    for (i = 0; i < res->len; ++i){
	fail_unless(res->gaussians[i]->mean == expected1[i], "Given mean differs from expected");
	fail_unless(res->gaussians[i]->stdev == 3, "Stdev differs from expected");
    }

    // Interval which results in a range not evenly divisible by the number of gaussians
    gauss_vector* res2 = gen_gaussian_vector_uniform(3, 0, 10, 3);
    fail_unless(res2->len == 4, "Vector length differs from expected");
    
    double expected2[] = {0, 3, 6, 9};
    
    for (i = 0; i < res2->len; ++i){
	printf("mean %lf\n", res2->gaussians[i]->mean);
	fail_unless(res2->gaussians[i]->mean == expected2[i], "Given mean differs from expected");
	fail_unless(res2->gaussians[i]->stdev == 3, "Stdev differs from expected.");
    }


}
END_TEST

START_TEST(test_gen_gaussian_vector_from_array)
{
    double points[] = {1.2, 2.4, 2.5, 2.9, 4.0};
    gauss_vector* a = gen_gaussian_vector_from_array(NULL, 5, 3);
    gauss_vector* b = gen_gaussian_vector_from_array(points, 0, 3);
    gauss_vector* c = gen_gaussian_vector_from_array(points, -1, 2);
    gauss_vector* d = gen_gaussian_vector_from_array(points, 5, -1);
    gauss_vector* e = gen_gaussian_vector_from_array(points, 5, 0);
    
    fail_unless(a == NULL, "Null points return non-null");
    fail_unless(b == NULL, "Zero length non-null return");
    fail_unless(c == NULL, "Negative length non-null return");
    fail_unless(d == NULL, "Negative stdev non-null return");
    fail_unless(e == NULL, "Zero stdev non-null return");

    gauss_vector* G = gen_gaussian_vector_from_array(points, sizeof(points)/sizeof(double), 3);

    fail_unless(G->len == sizeof(points)/sizeof(double));
    
    int i;
    
    for (i = 0; i < G->len; ++i) {
	printf("mean %lf\n", G->gaussians[i]->mean);
	fail_unless(G->gaussians[i]->mean == points[i], "Given mean differs from expected");
	fail_unless(G->gaussians[i]->stdev == 3, "Stdev differs from expected");
    }
}
END_TEST

START_TEST(test_find_min_value)
{
    double data[] = {1,2,3,4,5,6};
    double data1[] = {-153, -123, -222, -414, 34551};
    double data2[] = {13,313,444,555};

    printf("%lf min\n", find_min_value(data, 6));
    double diff = abs(find_min_value(data, 6) - 1.0);
    printf("diff is %.20lf\n", diff);
    double diff1 = abs(find_min_value(data1, 5) - -414.0);
    double diff2 = abs(find_min_value(data2, 4) - 13.0);
        
    fail_unless(diff >= 0 && diff < 0.000001, NULL);
    fail_unless(diff1 >= 0 && diff1 < 0.000001, NULL);
    fail_unless(diff2 >= 0 && diff2 < 0.000001, NULL);

    fail_unless(find_min_value(NULL, 5) == 0, NULL);
    fail_unless(find_min_value(data1, -1) == 0, NULL);
    fail_unless(find_min_value(data1, 0) == 0, NULL);
}
END_TEST

START_TEST(test_add_to_arr);
{
    double data[] = {10, 10, 10};
    double data1[] = {0, 0, 0};
    double data2[] = {-5, -2, -1};

    double* ret = add_to_arr(data, 3, 5);
    double* ret1 = add_to_arr(data1, 3, 10);
    double* ret2 = add_to_arr(data2, 3, 5);
    
    double correct[] = {15, 15, 15};
    double correct1[] = {10, 10, 10};
    double correct2[] = {0, 3, 4};
    
    int i;
    
    for (i = 0; i < 3; ++i) {
	double diff = abs(correct[i] - ret[i]);
	double diff1 = abs(correct1[i] - ret1[i]);
	double diff2 = abs(correct2[i] - ret2[i]);
	
	fail_unless(diff >= 0 && diff < 0.000001, NULL);
	fail_unless(diff1 >= 0 && diff1 < 0.000001, NULL);
	fail_unless(diff2 >= 0 && diff2 < 0.000001, NULL);
    }

}
END_TEST

START_TEST(test_weight_vector)
{
    double* data = weight_vector(2.5, 5);
    double* data1 = weight_vector(-1.0, 5);
    
    int i;
    
    for (i = 0; i < 5; ++i) {
	fail_unless(data[i] == 2.5);
	fail_unless(data1[i] == -1.0);
    }


    fail_unless(weight_vector(1, -1) == NULL, NULL);
    fail_unless(weight_vector(1, 0) == NULL, NULL);
}
END_TEST

Suite* math_util_suite(void)
{
    Suite *s = suite_create("math_util");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_fact);
    tcase_add_test(tc_core, test_prob_num_events_in_time_span);
    tcase_add_test(tc_core, test_sum_events_in_interval);
    tcase_add_test(tc_core, test_get_interval_midpoints);
    tcase_add_test(tc_core, test_get_interval_midpoint);
    tcase_add_test(tc_core, test_avg);
    tcase_add_test(tc_core, test_sum_double_arr);
    tcase_add_test(tc_core, test_sum_int_arr);
    tcase_add_test(tc_core, test_get_gradient);
    tcase_add_test(tc_core, test_get_intercept);
    tcase_add_test(tc_core, test_get_intercept_and_gradient);
    tcase_add_test(tc_core, test_evaluate_function);
    tcase_add_test(tc_core, test_get_midpoint);
    tcase_add_test(tc_core, test_make_gaussian);
    tcase_add_test(tc_core, test_random_vector);
    tcase_add_test(tc_core, test_gaussian_contribution_at_point);
    tcase_add_test(tc_core, test_gaussian_contribution);
    tcase_add_test(tc_core, test_sum_gaussians_at_point);
    tcase_add_test(tc_core, test_gauss_transform);
    tcase_add_test(tc_core, test_gen_gaussian_vector_uniform);
    tcase_add_test(tc_core, test_gen_gaussian_vector_from_array);
    tcase_add_test(tc_core, test_find_min_value);
    tcase_add_test(tc_core, test_add_to_arr);
    tcase_add_test(tc_core, test_weight_vector);
    
    suite_add_tcase(s, tc_core);

    return s;
}
