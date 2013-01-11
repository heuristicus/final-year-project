TESTS = check_all
check_PROGRAMS = check_all
check_all_SOURCES = testmain.c check_file_util.c check_paramlist.c check_math_util.c check_general_util.c tests.h
check_all_CFLAGS = -I$(top_builddir)/src -L$(top_builddir)/src -lm -lgmp -lcheck @CHECK_CFLAGS@
check_all_LDADD = $(top_builddir)/src/libfile_util.la $(top_builddir)/src/libparamlist.la $(top_builddir)/src/libgeneral_util.la $(top_builddir)/src/libmath_util.la @CHECK_LIBS@

#gcc -g -Wall -Werror -o testmain testmain.c check_paramlist.c check_general_util.c check_file_util.c check_math_util.c -I../src/ -L../src/.libs -lmath_util -lfile_util -lgeneral_util -lparamlist -lgmp -lm -lcheck