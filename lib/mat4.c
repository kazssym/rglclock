// mat4.c
// Copyright (C) 2022 Kaz Nishimura
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "mat4.h"

void mat4_copy(const GLfloat x[restrict 4][4], GLfloat y[restrict 4][4])
{
    for (int i = 0; i != 4; i++) {
        for (int j = 0; j != 4; j++) {
            y[i][j] = x[i][j];
        }
    }
}

void mat4_multiply(const GLfloat x[restrict 4][4], const GLfloat y[restrict 4][4],
    GLfloat z[restrict 4][4])
{
    for (int i = 0; i != 4; i++) {
        for (int j = 0; j != 4; j++) {
            z[i][j] = 0;
        }
    }
    for (int i = 0; i != 4; i++) {
        for (int j = 0; j != 4; j++) {
            for (int k = 0; k != 4; k++) {
                z[i][k] += x[j][k] * y[i][j];
            }
        }
    }
}
