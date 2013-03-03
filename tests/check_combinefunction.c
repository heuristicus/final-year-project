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

    double_arr* td = init_double_arr(2);
    double time_delay[] = {0, 1};
    td->data = time_delay;

    double_multi_arr* res = combine_functions(comb, td, 0, 6, 1, 2);
    int i;

    double correct[] = {6.5, 6.5, 7.5, 10, 10};
    
    for (i = 0; i < sizeof(correct)/sizeof(double); ++i) {
	fail_unless(res->data[0][i] == i + td->data[1], "Expected and given sample points do not match");
	fail_unless(res->data[1][i] == correct[i], "Received combination does not match.");
    }

    
    fail_unless(combine_functions(NULL, td, 0, 6, 1, 2) == NULL, NULL);    
    fail_unless(combine_functions(comb, NULL, 0, 6, 1, 2) == NULL, NULL);    
    fail_unless(combine_functions(comb, td, 0, 0, 1, 2) == NULL, NULL);
    fail_unless(combine_functions(comb, td, 0, 6, 1, 0) == NULL, NULL);
    fail_unless(combine_functions(comb, td, 0, 6, 0, 2) == NULL, NULL);
    fail_unless(combine_functions(comb, td, 0, -1, 1, 2) == NULL, NULL);
    fail_unless(combine_functions(comb, td, 0, 6, 1, -1) == NULL, NULL);
    fail_unless(combine_functions(comb, td, 0, 6, 0, -1) == NULL, NULL);

    free_est_arr(a);
    free_est_arr(b);
}
END_TEST

START_TEST(test_combine_gauss_vectors)
{
    double means[] = {2, 4, 6};
    
    gauss_vector* G1 = gen_gaussian_vector_from_array(means, sizeof(means)/sizeof(double), 3, 1, 0);
    gauss_vector* G2 = gen_gaussian_vector_from_array(means, sizeof(means)/sizeof(double), 3, 1, 0);

    gauss_vector* V[] = {G1, G2};

    double_arr* delay = init_double_arr(2);
    double td[] = {0, 0};
    delay->data = td;
    double start = 0, interval = 8, step = 1;
    
    double_multi_arr* comb = combine_gauss_vectors(V, delay, start, interval, step, 2);

    // values for mean 2 from 0-8
    /* double m2[] = {0.800737402, 0.945959468, 1.0, 0.945959468, 0.800737402, */
    /* 		   0.606530659, 0.411112290, 0.249352208, 0.135335283}; */
    /* // values for mean 4 from 0-8 */
    /* double m4[] = {0.411112290, 0.606530659, 0.800737402, 0.945959468, 1.0, */
    /* 		   0.945959468, 0.800737402, 0.606530659, 0.411112290}; */
    /* // values for mean 6 from 0-8 */
    /* double m6[] = {0.135335283, 0.249352208, 0.411112290, 0.606530659, */
    /* 		   0.800737402, 0.945959468, 1.0, 0.945959468, 0.800737402}; */
    

    // Since we pass the same gaussian vector twice, the values should come out
    // as the sum, because we take the average of the sums at each point.
    
    double sums[] = {1.347184975, 1.801842335, 2.211849692, 2.498449595, 
		     2.601474804, 2.498449595, 2.211849692, 1.801842335, 1.347184975};

    int i;
    
    for (i = 0; i < (int)((start + interval) / step); ++i) {
	printf("comb %lf, correct %lf\n", comb->data[1][i], sums[i]);
	fail_unless(epsck(comb->data[1][i], sums[i]), NULL);
    }

    fail_unless(combine_gauss_vectors(NULL, delay, start, interval, step, 2) == NULL, NULL);
    fail_unless(combine_gauss_vectors(V, NULL, start, interval, step, 2) == NULL, NULL);
    fail_unless(combine_gauss_vectors(V, delay, start, 0, step, 2) == NULL, NULL);
    fail_unless(combine_gauss_vectors(V, delay, start, interval, 0, 2) == NULL, NULL);
    fail_unless(combine_gauss_vectors(V, delay, start, interval, step, 0) == NULL, NULL);
}
END_TEST

Suite* combinefunction_suite(void)
{
    Suite* s = suite_create("combinefunction");
    TCase* tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_combine_functions);
    tcase_add_test(tc_core, test_combine_gauss_vectors);

    suite_add_tcase(s, tc_core);

    return s;
}
