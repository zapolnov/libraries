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
#include <cstddef>
#include <cstdint>
#include <memory>

namespace Z
{
    class Image
    {
    public:
        enum Format {
            Unknown = 0,
            Luminance8,
            LuminanceAlpha8,
            RGB8,
            RGBA8,
        };

        Image();
        Image(int w, int h, Format format);
        Image(int w, int h, Format format, size_t dataSize);
        Image(int w, int h, Format format, const uint8_t* data, size_t dataSize);
        Image(int w, int h, Format format, std::unique_ptr<uint8_t[]>&& data, size_t dataSize);
        Image(Image&& src);
        ~Image();

        Image& operator=(Image&& src);

        size_t dataSize() const { return m_DataSize; }
        const uint8_t* data() const { return m_Data.get(); }
        const uint8_t* constData() const { return m_Data.get(); }
        uint8_t* data() { return m_Data.get(); }

        Format format() const { return m_Format; }
        int width() const { return m_Width; }
        int height() const { return m_Height; }

        float luminanceAt(int x, int y) const;

    private:
        std::unique_ptr<uint8_t[]> m_Data;
        size_t m_DataSize;
        Format m_Format;
        int m_Width;
        int m_Height;

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
    };

    using ImagePtr = std::shared_ptr<Image>;
}
