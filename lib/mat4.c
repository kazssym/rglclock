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

#include <math.h>

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
        for (int j = 0; j != 4; j++) {
            for (int k = 0; k != 4; k++) {
                z[i][k] += x[j][k] * y[i][j];
            }
        }
    }
}

void mat4_rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, GLfloat rotation[restrict 4][4])
{
    GLfloat n = sqrtf(x * x + y * y + z * z);
    if (n != 0) {
        x /= n;
        y /= n;
        z /= n;
    }

    GLfloat c = cosf(angle);
    GLfloat s = sinf(angle);
    // Note this is a column-major matrix.
    GLfloat matrix[4][4] = {
        {x * x * (1 - c) +     c, y * x * (1 - c) + z * s, z * x * (1 - c) - y * s, 0},
        {x * y * (1 - c) - z * s, y * y * (1 - c) +     c, z * y * (1 - c) + x * s, 0},
        {x * z * (1 - c) + y * s, y * z * (1 - c) - x * s, z * z * (1 - c) +     c, 0},
        {0,                       0,                       0,                       1},
    };
    mat4_copy(matrix, rotation);
}
