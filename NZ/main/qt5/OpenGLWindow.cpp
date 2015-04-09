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
#include "OpenGLWindow.h"
#include <QResizeEvent>
#include <QDesktopWidget>

namespace Z
{
    OpenGLWindow::OpenGLWindow()
        : m_RenderThread(this)
    {
        resize(1024, 768);

        QRect position = frameGeometry();
        position.moveCenter(QDesktopWidget().availableGeometry().center());
        move(position.topLeft());

        QGLFormat openGLFormat;
        openGLFormat.setDoubleBuffer(true);
        setFormat(openGLFormat);
    }

    OpenGLWindow::~OpenGLWindow()
    {
        m_RenderThread.postShutdown();
        m_RenderThread.wait();
    }

    void OpenGLWindow::resizeEvent(QResizeEvent* resizeEvent)
    {
        if (m_Initialized)
        {
            int width = resizeEvent->size().width();
            int height = resizeEvent->size().height();
            m_RenderThread.postResize(width, height);
        }
    }

    void OpenGLWindow::paintEvent(QPaintEvent*)
    {
        if (!m_Initialized)
        {
            m_RenderThread.postResize(width(), height());
            m_RenderThread.start();
            m_Initialized = true;
        }
    }

    void OpenGLWindow::showEvent(QShowEvent*)
    {
        if (m_Initialized)
            m_RenderThread.resume();
    }

    void OpenGLWindow::hideEvent(QHideEvent*)
    {
        if (m_Initialized)
            m_RenderThread.suspend();
    }

    void OpenGLWindow::mousePressEvent(QMouseEvent*)
    {
    }

    void OpenGLWindow::mouseMoveEvent(QMouseEvent*)
    {
    }

    void OpenGLWindow::mouseReleaseEvent(QMouseEvent*)
    {
    }
}
