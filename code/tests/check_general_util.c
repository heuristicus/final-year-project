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

START_TEST(test_estimate_at_point)
{
    est_arr* a = malloc(sizeof(est_arr));

    a->len = 3;
    a->estimates = malloc(sizeof(est_data*) * a->len);
    
    est_data* e1 = malloc(sizeof(est_data));
    est_data* e2 = malloc(sizeof(est_data));
    est_data* e3 = malloc(sizeof(est_data));

    e1->start = 0;
    e1->end = 10;
    e1->est_a = 5;
    e1->est_b = 0;

    e2->start = 10;
    e2->end = 20;
    e2->est_a = 10;
    e2->est_b = 0;

    e3->start = 20;
    e3->end = 30;
    e3->est_a = 15;
    e3->est_b = 0;

    a->estimates[0] = e1;
    a->estimates[1] = e2;
    a->estimates[2] = e3;

    double ret1 = estimate_at_point(a, 5);
    double ret2 = estimate_at_point(a, 15);
    double ret3 = estimate_at_point(a, 25);
    double ret4 = estimate_at_point(a, 35);
    
    fail_unless(ret1 == 5, NULL);
    fail_unless(ret2 == 10, NULL);
    fail_unless(ret3 == 15, NULL);
    fail_unless(ret4 == 0, NULL);

    free_est_arr(a);
}
END_TEST

START_TEST(test_data_at_point)
{
    est_arr* a = malloc(sizeof(est_arr));

    a->len = 3;
    a->estimates = malloc(sizeof(est_data*) * a->len);
    
    est_data* e1 = malloc(sizeof(est_data));
    est_data* e2 = malloc(sizeof(est_data));
    est_data* e3 = malloc(sizeof(est_data));

    e1->start = 0;
    e1->end = 10;
    e1->est_a = 5;
    e1->est_b = 0;

    e2->start = 10;
    e2->end = 20;
    e2->est_a = 10;
    e2->est_b = 0;

    e3->start = 20;
    e3->end = 30;
    e3->est_a = 15;
    e3->est_b = 0;

    a->estimates[0] = e1;
    a->estimates[1] = e2;
    a->estimates[2] = e3;
    
    est_data* ret1 = data_at_point(a, 5);
    est_data* ret2 = data_at_point(a, 15);
    est_data* ret3 = data_at_point(a, 25);
    
    fail_unless(ret1 == e1, NULL);
    fail_unless(ret2 == e2, NULL);
    fail_unless(ret3 == e3, NULL);
    fail_unless(data_at_point(a, 35) == NULL, NULL);
    
    fail_unless(data_at_point(NULL, 5) == NULL, NULL);
    
    free_est_arr(a);
}
END_TEST

START_TEST(test_has_required_params)
{
    char* required[] = {"this", "is", "required", "asd"};
    
    paramlist* params = init_list("this", "test1");
    params = add(params, "is", "test2");
    params = add(params, "required", "test3");
    params = add(params, "asd", "test4");

    paramlist* badparams = init_list("no", "params");
    
    fail_unless(has_required_params(params, required, sizeof(required)/sizeof(char*)) == 1, NULL);
    fail_unless(has_required_params(badparams, required, sizeof(required)/sizeof(char*)) == 0, NULL);
}
END_TEST

Suite* general_util_suite(void)
{
    Suite* s = suite_create("general_util");
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_string_split);
    tcase_add_test(tc_core, test_interval_valid);
    tcase_add_test(tc_core, test_get_event_subinterval);
    tcase_add_test(tc_core, test_estimate_at_point);
    tcase_add_test(tc_core, test_data_at_point);
    tcase_add_test(tc_core, test_has_required_params);

    suite_add_tcase(s, tc_core);

    return s;
}
