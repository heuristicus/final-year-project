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

    paramlist* c = init_list("node1", "1");
    fail_unless(add(c, "node1", "2") == c, NULL);
    fail_unless(length(c) == 1, NULL);
}
END_TEST

START_TEST (test_get_string_param)
{
    paramlist* a = init_list("node1", "1");
    a = add(a, "node2", "2");
    a = add(a, "node3", "3");
    a = add(a, "node4", "4");
    
    fail_unless(strcmp(get_string_param(a, "node1"), "1") == 0, NULL);
    fail_unless(strcmp(get_string_param(a, "node2"), "2") == 0, NULL);
    fail_unless(strcmp(get_string_param(a, "node3"), "3") == 0, NULL);
    fail_unless(strcmp(get_string_param(a, "node4"), "4") == 0, NULL);
    fail_unless(get_string_param(a, "nonexistent") == NULL, NULL);
}
END_TEST

START_TEST (test_get_param)
{
    paramlist* a = init_list("node1", "1");
    paramlist* b = add(a, "node2", "2");
    paramlist* c = add(b, "node3", "3");
    paramlist* d = add(c, "node4", "4");
    fail_unless(get_param(d, "node1") == a, NULL);
    fail_unless(get_param(d, "node2") == b, NULL);
    fail_unless(get_param(d, "node3") == c, NULL);
    fail_unless(get_param(d, "node4") == d, NULL);
    
}
END_TEST

START_TEST (test_len)
{
    paramlist* a = NULL;
    fail_unless(length(a) == 0, NULL);
    a = init_list("node1", "1");
    fail_unless(length(a) == 1, NULL);
    a = add(a, "node2", "2");
    fail_unless(length(a) == 2, NULL);
    a = add(a, "node3", "3");
    fail_unless(length(a) == 3, NULL);
    a = add(a, "node4", "4");
    fail_unless(length(a) == 4, NULL);
}
END_TEST

Suite* paramlist_suite(void)
{
    Suite* s = suite_create("paramlist");
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_init_list);
    tcase_add_test(tc_core, test_add);
    tcase_add_test(tc_core, test_get_string_param);
    tcase_add_test(tc_core, test_len);
    tcase_add_test(tc_core, test_get_param);
    
    suite_add_tcase(s, tc_core);
    return s;
}
