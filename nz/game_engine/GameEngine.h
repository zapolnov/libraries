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
#include "renderer/Renderer.h"
#include "renderer/RendererCallbacks.h"
#include "filesystem/FileSystem.h"
#include "SceneElement.h"
#include "ResourceManager.h"
#include <memory>

namespace Z
{
    class GameEngine : public RendererCallbacks
    {
    public:
        GameEngine();
        ~GameEngine();

        Renderer* renderer() const { return m_Renderer; }

        ResourceManager* resourceManager() const { return m_ResourceManager.get(); }

        FileSystem* fileSystem() const { return m_FileSystem.get(); }
        void setFileSystem(FileSystem* fileSystem);

    protected:
        void onRendererDidFinishLaunching(Renderer*) override;
        void onRendererWillTerminate(Renderer*) override;

        void onRendererWillSuspend(Renderer*) override;
        void onRendererDidResume(Renderer*) override;

        void onRendererWillUnloadResources(Renderer*) override;
        void onRendererDidUnloadResources(Renderer*) override;
        void onRendererWillReloadResources(Renderer*) override;
        void onRendererDidReloadResources(Renderer*) override;

        void onRendererResized(Renderer*) override;

        void update(Renderer*, double time) override;
        void draw(Renderer*) override;

    private:
        Renderer* m_Renderer = nullptr;
        std::unique_ptr<ResourceManager> m_ResourceManager;
        std::unique_ptr<FileSystem> m_FileSystem;
    };
}
