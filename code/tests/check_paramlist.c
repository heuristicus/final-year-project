#include <check.h>
#include "paramlist.h"
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

START_TEST(test_add_exit)
{
    // Try adding a parameter with the same name but different value. Should exit.
    paramlist* c = init_list("node1", "1");
    add(c, "node1", "2");
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

START_TEST(test_get_double_list_param)
{
    paramlist* a = init_list("dlist", "1.2,2.4,4.5,5.6");
    double_arr* res = get_double_list_param(a, "dlist");
    
    int i;

    double correct[] = {1.2,2.4,4.5,5.6};
    
    fail_unless(res->len == 4);
    
    for (i = 0; i < 4; ++i) {
	fail_unless(epsck(res->data[i], correct[i]), NULL);
    }

    fail_unless(get_double_list_param(a, "noexist") == NULL, NULL);
    fail_unless(get_double_list_param(NULL, "none") == NULL, NULL);
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
    tcase_add_exit_test(tc_core, test_add_exit, 1);
    tcase_add_test(tc_core, test_get_double_list_param);
    
    suite_add_tcase(s, tc_core);
    return s;
}
