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
#include "GLProgram.h"
#include "utility/FileSystem.h"
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace Z
{
    class GLResource;

    class GLResourceManager
    {
    public:
        explicit GLResourceManager(const FileSystemPtr& fileSystem);
        virtual ~GLResourceManager();

        const FileSystemPtr& fileSystem() const { return m_FileSystem; }

        void unloadAllResources();
        void shutdown();

        void beginReloadResources();
        bool continueReloadResources(float* progress = nullptr);
        bool isReloadingResources() const { return m_ReloadingResources.load(); }

        GLProgramPtr createProgram(const std::string& vertex, const std::string& fragment);
        GLProgramPtr loadProgram(const std::string& fileName);

    protected:
        virtual void onResourceCreated(GLResource* resource);
        virtual void onResourceDestroyed(GLResource* resource);

    private:
        class Program;
        class StaticProgram;

        FileSystemPtr m_FileSystem;
        std::recursive_mutex m_Mutex;
        std::unordered_map<std::string, std::weak_ptr<Program>> m_Programs;
        std::unordered_set<GLResource*> m_Resources;
        std::vector<GLResource*> m_ReloadingResourcesList;
        size_t m_ReloadingResourceIndex = 0;
        std::atomic<bool> m_ReloadingResources;
        bool m_HaveErasedResources = false;
        bool m_Shutdown = false;

        GLResourceManager(const GLResourceManager&) = delete;
        GLResourceManager& operator=(const GLResourceManager&) = delete;

        friend class GLResource;
    };
}
