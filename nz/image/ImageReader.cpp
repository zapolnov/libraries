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
#include "ImageReader.h"
#include "utility/streams/FileInputStream.h"
#include "utility/debug.h"
#include <mutex>
#include <vector>
#include <memory>

namespace Z
{
    static std::mutex g_Mutex;
    static std::vector<ImageReaderPtr> g_Readers;

    void ImageReader::add(ImageReader* reader)
    {
        ImageReaderPtr ptr(reader);
        add(std::move(ptr));
    }

    void ImageReader::add(const ImageReaderPtr& reader)
    {
        Z_CHECK(reader != nullptr);
        if (!reader)
            return;

        std::lock_guard<decltype(g_Mutex)> lock(g_Mutex);
        g_Readers.emplace_back(reader);
    }

    void ImageReader::add(ImageReaderPtr&& reader)
    {
        Z_CHECK(reader != nullptr);
        if (!reader)
            return;

        std::lock_guard<decltype(g_Mutex)> lock(g_Mutex);
        g_Readers.emplace_back(std::move(reader));
    }

    ImagePtr ImageReader::read(const FileReaderPtr& file)
    {
        Z_CHECK(file != nullptr);
        if (!file)
            return nullptr;

        std::vector<ImageReaderPtr> readers;
        {
            std::lock_guard<decltype(g_Mutex)> lock(g_Mutex);
            readers = g_Readers;
        }

        for (const auto& reader : readers) {
            FileInputStream stream(file);
            if (reader->canReadImage(&stream)) {
                ImagePtr image = reader->readImage(&stream);
                if (image)
                    return image;
            }
        }

        Z_LOG("There is no reader able to load image \"" << file->name() << "\".");
        return nullptr;
    }
}
