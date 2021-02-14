// g_ptr.h
// Copyright (C) 1999 Hypercore Software Design, Ltd.
// Copyright (C) 2021 Kaz Nishimura
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

#ifndef G_PTR_H
#define G_PTR_H 1

#include <glib-object.h>
#include <utility>

template<class T> class g_ptr
{
private:

    T *_ptr = nullptr;

public:

    // Constructors.

    constexpr g_ptr() noexcept
    {
        // Nothing to do.
    }

    constexpr explicit g_ptr(nullptr_t) noexcept
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


    // Destructor.

    ~g_ptr()
    {
        unref();
    }


    // Assignment operators.

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


    void swap(g_ptr &other) noexcept
    {
        using std::swap;
        if (this != &other) {
            swap(_ptr, other._ptr);
        }
    }

    void reset(T *ptr = nullptr)
    {
        if (_ptr != ptr) {
            unref();
            _ptr = ptr;
            ref_sink();
        }
    }

    T *release() noexcept
    {
        using std::swap;
        T *ptr = nullptr;
        swap(_ptr, ptr);
        return ptr;
    }

    T *get() const noexcept
    {
        return _ptr;
    }

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
    {
        if (_ptr != nullptr) {
            g_object_ref(G_OBJECT(_ptr));
        }
    }

    void ref_sink() const
    {
        if (_ptr != nullptr) {
            g_object_ref_sink(G_OBJECT(_ptr));
        }
    }

    void unref() const
    {
        if (_ptr != nullptr) {
            g_object_unref(G_OBJECT(_ptr));
        }
    }
};

#endif
