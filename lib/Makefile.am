## Process this file with automake to produce a Makefile.in.

INCLUDES = -I${top_srcdir}/include \
${GTK_CFLAGS}

noinst_LIBRARIES = libgdkgl.a libutils.a libsimple.a libclock.a

libgdkgl_a_SOURCES = gdkgl.c

libutils_a_SOURCES = options.cc

libsimple_a_SOURCES = simple.c

libclock_a_SOURCES = module.cc glclock.cc clockoptions.cc about.cc

