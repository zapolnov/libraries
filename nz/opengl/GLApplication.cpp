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
#include "GLApplication.h"
#include "utility/streams/StaticMemoryFile.h"
#include "zipfs/ZipFileSystem.h"
#include "opengl.h"

extern const size_t z_shaders_zip_size;
extern const unsigned char z_shaders_zip[];

namespace Z
{
    GLApplication::GLApplication()
        : m_FileSystemList(std::make_shared<FileSystemList>())
        , m_ResourceManager(m_FileSystemList)
    {
        FileReaderPtr reader =
            std::make_shared<StaticMemoryFile>(z_shaders_zip, z_shaders_zip_size, "builtin:shaders.zip");
        m_FileSystemList->add(new ZipFileSystem(std::move(reader)));
    }

    void GLApplication::initializeGL(int width, int height)
    {
        gl::InitWrappers();
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        onApplicationDidFinishLaunching();
    }

    void GLApplication::shutdownGL()
    {
        onApplicationWillTerminate();
        m_ResourceManager.shutdown();
    }

    void GLApplication::suspendGL()
    {
        onApplicationWillEnterBackground();

        m_Suspended = true;
        cancelAllTouches();

        m_ResourceManager.unloadAllResources();
    }

    void GLApplication::resumeGL()
    {
        m_Suspended = false;

        onApplicationDidEnterForeground();
        onApplicationWillReloadResources();

        m_ResourceManager.beginReloadResources();
        m_ReloadingResources = true;

        drawResourceReloadScreen(0.0, 0.0f);

        cancelAllTouches();
    }

    void GLApplication::resizeGL(int width, int height)
    {
        if (m_ViewportWidth != width || m_ViewportHeight != height) {
            cancelAllTouches();
            m_ViewportWidth = width;
            m_ViewportHeight = height;
            onApplicationViewportChanged();
        }
    }

    void GLApplication::renderGL(double time)
    {
        if (m_ReloadingResources) {
            if (m_ResourceManager.isReloadingResources()) {
                float progress = 0.0f;
                m_ResourceManager.continueReloadResources(&progress);
                drawResourceReloadScreen(time, progress);
                return;
            }

            m_ReloadingResources = false;
            cancelAllTouches();
        }

        runFrame(time);
    }

    void GLApplication::onPointerPressed(int id, float x, float y)
    {
        if (m_Suspended || m_ReloadingResources)
            return;

        auto r = m_PressedPointers.insert(std::make_pair(id, std::make_pair(x, y)));
        if (!r.second) {
            onTouchCancelled(r.first->first, r.first->second.first, r.first->second.second);
            r.first->second.first = x;
            r.first->second.second = y;
        }

        onTouchBegan(id, x, y);
    }

    void GLApplication::onPointerMoved(int id, float x, float y)
    {
        if (m_Suspended || m_ReloadingResources)
            return;

        auto it = m_PressedPointers.find(id);
        if (it != m_PressedPointers.end()) {
            it->second.first = x;
            it->second.second = y;
            onTouchMoved(id, x, y);
        }
    }

    void GLApplication::onPointerReleased(int id, float x, float y)
    {
        if (m_Suspended || m_ReloadingResources)
            return;

        auto it = m_PressedPointers.find(id);
        if (it != m_PressedPointers.end()) {
            m_PressedPointers.erase(it);
            onTouchEnded(id, x, y);
        }
    }

    void GLApplication::onPointerCancelled(int id, float x, float y)
    {
        if (m_Suspended || m_ReloadingResources)
            return;

        auto it = m_PressedPointers.find(id);
        if (it != m_PressedPointers.end()) {
            m_PressedPointers.erase(it);
            onTouchCancelled(id, x, y);
        }
    }

    void GLApplication::cancelAllTouches()
    {
        while (!m_PressedPointers.empty()) {
            auto it = m_PressedPointers.begin();
            int id = it->first;
            float x = it->second.first;
            float y = it->second.second;
            m_PressedPointers.erase(it);
            onTouchCancelled(id, x, y);
        }
    }

    void GLApplication::drawResourceReloadScreen(double time, float progress)
    {
        (void)time;
        (void)progress;
    }
}
