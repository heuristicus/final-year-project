#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <check.h>
#include "tests.h"
#include "general_util.h"

START_TEST (test_string_split)
{
    string_arr* res1 = string_split("hello,test,testing,ing", ',');
    fail_unless(res1->len == 4);
    fail_unless(strcmp(res1->data[0], "hello") == 0, NULL); 
    fail_unless(strcmp(res1->data[1], "test") == 0, NULL);
    fail_unless(strcmp(res1->data[2], "testing") == 0, NULL);
    fail_unless(strcmp(res1->data[3], "ing") == 0, NULL);
    
    // Empty string
    string_arr* res2 = string_split("", ',');
    fail_unless(res2->len == 1);
    fail_unless(strcmp(res2->data[0], "") == 0, NULL);

    // Whitespace string
    string_arr* res3 = string_split("     ", ',');
    fail_unless(res3->len == 1, NULL);
    fail_unless(strcmp(res3->data[0], "     ") == 0, NULL);
    
    // String of only separators
    string_arr* res4 = string_split(",,,", ',');
    fail_unless(res4->len == 4, NULL);
    fail_unless(strcmp(res4->data[0], "") == 0, NULL);
    fail_unless(strcmp(res4->data[1], "") == 0, NULL);
    fail_unless(strcmp(res4->data[2], "") == 0, NULL);
    fail_unless(strcmp(res4->data[3], "") == 0, NULL);

    // alphabetic separator
    string_arr* res5 = string_split("thisziszaztest", 'z');
    fail_unless(res5->len == 4, NULL);
    fail_unless(strcmp(res5->data[0], "this") == 0, NULL);
    fail_unless(strcmp(res5->data[1], "is") == 0, NULL);
    fail_unless(strcmp(res5->data[2], "a") == 0, NULL);
    fail_unless(strcmp(res5->data[3], "test") == 0, NULL);

    fail_unless(string_split(NULL, ',') == NULL, NULL);
}
END_TEST

START_TEST (test_interval_valid)
{
    fail_unless(interval_valid(0, 0) == 0, NULL);
    fail_unless(interval_valid(-1, -1) == 0, NULL);
    fail_unless(interval_valid(-5, -10) == 0, NULL);
    fail_unless(interval_valid(10, 9) == 0, NULL);

    fail_unless(interval_valid(0, 1) == 1, NULL);
    // very short interval
    fail_unless(interval_valid(0, 0.000000001) == 1, NULL);
    fail_unless(interval_valid(10, 20) == 1, NULL);
}
END_TEST

START_TEST (test_get_event_subinterval)
{
    double a1[8] = {8, 0.5, 1, 1.5, 2, 2.5, 3, 3.5};
    
    // Get back what we put in
    double* res1 = get_event_subinterval(a1, 0, 4);
    int i;
    
    for (i = 0; i < 8; ++i) {
	//printf("%d: %lf %lf\n", i, res1[i], a1[i]);
	fail_unless(res1[i] == a1[i]);
    }
    free(res1);
    
    // Get a subinterval
    double* res2 = get_event_subinterval(a1, 0, 2);
    fail_unless(res2[0] == 4, NULL);
    
    for (i = 1; i < 5; ++i) {
	//printf("%d: %lf %lf\n", i, res1[i], a1[i]);
	fail_unless(res2[i] == a1[i], NULL);
    }
    free(res2);

    // non-present subinterval
    fail_unless(get_event_subinterval(a1, 4, 5) == NULL, NULL);
    
    // Invalid subinterval
    fail_unless(get_event_subinterval(a1, -2, -3) == NULL, NULL);
    double *a2 = NULL;
    
    // Null array
    fail_unless(get_event_subinterval(a2, 2, 3) == NULL, NULL);
    
    // Get events that are the same as the start interval times
    double* res3 = get_event_subinterval(a1, 3, 4);
    
    fail_unless(res3[0] == 3, NULL);
    fail_unless(res3[1] == 3.0, NULL);
    fail_unless(res3[2] == 3.5, NULL);

    // very short, but valid interval
    double* res4 = get_event_subinterval(a1, 0.999999, 1.00000001);
    fail_unless(res4[0] == 2, NULL);
    fail_unless(res4[1] == 1, NULL);
}
END_TEST

Suite* general_util_suite(void)
{
    Suite* s = suite_create("general_util");
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_string_split);
    tcase_add_test(tc_core, test_interval_valid);
    tcase_add_test(tc_core, test_get_event_subinterval);
    
    suite_add_tcase(s, tc_core);

    return s;
}
