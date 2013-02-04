#include <check.h>
#include "tests.h"
#include "file_util.h"
#include "paramlist.h"

START_TEST (test_get_parameters)
{
    paramlist* a = get_parameters("./files/testparam.txt");
    fail_unless(strcmp(get_string_param(a, "test"), "1") == 0, NULL);
    fail_unless(strcmp(get_string_param(a, "anothertest"), "2") == 0, NULL);
    fail_unless(strcmp(get_string_param(a, "multi"), "1,2,3") == 0, NULL);
    fail_unless(get_string_param(a, " ") == NULL, NULL);
    fail_unless(get_string_param(a, "\t") == NULL, NULL);
}
END_TEST

START_TEST (test_get_event_data_all)
{
    double data[15] = {0.1,0.3,0.8,1.1,1.6,2.4,2.9,3.0,3.8,4.3,4.4,4.8,5.0,5.1,5.9};
    double* file = get_event_data_all("./files/testdata.txt");

    // length of array in index 0
    fail_unless(file[0] == 16, NULL);
    
    int i;
    
    for (i = 0; i < 15; ++i) {
	//printf("%lf %lf\n", data[i], file[i + 1]);
	fail_unless(data[i] == file[i + 1], NULL);
    }

    
}
END_TEST

START_TEST (test_get_event_data_interval)
{
    double data[15] = {0.1,0.3,0.8,1.1,1.6,2.4,2.9,3.0,3.8,4.3,4.4,4.8,5.0,5.1,5.9};
    double* a = get_event_data_interval(1, 2, "./files/testdata.txt");

    // length of array in index 0
    fail_unless(a[0] == 3, NULL);
    
    int i;
    
    for (i = 1; i < a[i]; ++i) {
	//printf("%lf %lf\n", data[i + 2], a[i]);
	fail_unless(data[i + 2] == a[i], NULL);
    }

    // check that the "get all" functionality works ok with negatives
    double* b = get_event_data_interval(-1, -1, "./files/testdata.txt");
    
    fail_unless(b[0] == 16, NULL);
    
    for (i = 0; i < 15; ++i) {
	//printf("%lf %lf\n", data[i], file[i + 1]);
	fail_unless(data[i] == b[i + 1], NULL);
    }

    // empty interval
    double* c = get_event_data_interval(6, 7, "./files/testdata.txt");
    fail_unless(c[0] == 1);

    // Check invalid intervals
    double* d = get_event_data_interval(-1, 0, "./files/testdata.txt");
    fail_unless(d == NULL);
    d = get_event_data_interval(0.1, 0.01, "./files/testdata.txt");
    fail_unless(d == NULL);
            
}
END_TEST

START_TEST (test_valid_param)
{
    fail_unless(valid_param("   ") == 0, NULL);
    fail_unless(valid_param(" __test1 test") == 0, NULL);
    fail_unless(valid_param("testing  twospace") == 0, NULL);
    fail_unless(valid_param("testingtab\ttab") == 0, NULL);
        
    fail_unless(valid_param("!\"£$%^&*() chars") == 1, NULL);
    fail_unless(valid_param("test one") == 1, NULL);
    fail_unless(valid_param("test_two two") == 1, NULL);
    fail_unless(valid_param("test3 53,22") == 1, NULL);

}
END_TEST

START_TEST(test_output_gaussian_vector)
{
    gaussian* g1 = make_gaussian(2, 3);
    gaussian* g2 = make_gaussian(4, 2);
    gauss_vector* G = malloc(sizeof(gauss_vector));
    gaussian** gs = malloc(2 * sizeof(gaussian*));
    gs[0] = g1;
    gs[1] = g2;
        
    G->gaussians = gs;
    G->len = 2;
    G->w = malloc(2 * sizeof(double));
    G->w[0] = 1.5;
    G->w[1] = 3.5;

    output_gaussian_vector("files/gauss_out", "w", G);

    FILE *fp = fopen("files/gauss_out", "r");

    double mu, st, wt;

    char* line = malloc(100);

    /* while ((line = fgets(line, 100, fp)) != NULL){ */
    /* 	printf("line %s\n", line); */
    /* } */
    
    line = fgets(line, 100, fp);
    int s = sscanf(line, "%lf %lf %lf", &mu, &st, &wt);

    fail_unless(s == 3, NULL);
    fail_unless(mu == 2, NULL);
    fail_unless(st == 3, NULL);
    fail_unless(wt == 1.5, NULL);
    
    line = fgets(line, 100, fp);
    int t = sscanf(line, "%lf %lf %lf", &mu, &st, &wt);

    fail_unless(t == 3, NULL);
    fail_unless(mu == 4, NULL);
    fail_unless(st == 2, NULL);
    fail_unless(wt == 3.5, NULL);

    fclose(fp);

}
END_TEST

START_TEST(test_read_gauss_vector)
{
    gauss_vector* G = read_gauss_vector("files/gauss_out");

    fail_unless(G->gaussians[0]->mean == 2, NULL);
    fail_unless(G->gaussians[0]->stdev == 3, NULL);
    fail_unless(G->w[0] == 1.5, NULL);

    fail_unless(G->gaussians[1]->mean == 4, NULL);
    fail_unless(G->gaussians[1]->stdev == 2, NULL);
    fail_unless(G->w[1] == 3.5, NULL);
}
END_TEST

START_TEST(test_output_double_multi_arr)
{
    double_multi_arr* ck = malloc(sizeof(double_multi_arr));
    double** dt = malloc(3 * sizeof(double*));
    double* a = malloc(3 * sizeof(double));
    double* b = malloc(3 * sizeof(double));
    double* c = malloc(3 * sizeof(double));

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
 
    b[0] = 4;
    b[1] = 5;
    b[2] = 6;
    
    c[0] = 7;
    c[1] = 8;
    c[2] = 9;
    
    dt[0] = a;
    dt[1] = b;
    dt[2] = c;

    int ls[] = {3,3,3};
    int l = 3;
    
    ck->data = dt;
    ck->lengths = ls;
    ck->len = l;
    
    output_double_multi_arr("files/multi_arr", "w", ck);

    FILE *fp = fopen("files/multi_arr", "r");
    
    char* line = malloc(100);
    
    double one, two, three;

    line = fgets(line, 100, fp);
    int t = sscanf(line, "%lf %lf %lf", &one, &two, &three);
    fail_unless(one == 1 && two == 4 && three == 7, NULL);
    
    line = fgets(line, 100, fp);
    t = sscanf(line, "%lf %lf %lf", &one, &two, &three);
    fail_unless(one == 2 && two == 5 && three == 8, NULL);
    
    
    line = fgets(line, 100, fp);
    t = sscanf(line, "%lf %lf %lf", &one, &two, &three);
    fail_unless(one == 3 && two == 6 && three == 9, NULL);

    fclose(fp);
}
END_TEST

Suite* file_util_suite(void)
{
    Suite* s = suite_create("file_util");
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_get_parameters);
    tcase_add_test(tc_core, test_get_event_data_all);
    tcase_add_test(tc_core, test_get_event_data_interval);
    tcase_add_test(tc_core, test_valid_param);
    tcase_add_test(tc_core, test_output_gaussian_vector);
    tcase_add_test(tc_core, test_read_gauss_vector);    
    tcase_add_test(tc_core, test_output_double_multi_arr);
    
    suite_add_tcase(s, tc_core);

    return s;
}
