# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([RGLClock], [3.0],
[https://github.com/kazssym/rglclock/issues/new], [rglclock])
AC_CONFIG_SRCDIR([programs/clock.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_RANLIB
AC_USE_SYSTEM_EXTENSIONS

gl_EARLY

dnl Checks for libraries.
AC_CHECK_LIB([z], [deflate])
AC_CHECK_LIB([xml], [xmlParseDocument])
AC_CHECK_LIB([png], [png_create_read_struct], [PNG_LIBS="-lpng -lz"], [-lz])
AC_SUBST([PNG_LIBS])dnl
AM_PATH_GTK_2_0([2.8.0])
AC_CHECK_LIB([GL], [glClear], [GL_LIBS="-lGL $GL_LIBS"],, [$GL_LIBS $GTK_LIBS])
AC_CHECK_LIB([GLU], [gluLookAt], [GL_LIBS="-lGLU $GL_LIBS"],, [$GL_LIBS $GTK_LIBS])
AC_SUBST([GL_LIBS])dnl

dnl Checks for header files.
AC_HEADER_ASSERT
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([unistd.h sys/time.h png.h gnome-xml/parser.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

dnl Checks for library functions.
AC_CHECK_FUNCS([mkdir gettimeofday])

gl_INIT

ALL_LINGUAS="en ja"
AM_GNU_GETTEXT([external])

AC_LIBTOOL_DLOPEN
AC_LIBLTDL_INSTALLABLE
AC_PROG_LIBTOOL
LTDL_CPPFLAGS="$INCLTDL"
AC_SUBST([LTDL_CPPFLAGS])dnl
LTDL_LIBS="$LIBLTDL"
AC_SUBST([LTDL_LIBS])dnl

AC_CONFIG_FILES([Makefile libgnu/Makefile \
include/Makefile lib/Makefile modules/Makefile programs/Makefile \
testsuite/Makefile doc/Makefile po/Makefile.in])
AC_CONFIG_HEADER([config.h])
AC_OUTPUT
