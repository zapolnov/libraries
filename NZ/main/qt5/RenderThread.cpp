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
#include "RenderThread.h"
//#include "PlatformAPI.h"
//#include "core/Engine.h"
#include "utility/debug.h"
#include <new>
#include <future>

namespace Z
{
	RenderThread* RenderThread::m_Instance;

	RenderThread::RenderThread(QGLWidget* gl)
		: m_GL(gl)
	{
		Z_ASSERT(RenderThread::m_Instance == nullptr);
		m_Instance = this;
	}

	RenderThread::~RenderThread()
	{
		Z_ASSERT(RenderThread::m_Instance == this);
		m_Instance = nullptr;
	}

	void RenderThread::start()
	{
		m_GL->doneCurrent();
		m_GL->context()->moveToThread(this);
		QThread::start();
	}

	void RenderThread::suspend()
	{
		bool wasSuspended = m_Suspended.exchange(true, std::memory_order_seq_cst);
		if (!wasSuspended)
		{
			std::promise<void> promise;
			std::future<void> future = promise.get_future();
			m_FunctionQueue.post([this, &promise]() {
//				m_Engine->onSuspend();
				promise.set_value();
			});
			future.wait();
		}
	}

	void RenderThread::resume()
	{
		bool wasSuspended = m_Suspended.exchange(false, std::memory_order_seq_cst);
		if (wasSuspended)
		{
			std::promise<void> promise;
			std::future<void> future = promise.get_future();
			m_FunctionQueue.post([this, &promise]() {
//				m_Engine->onResume();
				promise.set_value();
			});
			future.wait();
		}
	}

	void RenderThread::postResize(int width, int height)
	{
		m_FunctionQueue.post([this, width, height]() {
			m_ViewportResized = true;
			m_ViewportWidth = width;
			m_ViewportHeight = height;
		});
	}

	void RenderThread::postShutdown()
	{
		m_ShuttingDown.store(true, std::memory_order_seq_cst);
	}

	void RenderThread::run()
	{
//		void* engineInstance[(sizeof(Z::Engine) + sizeof(void*) - 1) / sizeof(void*)];
//		void* platformAPIInstance[(sizeof(ZQt::PlatformAPI) + sizeof(void*) - 1) / sizeof(void*)];

		m_ShuttingDown.store(false, std::memory_order_seq_cst);

		// Initialize the engine
		m_GL->context()->makeCurrent();
//		PlatformAPI* api = new(platformAPIInstance) ZQt::PlatformAPI;
//		new(engineInstance) Z::Engine(*api, &m_Engine);
		m_GL->context()->doneCurrent();

		// Run the main loop
		m_Timer.start();
		while (!m_ShuttingDown.load(std::memory_order_relaxed))
		{
			// Consume less resources when suspended
			if (m_Suspended.load(std::memory_order_relaxed))
			{
				yieldCurrentThread();
				m_Timer.restart();
			}

			m_GL->context()->makeCurrent();

			// Process pending events
			m_FunctionQueue.processAll();
			if (m_ViewportResized)
			{
				m_ViewportResized = false;
//				m_Engine->onViewportResized(m_ViewportWidth, m_ViewportHeight);
			}

			// Render the frame
			auto time = m_Timer.restart();
//			m_Engine->onFrameRequested(double(time) * 0.001);

			m_GL->context()->swapBuffers();
			m_GL->context()->doneCurrent();
		}

		// Shutdown the engine
		m_GL->context()->makeCurrent();
//		m_Engine->~PlatformCallbacks();
//		m_Engine = nullptr;
//		api->~PlatformAPI();
		m_GL->context()->doneCurrent();
	}
}
