dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.61])
AC_INIT([RGLClock], [1.4.1], [rglclock@lists.yaromai.net], [rglclock])
AC_CONFIG_SRCDIR([lib/glclock.cc])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.9])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_CC_STDC
AC_USE_SYSTEM_EXTENSIONS

dnl Checks for libraries.
AC_CHECK_LIB([z], [deflate])
AC_CHECK_LIB([xml], [xmlParseDocument])
AC_CHECK_LIB([png], [png_create_read_struct], [PNG_LIBS="-lpng"])
AC_SUBST([PNG_LIBS])dnl
AM_PATH_GTK([1.2.0])
AC_CHECK_LIB([GL], [glClear], [GL_LIBS="-lGL $GL_LIBS"],, [$GL_LIBS $GTK_LIBS])
AC_CHECK_LIB([GLU], [gluLookAt], [GL_LIBS="-lGLU $GL_LIBS"],, [$GL_LIBS $GTK_LIBS])
AC_SUBST([GL_LIBS])dnl
AC_CHECK_LIB([nana], [_I_default_handler])

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_ASSERT
AC_CHECK_HEADERS([unistd.h sys/time.h png.h gnome-xml/parser.h])
AC_CHECK_HEADERS([nana.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM
AC_HEADER_TIME

dnl Checks for library functions.
AC_CHECK_FUNCS([asprintf gettimeofday mkdir])

ALL_LINGUAS="ja"
AM_GNU_GETTEXT([no-libtool])
AC_CONFIG_FILES([Makefile intl/Makefile \
include/Makefile lib/Makefile modules/Makefile programs/Makefile \
doc/Makefile po/Makefile.in])
AC_OUTPUT
