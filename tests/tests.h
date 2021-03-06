#ifndef _TEST_H
#define _TEST_H

#include <stdlib.h>
#include <check.h>
#include <stdio.h>
#include <math.h>

#define DOUBLE_EPSILON 0.0000001

Suite* math_util_suite(void);
Suite* general_util_suite(void);
Suite* paramlist_suite(void);
Suite* file_util_suite(void);
Suite* estimator_suite(void);
Suite* combinefunction_suite(void);
Suite* experimenter_suite(void);

int epsck(double result, double expected);

#endif
