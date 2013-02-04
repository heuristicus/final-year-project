#include "tests.h"
#include "experimenter.h"

START_TEST(test_parse_double_range)
{

    string_arr* a = malloc(sizeof(string_arr));
    char *st[] = {"1.0", "2.0", "...", "5.0"};
    a->data = st;
    a->len = sizeof(st)/sizeof(char*);

    string_arr* a1 = malloc(sizeof(string_arr));
    char *st0[] = {"1.0", "1.5", "...", "5.0"};
    a1->data = st0;
    a1->len = sizeof(st0)/sizeof(char*);
    
    string_arr* a2 = malloc(sizeof(string_arr));
    char *st01[] = {"1.0", "1.3", "...", "3.8"};
    a2->data = st01;
    a2->len = sizeof(st01)/sizeof(char*);
    
    double_arr* r = parse_double_range(a);
    double_arr* r1 = parse_double_range(a1);
    double_arr* r2 = parse_double_range(a2);
    
    double correct[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double correct1[] = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0};
    double correct2[] = {1.0, 1.3, 1.6, 1.9, 2.2, 2.5, 2.8, 3.1, 3.4, 3.7, 3.8};
        
    int i;
    
    for (i = 0; i < sizeof(correct)/sizeof(double); ++i) {
	double diff = r->data[i] - correct[i];
    	fail_unless(diff >= 0 && diff < 0.000001, NULL);
    }

    for (i = 0; i < sizeof(correct1)/sizeof(double); ++i) {
	double diff = r1->data[i] - correct1[i];
    	fail_unless(diff >= 0 && diff < 0.000001, NULL);
    }
    

    for (i = 0; i < sizeof(correct2)/sizeof(double); ++i) {
	double diff = r2->data[i] - correct2[i];
    	fail_unless(diff >= 0 && diff < 0.000001, NULL);
    }

    string_arr* b = malloc(sizeof(string_arr));
    char *st1[] = {"1.0", "2.0", "5.0"};
    b->data = st1;
    b->len = sizeof(st1)/sizeof(char*);
    
    string_arr* c = malloc(sizeof(string_arr));
    char *st2[] = {"...", "2.0", "5.0"};
    c->data = st2;
    c->len = sizeof(st1)/sizeof(char*);

    string_arr* d = malloc(sizeof(string_arr));
    char *st3[] = {"1.0", "2.0", "..."};
    d->data = st3;
    d->len = sizeof(st1)/sizeof(char*);
    
    fail_unless(parse_double_range(b) == NULL, NULL);
    fail_unless(parse_double_range(c) == NULL, NULL);
    fail_unless(parse_double_range(d) == NULL, NULL);
    
}
END_TEST

START_TEST(test_parse_int_range)
{
    string_arr* a = malloc(sizeof(string_arr));
    char *st[] = {"1", "2", "...", "5"};
    a->data = st;
    a->len = sizeof(st)/sizeof(char*);

    int_arr* r = parse_int_range(a);
    
    double correct[] = {1, 2, 3, 4, 5};
        
    int i;
    
    for (i = 0; i < r->len; ++i) {
    	fail_unless(r->data[i] == correct[i], NULL);
    }

    string_arr* b = malloc(sizeof(string_arr));
    char *st1[] = {"1", "2", "5"};
    b->data = st1;
    b->len = sizeof(st1)/sizeof(char*);
    
    string_arr* c = malloc(sizeof(string_arr));
    char *st2[] = {"...", "2", "5"};
    c->data = st2;
    c->len = sizeof(st1)/sizeof(char*);

    string_arr* d = malloc(sizeof(string_arr));
    char *st3[] = {"1", "2", "..."};
    d->data = st3;
    d->len = sizeof(st1)/sizeof(char*);
    
    fail_unless(parse_int_range(b) == NULL, NULL);
    fail_unless(parse_int_range(c) == NULL, NULL);
    fail_unless(parse_int_range(d) == NULL, NULL);
}
END_TEST

START_TEST(test_parse_param)
{
    paramlist* p = init_list("p1", "1,2,3,4,5");
    p = add(p, "p2", "1,2,...,5");

    double correct[] = {1,2,3,4,5};

    double_arr* res1 = parse_param(p, "p1");
    double_arr* res2 = parse_param(p, "p2");
    
    int i;
    
    for (i = 0; i < sizeof(correct)/sizeof(double); ++i) {
	fail_unless(res1->data[i] == correct[i]);
	fail_unless(res2->data[i] == correct[i]);
    }

}
END_TEST

Suite* experimenter_suite(void)
{
    Suite* s = suite_create("experimenter");
    TCase* tc_core = tcase_create("Core");
    
    tcase_add_test(tc_core, test_parse_double_range);
    tcase_add_test(tc_core, test_parse_int_range);
    tcase_add_test(tc_core, test_parse_param);

    suite_add_tcase(s, tc_core);

    return s;
}
