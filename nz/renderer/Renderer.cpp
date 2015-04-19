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
#include "Renderer.h"
#include "utility/debug.h"
#include <future>
#include <algorithm>

namespace Z
{
    Renderer::Renderer()
    {
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::init(RendererCallbacks* callbacks, int viewportWidth, int viewportHeight)
    {
        Z_ASSERT(callbacks != nullptr);
        m_Callbacks = callbacks;
        m_RenderThreadId = std::this_thread::get_id();
        m_ViewportWidth = viewportWidth;
        m_ViewportHeight = viewportHeight;
        m_Callbacks->onRendererDidFinishLaunching(this);
    }

    void Renderer::shutdown()
    {
        Z_ASSERT(isCurrentThreadARenderThread());
        m_Callbacks->onRendererWillTerminate(this);
        for (RendererResource* resource : m_AllResources)
            resource->onRendererDestroyed();
    }

    void Renderer::setViewportDimensions(int w, int h)
    {
        Z_ASSERT(isCurrentThreadARenderThread());
        if (w != m_ViewportWidth || h != m_ViewportHeight) {
            m_ViewportWidth = std::max(0, w);
            m_ViewportHeight = std::max(0, h);
            m_Callbacks->onRendererResized(this);
        }
    }

    void Renderer::suspend(bool unload)
    {
        performInRenderThreadSync([this, unload]() {
            if (m_Suspended == 0)
                m_Callbacks->onRendererWillSuspend(this);
            ++m_Suspended;
            if (unload)
                unloadResources();
        });
    }

    void Renderer::resume()
    {
        performInRenderThreadLater([this]() {
            Z_CHECK(m_Suspended > 0);
            if (m_Suspended > 0) {
                --m_Suspended;
                if (m_Suspended == 0) {
                    reloadResources();
                    m_Callbacks->onRendererDidResume(this);
                }
            }
        });
    }

    bool Renderer::isSuspended() const
    {
        return m_Suspended != 0;
    }

    void Renderer::unloadResources()
    {
        performInRenderThreadSync([this]() {
            if (!m_ResourcesUnloaded) {
                m_Callbacks->onRendererWillUnloadResources(this);
                for (RendererResource* resource : m_AllResources)
                    resource->unload();
                m_ResourcesUnloaded = true;
                m_Callbacks->onRendererDidUnloadResources(this);
            }
        });
    }

    void Renderer::reloadResources()
    {
        performInRenderThreadSync([this]() {
            if (m_ResourcesUnloaded) {
                m_Callbacks->onRendererWillReloadResources(this);
                for (RendererResource* resource : m_AllResources)
                    resource->reload();
                m_ResourcesUnloaded = false;
                m_Callbacks->onRendererDidReloadResources(this);
            }
        });
    }

    bool Renderer::isCurrentThreadARenderThread() const
    {
        return std::this_thread::get_id() == m_RenderThreadId;
    }

    void Renderer::performInRenderThreadSync(const std::function<void()>& what)
    {
        if (isCurrentThreadARenderThread())
            what();
        else {
            std::promise<void> promise;
            std::future<void> future = promise.get_future();
            m_FunctionQueue.post([what, &promise]() {
                what();
                promise.set_value();
            });
            future.wait();
        }
    }

    void Renderer::performInRenderThreadAsync(const std::function<void()>& what)
    {
        if (isCurrentThreadARenderThread())
            what();
        else
            m_FunctionQueue.post(what);
    }

    void Renderer::performInRenderThreadAsync(std::function<void()>&& what)
    {
        if (isCurrentThreadARenderThread())
            what();
        else
            m_FunctionQueue.post(std::move(what));
    }

    void Renderer::performInRenderThreadLater(const std::function<void()>& what)
    {
        m_FunctionQueue.post(what);
    }

    void Renderer::performInRenderThreadLater(std::function<void()>&& what)
    {
        m_FunctionQueue.post(std::move(what));
    }

    void Renderer::runFrame(double time)
    {
        Z_ASSERT(isCurrentThreadARenderThread());

        m_FunctionQueue.processAll();

        if (!m_Suspended)
            m_Callbacks->update(this, time);

        m_Callbacks->draw(this);
    }

    void Renderer::onResourceCreated(RendererResource* resource)
    {
        Z_ASSERT(isCurrentThreadARenderThread());

        resource->m_Renderer = this;
        m_AllResources.insert(resource);
    }

    void Renderer::onResourceDestroyed(RendererResource* resource)
    {
        performInRenderThreadSync([this, resource]() {
            m_AllResources.erase(resource);
        });
    }
}
