/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com)
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
#include "GLWindowDelegate.h"

namespace Z
{
    GLWindowDelegate::GLWindowDelegate(const GLApplicationPtr& app)
        : m_App(app)
    {
    }

    QSize GLWindowDelegate::preferredWindowSize() const
    {
        return QSize(m_App->preferredScreenWidth(), m_App->preferredScreenHeight());
    }

    int GLWindowDelegate::preferredDepthBufferBits() const
    {
        return m_App->preferredDepthBufferBits();
    }

    int GLWindowDelegate::preferredStencilBufferBits() const
    {
        return m_App->preferredStencilBufferBits();
    }

    void GLWindowDelegate::initializeGL(int width, int height)
    {
        m_App->initializeGL(width, height);
    }

    void GLWindowDelegate::shutdownGL()
    {
        m_App->shutdownGL();
    }

    void GLWindowDelegate::suspendGL()
    {
        m_App->suspendGL();
    }

    void GLWindowDelegate::resumeGL()
    {
        m_App->resumeGL();
    }

    void GLWindowDelegate::resizeGL(int width, int height)
    {
        m_App->resizeGL(width, height);
    }

    void GLWindowDelegate::renderGL(double time)
    {
        m_App->renderGL(time);
    }

    void GLWindowDelegate::onPointerPressed(int id, float x, float y)
    {
        m_App->onPointerPressed(id, x, y);
    }

    void GLWindowDelegate::onPointerMoved(int id, float x, float y)
    {
        m_App->onPointerMoved(id, x, y);
    }

    void GLWindowDelegate::onPointerReleased(int id, float x, float y)
    {
        m_App->onPointerReleased(id, x, y);
    }
}
