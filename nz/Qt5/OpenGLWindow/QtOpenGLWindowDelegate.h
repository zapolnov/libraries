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

#pragma once
#include <QSize>

namespace Z
{
    class QtOpenGLWindowDelegate
    {
    public:
        virtual ~QtOpenGLWindowDelegate() = default;

        virtual QSize preferredWindowSize() const { return QSize(1024, 768); }
        virtual int preferredDepthBufferBits() const { return 16; }
        virtual int preferredStencilBufferBits() const { return 0; }

        virtual void initializeGL(int width, int height) { (void)width; (void)height; }
        virtual void shutdownGL() {}
        virtual void suspendGL() {}
        virtual void resumeGL() {}

        virtual void resizeGL(int width, int height) { (void)width; (void)height; }
        virtual void renderGL(double time) = 0;

        virtual void onPointerPressed(int id, float x, float y) { (void)id; (void)x; (void)y; }
        virtual void onPointerMoved(int id, float x, float y) { (void)id; (void)x; (void)y; }
        virtual void onPointerReleased(int id, float x, float y) { (void)id; (void)x; (void)y; }

        virtual void onKeyPressed(int key) { (void)key; }
        virtual void onKeyReleased(int key) { (void)key; }
    };
}
