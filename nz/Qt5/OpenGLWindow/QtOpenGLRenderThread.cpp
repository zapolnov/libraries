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
#include "QtOpenGLWindowDelegate.h"
#include "utility/debug.h"
#include <limits>

namespace Z
{
    namespace
    {
        union ViewportSize
        {
            uint32_t packed;
            struct { int16_t width, height; } unpacked;

            void set(int width, int height)
            {
                const auto minSize = std::numeric_limits<int16_t>::min();
                const auto maxSize = std::numeric_limits<int16_t>::max();

                Z_ASSERT(width >= minSize && width <= maxSize);
                Z_ASSERT(height >= minSize && height <= maxSize);

                unpacked.width = int16_t(width < minSize ? minSize : (width > maxSize ? maxSize : width));
                unpacked.height = int16_t(height < minSize ? minSize : (height > maxSize ? maxSize : height));
            }
        };
    }

    QtOpenGLRenderThread::QtOpenGLRenderThread(QGLWidget* gl, QtOpenGLWindowDelegate* delegate)
        : m_GL(gl)
        , m_Delegate(delegate)
        , m_Suspended(false)
        , m_ShuttingDown(false)
    {
        Z_ASSERT(delegate != nullptr);
    }

    QtOpenGLRenderThread::~QtOpenGLRenderThread()
    {
    }

    void QtOpenGLRenderThread::start(int width, int height)
    {
        Z_ASSERT(!isRunning());

        m_GL->doneCurrent();
        m_GL->context()->moveToThread(this);

        ViewportSize size;
        size.set(width, height);
        m_ViewportSize.store(size.packed);

        m_ThreadStartPromise = std::promise<void>();
        std::future<void> future = m_ThreadStartPromise.get_future();
        QThread::start();
        future.wait();
    }

    void QtOpenGLRenderThread::postShutdown()
    {
        m_ShuttingDown.store(true);
    }

    void QtOpenGLRenderThread::suspend()
    {
        m_Suspended.store(true);
    }

    void QtOpenGLRenderThread::resume()
    {
        m_Suspended.store(false);
    }

    void QtOpenGLRenderThread::resize(int width, int height)
    {
        ViewportSize size;
        size.set(width, height);
        m_ViewportSize.store(size.packed);
    }

    void QtOpenGLRenderThread::run()
    {
        ViewportSize viewportSize;
        bool suspended = false;

        m_ShuttingDown.store(false);

        // Initialize the renderer
        m_GL->context()->makeCurrent();
        viewportSize.packed = m_ViewportSize.load();
        m_Delegate->initializeGL(viewportSize.unpacked.width, viewportSize.unpacked.height);
        m_GL->context()->doneCurrent();

        // Notify UI thread that we have started
        m_ThreadStartPromise.set_value();

        // Run the main loop
        m_Timer.start();
        while (!m_ShuttingDown.load(std::memory_order_relaxed))
        {
            processAll();

            // Consume less resources when suspended
            bool newSuspended = m_Suspended.load();
            if (suspended && newSuspended) {
                yieldCurrentThread();
                continue;
            }

            m_GL->context()->makeCurrent();

            // Notify delegate about suspension
            if (newSuspended != suspended) {
                if (newSuspended)
                    m_Delegate->suspendGL();
                else {
                    m_Delegate->resumeGL();
                    m_Timer.restart();
                }
                suspended = newSuspended;
            }

            if (!suspended)
            {
                // Notify delegate about viewport size changes
                uint32_t newViewportSize = m_ViewportSize.load();
                if (viewportSize.packed != newViewportSize) {
                    viewportSize.packed = newViewportSize;
                    m_Delegate->resizeGL(viewportSize.unpacked.width, viewportSize.unpacked.height);
                }

                // Run frame
                auto time = m_Timer.restart();
                m_Delegate->renderGL(time * 0.001);
                m_GL->context()->swapBuffers();
            }

            m_GL->context()->doneCurrent();
        }

        // Shutdown the engine
        m_GL->context()->makeCurrent();
        m_Delegate->shutdownGL();
        m_GL->context()->doneCurrent();
    }
}
