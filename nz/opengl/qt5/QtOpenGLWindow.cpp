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
#include "QtOpenGLWindow.h"
#include "utility/debug.h"
#include <QResizeEvent>
#include <QDesktopWidget>

namespace Z
{
    QtOpenGLWindow::QtOpenGLWindow(OpenGLWindowDelegate* delegate, QWidget* parent)
        : QGLWidget(parent)
        , m_RenderThread(this, delegate)
    {
        Z_ASSERT(delegate != nullptr);
        resize(delegate->preferredScreenWidth(), delegate->preferredScreenHeight());

        if (!parent) {
            QRect position = frameGeometry();
            position.moveCenter(QDesktopWidget().availableGeometry().center());
            move(position.topLeft());
        }

        QGLFormat openGLFormat;
        openGLFormat.setSwapInterval(1);
        openGLFormat.setDoubleBuffer(true);

        int depthBits = delegate->preferredDepthBufferBits();
        if (depthBits <= 0)
            openGLFormat.setDepth(false);
        else
            openGLFormat.setDepthBufferSize(depthBits);

        int stencilBits = delegate->preferredStencilBufferBits();
        if (stencilBits <= 0)
            openGLFormat.setStencil(false);
        else
            openGLFormat.setStencilBufferSize(stencilBits);

        setFormat(openGLFormat);
    }

    QtOpenGLWindow::~QtOpenGLWindow()
    {
        m_RenderThread.postShutdown();
        m_RenderThread.wait();
    }

    void QtOpenGLWindow::resizeEvent(QResizeEvent* resizeEvent)
    {
        if (m_Initialized)
        {
            int width = resizeEvent->size().width();
            int height = resizeEvent->size().height();
            m_RenderThread.postResize(width, height);
        }
    }

    void QtOpenGLWindow::paintEvent(QPaintEvent*)
    {
        if (!m_Initialized)
        {
            m_RenderThread.start(width(), height());
            m_Initialized = true;
        }
    }

    void QtOpenGLWindow::showEvent(QShowEvent*)
    {
        if (m_Initialized)
            m_RenderThread.resume();
    }

    void QtOpenGLWindow::hideEvent(QHideEvent*)
    {
        if (m_Initialized)
            m_RenderThread.suspend();
    }

    void QtOpenGLWindow::mousePressEvent(QMouseEvent*)
    {
    }

    void QtOpenGLWindow::mouseMoveEvent(QMouseEvent*)
    {
    }

    void QtOpenGLWindow::mouseReleaseEvent(QMouseEvent*)
    {
    }
}
