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
#include "Utf8String.h"
#include <utility>

namespace Z
{
    struct Utf8String::Data
    {
        std::string text;

        Data() {}
        Data(const std::string& str) : text(str) {}
        Data(std::string&& str) : text(std::move(str)) {}
        Data(const char* str) : text(str) {}
        Data(const void* str, size_t length) : text(reinterpret_cast<const char*>(str), length) {}
    };

    std::shared_ptr<Utf8String::Data> Utf8String::m_EmptyStringData = std::make_shared<Utf8String::Data>();

    Utf8String::Utf8String()
        : m_Data(m_EmptyStringData)
    {
    }

    Utf8String::Utf8String(const Utf8String& src)
        : m_Data(src.m_Data)
    {
    }

    Utf8String::Utf8String(Utf8String&& src)
        : m_Data(std::move(src.m_Data))
    {
    }

    Utf8String::Utf8String(const std::shared_ptr<Data>& data)
        : m_Data(data)
    {
    }

    Utf8String::~Utf8String()
    {
    }

    Utf8String& Utf8String::operator=(const Utf8String& src)
    {
        m_Data = src.m_Data;
        return *this;
    }

    Utf8String& Utf8String::operator=(Utf8String&& src)
    {
        m_Data = std::move(src.m_Data);
        return *this;
    }

    Utf8String Utf8String::fromRawBytes(const char* str)
    {
        if (!str || !*str)
            return m_EmptyStringData;
        return std::make_shared<Data>(str);
    }

    Utf8String Utf8String::fromRawBytes(const void* str, size_t length)
    {
        if (!str || !length)
            return m_EmptyStringData;
        return std::make_shared<Data>(str, length);
    }

    Utf8String Utf8String::fromRawBytes(const std::string& str)
    {
        if (str.empty())
            return m_EmptyStringData;
        return std::make_shared<Data>(str);
    }

    Utf8String Utf8String::fromRawBytes(std::string&& str)
    {
        if (str.empty())
            return m_EmptyStringData;
        return std::make_shared<Data>(std::move(str));
    }

    const char* Utf8String::rawBytes() const
    {
        return m_Data->text.c_str();
    }

    size_t Utf8String::size() const
    {
        return m_Data->text.length();
    }

    const std::string& Utf8String::asStdString() const
    {
        return m_Data->text;
    }

    bool Utf8String::operator==(const Utf8String& other) const
    {
        return m_Data == other.m_Data || m_Data->text == other.m_Data->text;
    }

    bool Utf8String::operator!=(const Utf8String& other) const
    {
        return m_Data != other.m_Data && m_Data->text != other.m_Data->text;
    }

    bool Utf8String::operator<(const Utf8String& other) const
    {
        return m_Data != other.m_Data && m_Data->text < other.m_Data->text;
    }

    std::ostream& operator<<(std::ostream& stream, const Utf8String& string)
    {
        stream.write(string.rawBytes(), std::streamsize(string.size()));
        return stream;
    }
}
