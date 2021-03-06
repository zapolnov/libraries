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
#include "utility/FileSystem.h"
#include "utility/streams/FileReader.h"
#include <mutex>
#include <string>
#include <memory>

namespace Z
{
    class ZipFileSystem : public FileSystem
    {
    public:
        explicit ZipFileSystem(const FileReaderPtr& zipFile);
        explicit ZipFileSystem(FileReaderPtr&& zipFile);
        ZipFileSystem(const FileReaderPtr& zipFile, const std::string& password);
        ZipFileSystem(FileReaderPtr&& zipFile, const std::string& password);
        ~ZipFileSystem();

        bool fileExists(const std::string& path) final override;
        FileReaderPtr openFile(const std::string& path) final override;

    private:
        std::mutex m_Mutex;
        FileReaderPtr m_ZipReader;
        void* m_ZipFile;
        std::unique_ptr<std::string> m_Password;

        ZipFileSystem(const ZipFileSystem&) = delete;
        ZipFileSystem& operator=(const ZipFileSystem&) = delete;
    };
}
