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
#include "GameEngine.h"
#include "filesystem/FileSystemList.h"

namespace Z
{
    GameEngine::GameEngine()
    {
        m_FileSystem.reset(new FileSystemList(FileSystemList::Native));
        m_ResourceManager.reset(new ResourceManager);
    }

    GameEngine::~GameEngine()
    {
    }

    void GameEngine::setFileSystem(FileSystem* fileSystem)
    {
        std::unique_ptr<FileSystem> newFileSystem(fileSystem);
        m_FileSystem = std::move(newFileSystem);
    }

    void GameEngine::onRendererDidFinishLaunching(Renderer* renderer)
    {
        m_Renderer = renderer;
        m_Renderer->setClearColor({ 0.3f, 0.5f, 0.7f, 1.0f });
    }

    void GameEngine::onRendererWillTerminate(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererWillSuspend(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererDidResume(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererWillUnloadResources(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererDidUnloadResources(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererWillReloadResources(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererDidReloadResources(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::onRendererResized(Renderer* renderer)
    {
        m_Renderer = renderer;
    }

    void GameEngine::update(Renderer* renderer, double time)
    {
        m_Renderer = renderer;
        (void)time;
    }

    void GameEngine::draw(Renderer* renderer)
    {
        m_Renderer = renderer;
        renderer->clearBuffers(Renderer::ColorBufferBit | Renderer::DepthBufferBit | Renderer::StencilBufferBit);
    }
}
