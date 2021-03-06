/* -*-C++-*- */
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

#ifndef PROFILE_H
#define PROFILE_H 1

#include "clock.h"
#include <string>

/* Options profile of a clock.  This object keeps the options setting
   in a file.  */
class profile
{
private:
  std::string file_name;
  bool changed;
public:
  profile();
  ~profile();
public:
  /* Opens a file with NAME to keep options settings.  */
  void open(const char *name);

  /* Restores all options setting of CLOCK.  */
  void restore(movement *clock);

  /* Saves all options setting of CLOCK.  */
  void save(movement *clock);

  void options_changed(movement *);
};

#endif /* not PROFILE_H */
