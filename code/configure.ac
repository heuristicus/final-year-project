# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([fyp], [0.2], [mxs968@cs.bham.ac.uk])
AC_CONFIG_SRCDIR([src/math_util.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
AC_SEARCH_LIBS([pow], [m], , AC_MSG_ERROR([Could not find standard math library.]))
# Don't forget - muparser not muParser!
AC_SEARCH_LIBS([mupCreate], [muparser], ,AC_MSG_ERROR([Could not find muparser library.]))
AC_SEARCH_LIBS([__gmpz_init], [gmp], ,AC_MSG_ERROR([Could not find gmp library.]))

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h muParser.h gmp.h math.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday pow sqrt strdup])

AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
