#include <check.h>
#include "tests.h"
#include "paramlist.h"

START_TEST(test_has_required_params)
{
    paramlist* p = init_list("test1", "val1");
    p = add(p, "test2", "val2");
    p = add(p, "test3", "val3");
    p = add(p, "test4", "val4");
    
    char *req1[] = {"test1", "test2"};
    char *req2[] = {"test3", "test5"};
    
    fail_unless(has_required_params(p, req1, 2) == 1, NULL);
    fail_unless(has_required_params(p, req2, 2) == 0, NULL);
    fail_unless(has_required_params(NULL, req1, 2) == 0, NULL);
}
END_TEST

Suite* estimator_suite(void)
{
    Suite* s = suite_create("estimator");
    TCase* tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_has_required_params);
    
    suite_add_tcase(s, tc_core);

    return s;
}

