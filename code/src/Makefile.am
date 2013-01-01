lib_LTLIBRARIES = libfile_util.la libmath_util.la libgeneral_util.la libparamlist.la
libfile_util_la_SOURCES = file_util.c file_util.h
libmath_util_la_SOURCES = math_util.c math_util.h
libgeneral_util_la_SOURCES = general_util.c general_util.h
libparamlist_la_SOURCES = paramlist.c paramlist.h

bin_PROGRAMS = generator estimator estimator_piecewise estimator_baseline
# DIRTY DIRTY HACK!
generator_CFLAGS = -I/usr/include/muParser
generator_SOURCES = generator.c start.c generator.h
estimator_SOURCES = estimator_IWLS.c estimator.h
estimator_piecewise_SOURCES = estimator_IWLS.c estimator_piecewise.c estimator.h
estimator_baseline_SOURCES = estimator_IWLS.c estimator_piecewise.c estimator_baseline.c estimator.h
generator_LDADD = libfile_util.la libmath_util.la libgeneral_util.la libparamlist.la
estimator_LDADD = libfile_util.la libmath_util.la libgeneral_util.la libparamlist.la
estimator_piecewise_LDADD = libfile_util.la libmath_util.la libgeneral_util.la libparamlist.la
estimator_baseline_LDADD = libfile_util.la libmath_util.la libgeneral_util.la libparamlist.la
