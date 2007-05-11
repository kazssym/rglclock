#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([RGLClock], [1.5.1], [rglclock@lists.yaromai.net], [rglclock])
AC_CONFIG_SRCDIR([glclock.cc])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_USE_SYSTEM_EXTENSIONS

# Checks for libraries.
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([z], [gzopen])
AC_CHECK_LIB([xml], [xmlParseDocument])
AM_PATH_GTK([1.2.0])
AC_CHECK_LIB([GL], [glClear], [GL_LIBS="-lGL $GL_LIBS"],, [$GL_LIBS $GTK_LIBS])
AC_CHECK_LIB([GLU], [gluLookAt], [GL_LIBS="-lGLU $GL_LIBS"],, [$GL_LIBS $GTK_LIBS])
AC_SUBST([GL_LIBS])dnl

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([gnome-xml/parser.h stdlib.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([asprintf gettimeofday memset mkdir sqrt])

ALL_LINGUAS="ja"
AM_GNU_GETTEXT
AC_CONFIG_FILES([Makefile po/Makefile.in intl/Makefile])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT
