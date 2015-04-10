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
#include "../OpenGLWindowDelegate.h"
#include <atomic>
#include <future>
#include <QElapsedTimer>
#include <QThread>
#include <QGLWidget>

namespace Z
{
    class QtRenderThread : public QThread
    {
    public:
        QtRenderThread(QGLWidget* qt, OpenGLWindowDelegate* delegate);
        ~QtRenderThread();

        void start(int width, int height);
        void suspend();
        void resume();

        void postResize(int width, int height);
        void postShutdown();

    protected:
        void run() override;

    private:
        QGLWidget* m_GL;
        OpenGLWindowDelegate* m_Delegate;
        Z::FunctionQueue m_FunctionQueue;
        QElapsedTimer m_Timer;
        int m_ViewportWidth = 0;
        int m_ViewportHeight = 0;
        bool m_ViewportResized = false;
        std::promise<void> m_ThreadStartPromise;
        std::atomic<bool> m_Suspended;
        std::atomic<bool> m_ShuttingDown;
    };
}
