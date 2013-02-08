#include "tests.h"

/*
 * Checks whether a double is within some epsilon of the expected result
 */
int epsck(double result, double expected)
{
    double diff = fabs(expected - result);

    return diff < DOUBLE_EPSILON && diff >= 0;
}

START_TEST(test_epsck)
{
    fail_unless(epsck(2, 1) == 0, NULL);
    fail_unless(epsck(2.1234567, 2.1234567) == 1, NULL);
    fail_unless(epsck(0, 0) == 1, NULL);
    fail_unless(epsck(2.1234567 + 2 * DOUBLE_EPSILON, 2.1234567) == 0, NULL);
}
END_TEST

int main(int argc, char *argv[])
{
    int number_failed;

    Suite *s = suite_create("test_functions");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_epsck);
    suite_add_tcase(s, tc_core);

    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_FORK); // Allow exit status checking
    srunner_add_suite(sr, math_util_suite());
    srunner_add_suite(sr, general_util_suite());
    srunner_add_suite(sr, paramlist_suite());
    srunner_add_suite(sr, file_util_suite());
    srunner_add_suite(sr, estimator_suite());
    srunner_add_suite(sr, combinefunction_suite());
    srunner_add_suite(sr, experimenter_suite());
    
    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    
    return 0;
}
