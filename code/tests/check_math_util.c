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
    suite_add_tcase(s, tc_core);

    return s;
}
