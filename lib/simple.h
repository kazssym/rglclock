/* rglclock - Rotating GL Clock.
   Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.

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

#ifndef SIMPLE_H
#define SIMPLE_H 1

#ifdef __cplusplus
# define BEGIN_DECLS extern "C" {
# define END_DECLS }
#else /* not __cplusplus */
# define BEGIN_DECLS
# define END_DECLS
#endif /* not __cplusplus */

BEGIN_DECLS

int simple_init(void);
int simple_set_prop(const char *, const char *);
int simple_draw_clock(void);

END_DECLS

#endif /* not SIMPLE_H */
