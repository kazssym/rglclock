# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([RGLClock], [3.0],
[https://github.com/kazssym/rglclock/issues/new], [rglclock])
AC_CONFIG_SRCDIR([programs/clock.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])
# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
gl_EARLY
# Checks for libraries.
AC_SEARCH_LIBS([XFree], [X11])
AC_SEARCH_LIBS([glClear], [GL])
AC_SEARCH_LIBS([gluLookAt], [GLU])
AC_CHECK_LIB([ltdl], [lt_dlopen], [LTDL_LIBS="-lltdl $LTDL_LIBS"],, [$LTDL_LIBS])
AC_SUBST([LTDL_LIBS])dnl

dnl Checks for header files.
AC_HEADER_ASSERT
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([ltdl.h unistd.h sys/time.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

dnl Checks for library functions.
AC_CHECK_FUNCS([mkdir gettimeofday])

LT_INIT([dlopen])
gl_INIT

ALL_LINGUAS="en ja"
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.3])
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0])
PKG_CHECK_MODULES([LIBPNG], [libpng])

AC_CONFIG_FILES([Makefile libgnu/Makefile \
include/Makefile lib/Makefile modules/Makefile programs/Makefile \
testsuite/Makefile doc/Makefile po/Makefile.in])
AC_CONFIG_HEADER([config.h])
AC_OUTPUT
