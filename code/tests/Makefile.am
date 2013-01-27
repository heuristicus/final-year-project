TESTS = check_all
check_PROGRAMS = check_all
check_all_SOURCES = testmain.c check_file_util.c check_paramlist.c check_math_util.c check_general_util.c check_estimator.c check_combinefunction.c tests.h ../src/estimator.c ../src/estimator_IWLS.c ../src/estimator_piecewise.c ../src/estimator_baseline.c estimator.h ../src/combinefunction.c 
check_all_CFLAGS = -I$(top_builddir)/src -L$(top_builddir)/src -lm -lgsl -lcheck @CHECK_CFLAGS@
check_all_LDADD = $(top_builddir)/src/libfile_util.la $(top_builddir)/src/libparamlist.la $(top_builddir)/src/libgeneral_util.la $(top_builddir)/src/libmath_util.la @CHECK_LIBS@

#gcc -g -Wall -Werror -o testmain testmain.c check_paramlist.c check_general_util.c check_file_util.c check_math_util.c -I../src/ -L../src/.libs -lmath_util -lfile_util -lgeneral_util -lparamlist -lgmp -lm -lcheck