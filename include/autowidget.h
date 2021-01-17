/* -*-C++-*- */
/* Copyright (C) 1999 Hypercore Software Design, Ltd.
   Copyright (C) 2021 Kaz Nishimura

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

#ifndef AUTOWIDGET_H
#define AUTOWIDGET_H 1

#include <glib-object.h>

template<class Object> class g_ptr
{
private:
  Object *object;
public:
  g_ptr() throw()
    : object(0) {}
  g_ptr(Object *x)
    : object(x) {ref_sink();}
  g_ptr(const g_ptr &another)
    : object(another.object) {ref();}
  ~g_ptr()
    {unref();}
public:
  g_ptr &operator=(const g_ptr &another)
    {
      if (&another != this)
        {
          unref();
          object = another.object;
          ref();
        }
      return *this;
    }
  Object &operator*() const throw()
    {return *object;}
  Object *operator->() const throw()
    {return object;}
  Object *get() const throw()
    {return object;}
protected:
  void ref() const
    {if (object != 0) {g_object_ref(G_OBJECT(object));}}
  void ref_sink() const
    {
      if (object != 0)
        {
          g_object_ref_sink(G_OBJECT(object));
        }
    }
  void unref() const
    {if (object != 0) g_object_unref(G_OBJECT(object));}
};

#endif /* not AUTOWIDGET_H */

