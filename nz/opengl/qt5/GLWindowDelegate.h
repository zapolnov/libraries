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
#include "../GLApplication.h"
#include "QtOpenGLWindowDelegate.h"
#include <memory>

namespace Z
{
    class GLWindowDelegate : public QtOpenGLWindowDelegate
    {
    public:
        explicit GLWindowDelegate(const GLApplicationPtr& app);

        QSize preferredWindowSize() const override;
        int preferredDepthBufferBits() const override;
        int preferredStencilBufferBits() const override;

        void initializeGL(int width, int height) override;
        void shutdownGL() override;
        void suspendGL() override;
        void resumeGL() override;

        void resizeGL(int width, int height) override;
        void renderGL(double time) override;

        void onPointerPressed(int id, float x, float y) override;
        void onPointerMoved(int id, float x, float y) override;
        void onPointerReleased(int id, float x, float y) override;

        void onKeyPressed(int key) override;
        void onKeyReleased(int key) override;

    private:
        GLApplicationPtr m_App;
    };
}
