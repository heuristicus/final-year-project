#include <stdlib.h>
#include <check.h>
#include <math.h>
#include "../math_util.h"
#include "../general_util.h"

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
    fail_unless(res3[0] == 4 && res3[1] == 1);
    free(res3);
    // Only a single event
    int *res4 = sum_events_in_interval(a2, 1, 0, 1, 1);
    fail_unless(res4[0] == 1);
    free(res4);
    // Single event with more than one subinterval
    int *res5 = sum_events_in_interval(a2, 1, 0, 1, 2);
    fail_unless(res5[0] == 1 && res5[1] == 0);
    free(res5);
}
END_TEST

Suite* math_suite(void)
{
    Suite *s = suite_create("math");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_fact);
    tcase_add_test(tc_core, test_prob_num_events_in_time_span);
    tcase_add_test(tc_core, test_sum_events_in_interval);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char *argv[])
{
    int number_failed;
    Suite *s = math_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    
    return 0;
}
