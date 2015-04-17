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
#include "Image.h"
#include "utility/debug.h"
#include <cstring>
#include <algorithm>

namespace Z
{
    Image::Image()
        : m_DataSize(0)
        , m_Format(Format::Unknown)
        , m_Width(0)
        , m_Height(0)
    {
    }

    Image::Image(int w, int h, Format format)
        : m_DataSize(0)
        , m_Format(format)
        , m_Width(std::max(w, 0))
        , m_Height(std::max(h, 0))
    {
        switch (m_Format)
        {
        case Unknown: Z_ASSERT(m_Format != Unknown); break;
        case Luminance8: m_DataSize = m_Width * m_Height; break;
        case LuminanceAlpha8: m_DataSize = m_Width * m_Height * 2; break;
        case RGB8: m_DataSize = m_Width * m_Height * 3; break;
        case RGBA8: m_DataSize = m_Width * m_Height * 4; break;
        }

        if (m_DataSize > 0)
            m_Data.reset(new uint8_t[m_DataSize]);
    }

    Image::Image(int w, int h, Format format, size_t dataSize)
        : m_DataSize(dataSize)
        , m_Format(format)
        , m_Width(std::max(w, 0))
        , m_Height(std::max(h, 0))
    {
      #if Z_ASSERTIONS_ENABLED
        switch (m_Format)
        {
        case Unknown: break;
        case Luminance8: Z_CHECK(dataSize == m_Width * m_Height); break;
        case LuminanceAlpha8: Z_CHECK(m_DataSize == m_Width * m_Height * 2); break;
        case RGB8: Z_CHECK(m_DataSize == m_Width * m_Height * 3); break;
        case RGBA8: Z_CHECK(m_DataSize == m_Width * m_Height * 4); break;
        }
      #endif

        if (m_DataSize > 0)
            m_Data.reset(new uint8_t[m_DataSize]);
    }

    Image::Image(int w, int h, Format format, const uint8_t* data, size_t dataSize)
        : m_DataSize(dataSize)
        , m_Format(format)
        , m_Width(std::max(w, 0))
        , m_Height(std::max(h, 0))
    {
      #if Z_ASSERTIONS_ENABLED
        switch (m_Format)
        {
        case Unknown: break;
        case Luminance8: Z_CHECK(dataSize == m_Width * m_Height); break;
        case LuminanceAlpha8: Z_CHECK(m_DataSize == m_Width * m_Height * 2); break;
        case RGB8: Z_CHECK(m_DataSize == m_Width * m_Height * 3); break;
        case RGBA8: Z_CHECK(m_DataSize == m_Width * m_Height * 4); break;
        }
      #endif

        if (m_DataSize > 0) {
            m_Data.reset(new uint8_t[m_DataSize]);
            memcpy(m_Data.get(), data, m_DataSize);
        }
    }

    Image::Image(int w, int h, Format format, std::unique_ptr<uint8_t[]>&& data, size_t dataSize)
        : m_Data(std::move(data))
        , m_DataSize(dataSize)
        , m_Format(format)
        , m_Width(std::max(w, 0))
        , m_Height(std::max(h, 0))
    {
    }

    Image::Image(Image&& src)
        : m_Data(std::move(src.m_Data))
        , m_DataSize(src.m_DataSize)
        , m_Format(src.m_Format)
        , m_Width(src.m_Width)
        , m_Height(src.m_Height)
    {
        src.m_DataSize = 0;
        src.m_Format = Unknown;
        src.m_Width = 0;
        src.m_Height = 0;
    }

    Image::~Image()
    {
    }

    Image& Image::operator=(Image&& src)
    {
        m_Data = std::move(src.m_Data);
        m_DataSize = src.m_DataSize;
        m_Format = src.m_Format;
        m_Width = src.m_Width;
        m_Height = src.m_Height;
        src.m_DataSize = 0;
        src.m_Format = Unknown;
        src.m_Width = 0;
        src.m_Height = 0;
        return *this;
    }
}
