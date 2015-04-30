/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include "GLResourceManager.h"
#include "GLResource.h"
#include <vector>

namespace Z
{
    GLResourceManager::GLResourceManager(const FileSystemPtr& fileSystem)
        : m_FileSystem(fileSystem)
    {
    }

    GLResourceManager::~GLResourceManager()
    {
        unloadAllResources();

        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        for (auto resource : m_Resources)
            resource->m_Manager = nullptr;
    }

    void GLResourceManager::unloadAllResources()
    {
        std::vector<GLResource*> resources;

        {
            std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
            resources.reserve(m_Resources.size());
            for (auto resource : m_Resources)
                resources.push_back(resource);
        }

        for (auto resource : resources)
            resource->unload();
    }

    void GLResourceManager::reloadAllResources()
    {
        std::vector<GLResource*> resources;

        {
            std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
            resources.reserve(m_Resources.size());
            for (auto resource : m_Resources)
                resources.push_back(resource);
        }

        for (auto resource : resources)
            resource->reload();
    }

    void GLResourceManager::onResourceCreated(GLResource* resource)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        m_Resources.insert(resource);
    }

    void GLResourceManager::onResourceDestroyed(GLResource* resource)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        m_Resources.erase(resource);
    }
}
