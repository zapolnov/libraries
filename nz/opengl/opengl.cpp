/*
 * Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "opengl.h"

#ifdef Z_TARGET_QT5
 #include "qt5/opengl.h"
#elif defined(Z_OPENGL_FOUND)
 #include "normal/opengl.h"
#else
 #include "dummy/opengl.h"
#endif

GL::Int gl::GetInteger(GL::Enum param)
{
    GL::Int value = 0;
    gl::GetIntegerv(param, &value);
    return value;
}

void gl::EnableOrDisable(GL::Enum cap, bool flag)
{
    if (flag)
        gl::Enable(cap);
    else
        gl::Disable(cap);
}

void gl::BindAttribLocation(GL::UInt program, Z::GLAttribute index, const GL::Char* name)
{
    BindAttribLocation(program, int(index), name);
}

void gl::DisableVertexAttribArray(Z::GLAttribute index)
{
    DisableVertexAttribArray(GL::UInt(index));
}

void gl::EnableVertexAttribArray(Z::GLAttribute index)
{
    EnableVertexAttribArray(GL::UInt(index));
}

void gl::VertexAttrib1f(Z::GLAttribute index, GL::Float x)
{
    VertexAttrib1f(GL::UInt(index), x);
}

void gl::VertexAttrib1fv(Z::GLAttribute index, const GL::Float* values)
{
    VertexAttrib1fv(GL::UInt(index), values);
}

void gl::VertexAttrib2f(Z::GLAttribute index, GL::Float x, GL::Float y)
{
    VertexAttrib2f(GL::UInt(index), x, y);
}

void gl::VertexAttrib2fv(Z::GLAttribute index, const GL::Float* values)
{
    VertexAttrib2fv(GL::UInt(index), values);
}

void gl::VertexAttrib3f(Z::GLAttribute index, GL::Float x, GL::Float y, GL::Float z)
{
    VertexAttrib3f(GL::UInt(index), x, y, z);
}

void gl::VertexAttrib3fv(Z::GLAttribute index, const GL::Float* values)
{
    VertexAttrib3fv(GL::UInt(index), values);
}

void gl::VertexAttrib4f(Z::GLAttribute index, GL::Float x, GL::Float y, GL::Float z, GL::Float w)
{
    VertexAttrib4f(GL::UInt(index), x, y, z, w);
}

void gl::VertexAttrib4fv(Z::GLAttribute index, const GL::Float* values)
{
    VertexAttrib4fv(GL::UInt(index), values);
}

void gl::VertexAttribPointer(Z::GLAttribute index, GL::Int size, GL::Enum type, GL::Boolean norm, GL::Sizei stride,
    const void* ptr)
{
    VertexAttribPointer(GL::UInt(index), size, type, norm, stride, ptr);
}
