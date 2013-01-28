#include "tests.h"

int main(int argc, char *argv[])
{
    int number_failed;

    SRunner *sr = srunner_create(math_util_suite());
    srunner_set_fork_status(sr, CK_FORK); // Allow exit status checking
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
