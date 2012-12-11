#include <check.h>
#include "../paramlist.h"
#include "tests.h"

START_TEST (test_init_list)
{
    paramlist* a = init_list("test", "hello");
    fail_unless(strcmp(a->par, "test") == 0, NULL);
    fail_unless(strcmp(a->val, "hello") == 0, NULL);
    fail_unless(a->next == NULL, NULL);
}
END_TEST

START_TEST (test_add)
{
    paramlist* a = init_list("test", "hello");
    
    paramlist* b = add(a, "node2", "again");
    fail_unless(strcmp(b->par, "node2") == 0, NULL);
    fail_unless(strcmp(b->val, "again") == 0, NULL);
    fail_unless(b->next == a, NULL);
    fail_unless(strcmp(b->next->par, "test") == 0, NULL);
    fail_unless(strcmp(b->next->val, "hello") == 0, NULL);
    fail_unless(b->next->next == NULL, NULL);

    // Check that adding the previously given variable does not modify the list
    fail_unless(b == add(b, "node2", "again"), NULL);
    fail_unless(length(b) == 2, NULL);
}
END_TEST

START_TEST (test_get_param_val)
{
    paramlist* a = init_list("node1", "1");
    a = add(a, "node2", "2");
    a = add(a, "node3", "3");
    a = add(a, "node4", "4");
    
    fail_unless(strcmp(get_param_val(a, "node1"), "1") == 0, NULL);
    fail_unless(strcmp(get_param_val(a, "node2"), "2") == 0, NULL);
    fail_unless(strcmp(get_param_val(a, "node3"), "3") == 0, NULL);
    fail_unless(strcmp(get_param_val(a, "node4"), "4") == 0, NULL);
    fail_unless(get_param_val(a, "nonexistent") == NULL, NULL);
}
END_TEST

START_TEST (test_get_param_val_exit)
{
    paramlist* a = init_list("node1", "1");
    add(a, "node1", "2");
}
END_TEST

Suite* paramlist_suite(void)
{
    Suite* s = suite_create("paramlist");
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_init_list);
    tcase_add_test(tc_core, test_add);
    tcase_add_test(tc_core, test_get_param_val);
    tcase_add_exit_test(tc_core, test_get_param_val_exit, 1);
    
    suite_add_tcase(s, tc_core);
    return s;
}
