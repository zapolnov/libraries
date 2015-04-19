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
#include "RendererCallbacks.h"
#include "RendererTexture.h"
#include "RendererResource.h"
#include "utility/FunctionQueue.h"
#include <mutex>
#include <unordered_set>
#include <memory>
#include <functional>
#include <thread>
#include <glm/glm.hpp>

namespace Z
{
    class Renderer
    {
    public:
        enum {
            ColorBufferBit   = 0b00001,
            DepthBufferBit   = 0b00010,
            StencilBufferBit = 0b00100,
        };

        Renderer();
        virtual ~Renderer();

        void init(RendererCallbacks* callbacks, int viewportWidth, int viewportHeight);
        void shutdown();

        int viewportWidth() const { return m_ViewportWidth; }
        int viewportHeight() const { return m_ViewportHeight; }
        void setViewportDimensions(int w, int h);

        void suspend(bool unload);
        void resume();
        bool isSuspended() const;

        void unloadResources();
        void reloadResources();

        bool isCurrentThreadARenderThread() const;
        void performInRenderThreadSync(const std::function<void()>& what);
        void performInRenderThreadAsync(const std::function<void()>& what);
        void performInRenderThreadAsync(std::function<void()>&& what);
        void performInRenderThreadLater(const std::function<void()>& what);
        void performInRenderThreadLater(std::function<void()>&& what);

        void runFrame(double time);

        virtual void setClearColor(const glm::vec4& color) = 0;
        virtual void clearBuffers(int buffers) = 0;

        virtual RendererTexturePtr createTexture() = 0;

    protected:
        template <class T, class... ARGS> std::shared_ptr<T> createResource(ARGS... args)
        {
            std::shared_ptr<T> resource = std::make_shared<T>(std::forward(args)...);
            onResourceCreated(static_cast<RendererResource*>(resource.get()));
            return resource;
        }

    private:
        std::thread::id m_RenderThreadId;
        std::unordered_set<RendererResource*> m_AllResources;
        Z::FunctionQueue m_FunctionQueue;
        RendererCallbacks* m_Callbacks = nullptr;
        bool m_ResourcesUnloaded = false;
        volatile int m_ViewportWidth = 0;
        volatile int m_ViewportHeight = 0;
        volatile int m_Suspended = 0;

        void onResourceCreated(RendererResource* resource);
        void onResourceDestroyed(RendererResource* resource);

        friend class RendererResource;
    };
}
