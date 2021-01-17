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
#include <utility>

template<class T> class g_ptr
{
private:
    T *_ptr;

public:
    g_ptr() noexcept
    :
        g_ptr(nullptr)
    {
        // Nothing to do.
    }

    explicit g_ptr(nullptr_t) noexcept
    :
        _ptr {nullptr}
    {
        // Nothing to do.
    }

    explicit g_ptr(T *ptr)
    :
        _ptr {ptr}
    {
        ref_sink();
    }

    g_ptr(const g_ptr &other)
    :
        _ptr {other._ptr}
    {
        ref();
    }

    g_ptr(g_ptr &&other) noexcept
    :
        _ptr {other.release()}
    {
        // Nothing to do.
    }

public:
    ~g_ptr()
    {
        unref();
    }

public:
    g_ptr &operator =(const g_ptr &other)
    {
        if (this != &other) {
            unref();
            _ptr = other._ptr;
            ref();
        }
        return *this;
    }

    g_ptr &operator =(g_ptr &&other) noexcept
    {
        swap(other);
    }

public:
    void swap(g_ptr &other) noexcept
    {
        using std::swap;
        if (this != &other) {
            swap(_ptr, other._ptr);
        }
    }

public:
    T *release() noexcept
    {
        using std::swap;
        T *ptr = nullptr;
        swap(_ptr, ptr);
        return ptr;
    }

public
    T *get() const noexcept
    {
        return _ptr;
    }

public:
    T &operator *() const noexcept
    {
        return *_ptr;
    }

    T *operator ->() const noexcept
    {
        return _ptr;
    }

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

