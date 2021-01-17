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

template<class T> class g_ptr
{
private:
    T *_ptr;

public:
  g_ptr() throw()
    : _ptr(0) {}
  g_ptr(T *x)
    : _ptr(x) {ref_sink();}
  g_ptr(const g_ptr &another)
    : _ptr(another._ptr) {ref();}
  ~g_ptr()
    {unref();}
public:
  g_ptr &operator=(const g_ptr &another)
    {
      if (&another != this)
        {
          unref();
          _ptr = another._ptr;
          ref();
        }
      return *this;
    }
  T &operator*() const throw()
    {return *_ptr;}
  T *operator->() const throw()
    {return _ptr;}
  T *get() const throw()
    {return _ptr;}
protected:
  void ref() const
    {if (_ptr != 0) {g_object_ref(G_OBJECT(_ptr));}}
  void ref_sink() const
    {
      if (_ptr != 0)
        {
          g_object_ref_sink(G_OBJECT(_ptr));
        }
    }
  void unref() const
    {if (_ptr != 0) g_object_unref(G_OBJECT(_ptr));}
};

#endif /* not AUTOWIDGET_H */

