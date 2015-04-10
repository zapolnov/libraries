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
#include "QtRenderThread.h"
#include "utility/debug.h"
#include <new>

namespace Z
{
    QtRenderThread::QtRenderThread(QGLWidget* gl, OpenGLWindowDelegate* delegate)
        : m_GL(gl)
        , m_Delegate(delegate)
    {
        Z_ASSERT(delegate != nullptr);
    }

    QtRenderThread::~QtRenderThread()
    {
    }

    void QtRenderThread::start(int width, int height)
    {
        Z_ASSERT(!isRunning());

        m_GL->doneCurrent();
        m_GL->context()->moveToThread(this);

        m_ViewportWidth = width;
        m_ViewportHeight = height;

        m_ThreadStartPromise = std::promise<void>();
        std::future<void> future = m_ThreadStartPromise.get_future();
        QThread::start();
        future.wait();
    }

    void QtRenderThread::suspend()
    {
        bool wasSuspended = m_Suspended.exchange(true, std::memory_order_seq_cst);
        if (!wasSuspended)
        {
            std::promise<void> promise;
            std::future<void> future = promise.get_future();
            m_FunctionQueue.post([this, &promise]() {
                m_Delegate->onSuspend();
                promise.set_value();
            });
            future.wait();
        }
    }

    void QtRenderThread::resume()
    {
        bool wasSuspended = m_Suspended.exchange(false, std::memory_order_seq_cst);
        if (wasSuspended)
        {
            std::promise<void> promise;
            std::future<void> future = promise.get_future();
            m_FunctionQueue.post([this, &promise]() {
                m_Delegate->onResume();
                promise.set_value();
            });
            future.wait();
        }
    }

    void QtRenderThread::postResize(int width, int height)
    {
        m_FunctionQueue.post([this, width, height]() {
            m_ViewportResized = true;
            m_ViewportWidth = width;
            m_ViewportHeight = height;
        });
    }

    void QtRenderThread::postShutdown()
    {
        m_ShuttingDown.store(true, std::memory_order_seq_cst);
    }

    void QtRenderThread::run()
    {
        m_ShuttingDown.store(false, std::memory_order_seq_cst);
        m_ViewportResized = false;

        // Initialize the engine
        m_GL->context()->makeCurrent();
        m_Delegate->onInitialize(m_ViewportWidth, m_ViewportHeight);
        m_GL->context()->doneCurrent();

        // Notify UI thread that we have started
        m_ThreadStartPromise.set_value();

        // Run the main loop
        m_Timer.start();
        while (!m_ShuttingDown.load(std::memory_order_relaxed))
        {
            bool suspended = false;

            // Consume less resources when suspended
            if (m_Suspended.load(std::memory_order_relaxed))
            {
                suspended = true;
                yieldCurrentThread();
                m_Timer.restart();
            }

            m_GL->context()->makeCurrent();

            // Process pending events
            m_FunctionQueue.processAll();
            if (m_ViewportResized)
            {
                m_ViewportResized = false;
                m_Delegate->onResize(m_ViewportWidth, m_ViewportHeight);
            }

            // Update
            if (!suspended) {
                auto time = m_Timer.restart();
                m_Delegate->update(time);
            }

            // Render
            m_Delegate->draw();

            m_GL->context()->swapBuffers();
            m_GL->context()->doneCurrent();
        }

        // Shutdown the engine
        m_GL->context()->makeCurrent();
        m_Delegate->onShutdown();
        m_GL->context()->doneCurrent();
    }
}
