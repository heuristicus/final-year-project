#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <check.h>
#include "tests.h"
#include "../general_util.h"

START_TEST (test_string_split)
{
    char** res1 = string_split("hello,test,testing,ing", ',');
    fail_unless(atoi(res1[0]) == 5, NULL);
    fail_unless(strcmp(res1[1], "hello") == 0, NULL); 
    fail_unless(strcmp(res1[2], "test") == 0, NULL);
    fail_unless(strcmp(res1[3], "testing") == 0, NULL);
    fail_unless(strcmp(res1[4], "ing") == 0, NULL);
    
    // Empty string
    char** res2 = string_split("", ',');
    fail_unless(atoi(res2[0]) == 2, NULL);
    fail_unless(strcmp(res2[1], "") == 0, NULL);

    // Whitespace string
    char** res3 = string_split("     ", ',');
    fail_unless(atoi(res3[0]) == 2, NULL);
    fail_unless(strcmp(res3[1], "     ") == 0, NULL);
    
    // String of only separators
    char** res4 = string_split(",,,", ',');
    fail_unless(atoi(res4[0]) == 5, NULL);
    fail_unless(strcmp(res4[1], "") == 0, NULL);
    fail_unless(strcmp(res4[2], "") == 0, NULL);
    fail_unless(strcmp(res4[3], "") == 0, NULL);
    fail_unless(strcmp(res4[4], "") == 0, NULL);

    // alphabetic separator
    char** res5 = string_split("thisziszaztest", 'z');
    fail_unless(atoi(res5[0]) == 5, NULL);
    fail_unless(strcmp(res5[1], "this") == 0, NULL);
    fail_unless(strcmp(res5[2], "is") == 0, NULL);
    fail_unless(strcmp(res5[3], "a") == 0, NULL);
    fail_unless(strcmp(res5[4], "test") == 0, NULL);
}
END_TEST

START_TEST (test_interval_check)
{
    fail_unless(interval_check(0, 0) == 0, NULL);
    fail_unless(interval_check(-1, -1) == 0, NULL);
    fail_unless(interval_check(-5, -10) == 0, NULL);
    fail_unless(interval_check(10, 9) == 0, NULL);

    fail_unless(interval_check(0, 1) == 1, NULL);
    // very short interval
    fail_unless(interval_check(0, 0.000000001) == 1, NULL);
    fail_unless(interval_check(10, 20) == 1, NULL);
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
    tcase_add_test(tc_core, test_interval_check);
    tcase_add_test(tc_core, test_get_event_subinterval);
    
    suite_add_tcase(s, tc_core);

    return s;
}
