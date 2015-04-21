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
#include "utility/FunctionQueue.h"
#include <atomic>
#include <future>
#include <cstdint>
#include <QElapsedTimer>
#include <QThread>
#include <QGLWidget>

namespace Z
{
    class QtOpenGLWindowDelegate;

    class QtOpenGLRenderThread : public QThread, protected FunctionQueue
    {
        Q_OBJECT

    public:
        QtOpenGLRenderThread(QGLWidget* qt, QtOpenGLWindowDelegate* delegate);
        ~QtOpenGLRenderThread();

        void start(int width, int height);
        void postShutdown();

        void suspend();
        void resume();

        void resize(int width, int height);

        QtOpenGLWindowDelegate* delegate() const { return m_Delegate; }
        using FunctionQueue::post;

    protected:
        void run() override;

    private:
        QGLWidget* m_GL;
        QtOpenGLWindowDelegate* m_Delegate;
        QElapsedTimer m_Timer;
        std::atomic<uint32_t> m_ViewportSize;
        std::promise<void> m_ThreadStartPromise;
        std::atomic<bool> m_Suspended;
        std::atomic<bool> m_ShuttingDown;
    };
}
