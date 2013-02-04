#include "combinefunction.h"
#include "tests.h"

START_TEST(test_combine_functions)
{
    est_arr* a = malloc(sizeof(est_arr));
    est_arr* b = malloc(sizeof(est_arr));
    
    a->len = 2;
    a->estimates = malloc(sizeof(est_data*) * a->len);
    b->len = 2;
    b->estimates = malloc(sizeof(est_data*) * a->len);
    
    est_data* e1 = malloc(sizeof(est_data));
    est_data* e2 = malloc(sizeof(est_data));
    est_data* e3 = malloc(sizeof(est_data));
    est_data* e4 = malloc(sizeof(est_data));

    e1->start = 0;
    e1->end = 3;
    e1->est_a = 3;
    e1->est_b = 0;

    e2->start = 3;
    e2->end = 6;
    e2->est_a = 5;
    e2->est_b = 0;

    e3->start = 0;
    e3->end = 6;
    e3->est_a = 10;
    e3->est_b = 0;

    e4->start = 3;
    e4->end = 6;
    e4->est_a = 15;
    e4->est_b = 0;

    a->estimates[0] = e1;
    a->estimates[1] = e2;
    b->estimates[0] = e3;
    b->estimates[1] = e4;

    est_arr** comb = malloc(sizeof(est_arr*) * 2);
    comb[0] = a;
    comb[1] = b;

    double time_delay[] = {0, 1};
    
    double_multi_arr* res = combine_functions(comb, time_delay, 6, 2, 1);
    int i;

    double correct[] = {6.5, 6.5, 7.5, 10, 10};
    
    for (i = 0; i < sizeof(correct)/sizeof(double); ++i) {
	fail_unless(res->data[0][i] == i + time_delay[1], "Expected and given sample points do not match");
	fail_unless(res->data[1][i] == correct[i], "Received combination does not match.");
    }

    
    fail_unless(combine_functions(NULL, time_delay, 6, 2, 1) == NULL, NULL);    
    fail_unless(combine_functions(comb, NULL, 6, 2, 1) == NULL, NULL);    
    fail_unless(combine_functions(comb, time_delay, 0, 2, 1) == NULL, NULL);
    fail_unless(combine_functions(comb, time_delay, 6, 0, 1) == NULL, NULL);
    fail_unless(combine_functions(comb, time_delay, 6, 2, 0) == NULL, NULL);
    fail_unless(combine_functions(comb, time_delay, -1, 2, 1) == NULL, NULL);
    fail_unless(combine_functions(comb, time_delay, 6, -1, 1) == NULL, NULL);
    fail_unless(combine_functions(comb, time_delay, 6, -1, 0) == NULL, NULL);

    free_est_arr(a);
    free_est_arr(b);
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

Suite* combinefunction_suite(void)
{
    Suite* s = suite_create("combinefunction");
    TCase* tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_combine_functions);
    tcase_add_test(tc_core, test_estimate_at_point);
    tcase_add_test(tc_core, test_data_at_point);

    suite_add_tcase(s, tc_core);

    return s;
}
