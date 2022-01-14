/* Module interface for rglclock.
   Copyright (C) 1998, 2000 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   As a special exception, you may copy and distribute this program
   linked with an OpenGL library in executable form without the source
   code for the library, only if such distribution is not prohibited
   by the library's license.  */

#ifndef _RGLCLOCKMOD_H
#define _RGLCLOCKMOD_H 1

#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#include <GL/gl.h>
#include <sys/types.h>

#define RGLCLOCK_INTERFACE_NAME "rglclock"

BEGIN_DECLS

struct rglclockmod_version_1;

/* Module interface.  */
union rglclockmod_interface
{
  struct rglclockmod_version_1 *version_1;
};

typedef int (*query_interface_function)(const char *iname, int version_max,
					union rglclockmod_interface *iface);

struct rglclockmod_version_1
{
  int (*open)(void **);
  int (*close)(void *);
  int (*set_up)(void *);
  int (*draw)(void *, const GLfloat model_matrix[4][4]);
  int (*get_property_names)(void *, const char **, size_t);
  int (*get_property)(void *, const char *, char *, size_t);
  int (*set_property)(void *, const char *, const char *);
};

END_DECLS

#endif /* not _RGLCLOCKMOD_H */
