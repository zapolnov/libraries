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
#include "QtOpenGLRenderThread.h"
#include "../../impl/gl1/GL1Renderer.h"
#include "utility/debug.h"
#include <new>

namespace Z
{
    QtOpenGLRenderThread::QtOpenGLRenderThread(QGLWidget* gl, RendererCallbacks* callbacks)
        : m_GL(gl)
        , m_Callbacks(callbacks)
    {
        Z_ASSERT(callbacks != nullptr);
    }

    QtOpenGLRenderThread::~QtOpenGLRenderThread()
    {
    }

    void QtOpenGLRenderThread::start(int width, int height)
    {
        Z_ASSERT(!isRunning());

        m_GL->doneCurrent();
        m_GL->context()->moveToThread(this);

        m_InitialViewportWidth = width;
        m_InitialViewportHeight = height;

        m_ThreadStartPromise = std::promise<void>();
        std::future<void> future = m_ThreadStartPromise.get_future();
        QThread::start();
        future.wait();
    }

    void QtOpenGLRenderThread::postShutdown()
    {
        m_ShuttingDown.store(true, std::memory_order_seq_cst);
    }

    void QtOpenGLRenderThread::suspend()
    {
        bool wasSuspended = m_Suspended.exchange(true, std::memory_order_seq_cst);
        if (!wasSuspended)
            m_Renderer->suspend(false);
    }

    void QtOpenGLRenderThread::resume()
    {
        bool wasSuspended = m_Suspended.exchange(false, std::memory_order_seq_cst);
        if (wasSuspended)
            m_Renderer->resume();
    }

    void QtOpenGLRenderThread::run()
    {
        m_ShuttingDown.store(false, std::memory_order_seq_cst);

        // Initialize the renderer
        m_GL->context()->makeCurrent();
        m_Renderer = new GL1Renderer();
        m_Renderer->init(m_Callbacks, m_InitialViewportWidth, m_InitialViewportHeight);
        m_GL->context()->doneCurrent();

        // Notify UI thread that we have started
        m_ThreadStartPromise.set_value();

        // Run the main loop
        m_Timer.start();
        while (!m_ShuttingDown.load(std::memory_order_relaxed)) {
            bool suspended = false;

            // Consume less resources when suspended
            if (m_Renderer->isSuspended()) {
                suspended = true;
                yieldCurrentThread();
                m_Timer.restart();
            }

            m_GL->context()->makeCurrent();

            // Run frame
            auto time = m_Timer.restart();
            m_Renderer->runFrame(time);

            m_GL->context()->swapBuffers();
            m_GL->context()->doneCurrent();
        }

        // Shutdown the engine
        m_GL->context()->makeCurrent();
        m_Renderer->shutdown();
        delete m_Renderer;
        m_GL->context()->doneCurrent();
    }
}
