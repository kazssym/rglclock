# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([RGLClock], [4.0],
[https://github.com/kazssym/rglclock/issues/new], [rglclock])
AC_CONFIG_SRCDIR([lib/movement.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])
# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
gl_EARLY
# Checks for libraries.
AC_CANONICAL_HOST
case "$host_os" in
mingw32* | mingw64*)
  ;;
*)
  AC_SEARCH_LIBS([XFree], [X11])
  AC_SEARCH_LIBS([glClear], [GL],, [AC_MSG_ERROR([missing OpenGL library])])
  AC_SEARCH_LIBS([gluLookAt], [GLU],, [AC_MSG_ERROR([missing GLU library])])
  ;;
esac

AC_CHECK_LIB([ltdl], [lt_dlopen], [LTDL_LIBS="-lltdl $LTDL_LIBS"],, [$LTDL_LIBS])
AC_SUBST([LTDL_LIBS])dnl
# Checks for header files.
AC_HEADER_ASSERT
AC_CHECK_HEADERS([ltdl.h])
# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
# Checks for library functions.

LT_PREREQ([2.4.6])
LT_INIT([dlopen])
gl_INIT
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])
PKG_CHECK_MODULES([GTK], [gtk+-3.0])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0])
PKG_CHECK_MODULES([LIBPNG], [libpng])

AC_CONFIG_FILES([Makefile libgnu/Makefile \
include/Makefile lib/Makefile modules/Makefile programs/Makefile \
testsuite/Makefile doc/Makefile po/Makefile.in])
AC_CONFIG_HEADER([config.h])
AC_OUTPUT
