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

#pragma once
#include "utility/FileSystem.h"
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace Z
{
    class GLResource;

    class GLResourceManager
    {
    public:
        explicit GLResourceManager(const FileSystemPtr& fileSystem);
        virtual ~GLResourceManager();

        void unloadAllResources();
        void shutdown();

        void beginReloadResources();
        bool continueReloadResources(float* progress = nullptr);
        bool isReloadingResources() const { return m_ReloadingResources.load(); }

    protected:
        virtual void onResourceCreated(GLResource* resource);
        virtual void onResourceDestroyed(GLResource* resource);

    private:
        FileSystemPtr m_FileSystem;
        std::mutex m_Mutex;
        std::unordered_set<GLResource*> m_Resources;
        std::vector<GLResource*> m_ReloadingResourcesList;
        size_t m_ReloadingResourceIndex = 0;
        std::atomic<bool> m_ReloadingResources;
        bool m_HaveErasedResources = false;
        bool m_Shutdown = false;

        GLResourceManager(const GLResourceManager&);
        GLResourceManager& operator=(const GLResourceManager&);

        friend class GLResource;
    };
}
