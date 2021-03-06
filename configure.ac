# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([fyp], [0.2], [mxs968@cs.bham.ac.uk])
AC_CONFIG_SRCDIR([src/math_util.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

# Checks for libraries.
AC_SEARCH_LIBS([cos], [m], , AC_MSG_ERROR([Could not find standard math library.]))
# Don't forget - muparser not muParser!
AC_SEARCH_LIBS([mupCreate], [muparser], ,AC_MSG_ERROR([Could not find muparser library.]))
#AC_SEARCH_LIBS([__gmpz_init], [gmp], ,AC_MSG_ERROR([Could not find gmp library.]))
AC_SEARCH_LIBS([cblas_dgemm], [gslcblas], ,AC_MSG_ERROR([Could not find gslcblas library.]))
AC_SEARCH_LIBS([gsl_blas_dgemm], [gsl], ,AC_MSG_ERROR([Could not find gsl library.]))
AC_CHECK_LIB([m],[cos])
AC_CHECK_LIB([gslcblas],[cblas_dgemm])
AC_CHECK_LIB([gsl],[gsl_blas_dgemm])
# Use pkg-config to set up the appropriate flags for muparser and gmp
PKG_CHECK_MODULES([DEPS], [muparser >= 2.2.3])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for header files.
AC_CHECK_FILE([/usr/include/muParser.h], ,AC_MSG_ERROR([muParser header not found.]))
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h], ,AC_MSG_ERROR([Header file missing.]))

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday pow sqrt strdup])

AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile
                 src/Makefile
		 tests/Makefile])
AC_OUTPUT