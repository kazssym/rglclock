#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([rglclock], [1.5.0], [rglclock@lists.yaromai.net])
AC_CONFIG_SRCDIR([glclock.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
ALL_LINGUAS="ja"

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

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
AM_GNU_GETTEXT
AC_FUNC_MALLOC
AC_CHECK_FUNCS([asprintf gettimeofday memset mkdir sqrt])

AC_CONFIG_FILES([Makefile po/Makefile.in intl/Makefile])
AC_OUTPUT
