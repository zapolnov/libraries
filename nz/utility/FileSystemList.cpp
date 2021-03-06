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
#include "FileSystemList.h"
#include "debug.h"
#include <mutex>
#include <vector>

namespace Z
{
    FileSystemList::FileSystemList()
    {
    }

    FileSystemList::~FileSystemList()
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        m_CachedFileSystems.reset();
        m_FileSystems.clear();
    }

    bool FileSystemList::fileExists(const std::string& path)
    {
        auto fileSystems = cachedFileSystems();
        for (auto fileSystem : *fileSystems) {
            if (fileSystem->fileExists(path))
                return true;
        }
        return false;
    }

    FileReaderPtr FileSystemList::openFile(const std::string& path)
    {
        auto fileSystems = cachedFileSystems();
        for (auto fileSystem : *fileSystems) {
            if (!fileSystem->fileExists(path))
                continue;
            FileReaderPtr reader = fileSystem->openFile(path);
            if (reader)
                return reader;
        }

        Z_LOG("File not found: \"" << path << "\".");
        return nullptr;
    }

    void FileSystemList::add(FileSystem* fileSystem)
    {
        FileSystemPtr ptr(fileSystem);
        add(std::move(ptr));
    }

    void FileSystemList::add(const FileSystemPtr& fileSystem)
    {
        Z_CHECK(fileSystem != nullptr);
        if (!fileSystem)
            return;

        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        m_FileSystems.emplace_back(fileSystem);
        invalidateCache();
    }

    void FileSystemList::add(FileSystemPtr&& fileSystem)
    {
        Z_CHECK(fileSystem != nullptr);
        if (!fileSystem)
            return;

        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        m_FileSystems.emplace_back(std::move(fileSystem));
        invalidateCache();
    }

    std::shared_ptr<FileSystemList::Array> FileSystemList::cachedFileSystems()
    {
        std::shared_ptr<Array> fileSystems = m_CachedFileSystems;
        if (!fileSystems) {
            std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
            fileSystems.reset(new Array(m_FileSystems));
            m_CachedFileSystems = fileSystems;
        }
        return fileSystems;
    }

    void FileSystemList::invalidateCache()
    {
        m_CachedFileSystems.reset();
    }
}
