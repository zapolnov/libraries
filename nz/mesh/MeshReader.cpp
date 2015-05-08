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
#include "MeshReader.h"
#include "utility/streams/FileInputStream.h"
#include "utility/debug.h"
#include <mutex>
#include <vector>
#include <memory>

namespace Z
{
    static std::mutex g_Mutex;
    static std::vector<MeshReaderPtr> g_Readers;

    void MeshReader::add(MeshReader* reader)
    {
        MeshReaderPtr ptr(reader);
        add(std::move(ptr));
    }

    void MeshReader::add(const MeshReaderPtr& reader)
    {
        Z_CHECK(reader != nullptr);
        if (!reader)
            return;

        std::lock_guard<decltype(g_Mutex)> lock(g_Mutex);
        g_Readers.emplace_back(reader);
    }

    void MeshReader::add(MeshReaderPtr&& reader)
    {
        Z_CHECK(reader != nullptr);
        if (!reader)
            return;

        std::lock_guard<decltype(g_Mutex)> lock(g_Mutex);
        g_Readers.emplace_back(std::move(reader));
    }

    MeshPtr MeshReader::read(const FileReaderPtr& file, const VertexFormatPtr& format, unsigned readFlags)
    {
        Z_CHECK(file != nullptr);
        if (!file)
            return nullptr;

        std::vector<MeshReaderPtr> readers;
        {
            std::lock_guard<decltype(g_Mutex)> lock(g_Mutex);
            readers = g_Readers;
        }

        for (const auto& reader : readers) {
            FileInputStream stream(file);
            if (!reader->canReadMesh(&stream))
                continue;

            FileInputStream stream2(file);
            MeshPtr mesh = reader->readMesh(&stream2, format, readFlags);
            if (mesh) {
                const std::string& fileName = file->name();
                size_t index = fileName.rfind('/');
                mesh->setBaseDirectory(index == std::string::npos ? std::string() : fileName.substr(0, index + 1));
                return mesh;
            }
        }

        Z_LOG("There is no reader able to load mesh \"" << file->name() << "\".");
        return nullptr;
    }
}
