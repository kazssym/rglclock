// simple.h
// Copyright (C) 1998, 1999 Hypercore Software Design, Ltd.
// Copyright (C) 2021-2022 Kaz Nishimura
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SIMPLE_H
#define SIMPLE_H 1

#ifdef __cplusplus
# define BEGIN_DECLS extern "C" {
# define END_DECLS }
#else /* not __cplusplus */
# define BEGIN_DECLS
# define END_DECLS
#endif /* not __cplusplus */

#include <rglclockmod.h>

BEGIN_DECLS

int simple_init(void);
int simple_set_prop(const char *, const char *);
int simple_draw_clock(const GLfloat model_matrix[4][4]);

int simple_LTX_query_interface(const char *, int,
			       union rglclockmod_interface *);

END_DECLS

#endif /* not SIMPLE_H */
