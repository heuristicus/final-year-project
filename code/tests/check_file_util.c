#include <check.h>
#include "tests.h"
#include "../file_util.h"
#include "../paramlist.h"

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

Suite* file_util_suite(void)
{
    Suite* s = suite_create("file_util");
    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_get_parameters);
    tcase_add_test(tc_core, test_get_event_data_all);
    tcase_add_test(tc_core, test_get_event_data_interval);
    tcase_add_test(tc_core, test_valid_param);
    
    
    suite_add_tcase(s, tc_core);

    return s;
}

