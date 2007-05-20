dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.61])
AC_INIT([RGLClock], [2.0.0], [rglclock@lists.yaromai.net], [rglclock])
AC_CONFIG_SRCDIR([lib/glclock.cc])
AM_INIT_AUTOMAKE([1.9.0])
AC_LIBTOOL_DLOPEN
AC_LIBLTDL_INSTALLABLE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_LIBTOOL
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
AC_CHECK_FUNCS([asprintf gettimeofday mkdir])

gl_INIT
ALL_LINGUAS="en ja"
AM_GNU_GETTEXT([external])
LTDL_CPPFLAGS="$INCLTDL"
AC_SUBST([LTDL_CPPFLAGS])dnl
LTDL_LIBS="$LIBLTDL"
AC_SUBST([LTDL_LIBS])dnl

AC_CONFIG_FILES([Makefile libgnu/Makefile \
include/Makefile lib/Makefile modules/Makefile programs/Makefile \
doc/Makefile po/Makefile.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_SUBDIRS([libltdl])
AC_OUTPUT
