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
#include <string>
#include <ostream>
#include <memory>

namespace Z
{
    class Utf8String
    {
    public:
        Utf8String();
        Utf8String(const Utf8String& src);
        Utf8String(Utf8String&& src);
        ~Utf8String();

        Utf8String& operator=(const Utf8String& src);
        Utf8String& operator=(Utf8String&& src);

        static Utf8String fromRawBytes(const char* str);
        static Utf8String fromRawBytes(const void* str, size_t length);
        static Utf8String fromRawBytes(const std::string& str);
        static Utf8String fromRawBytes(std::string&& str);

        const char* rawBytes() const;
        size_t size() const;

        const std::string& asStdString() const;

    private:
        struct Data;

        static std::shared_ptr<Data> m_EmptyStringData;
        std::shared_ptr<Data> m_Data;

        Utf8String(const std::shared_ptr<Data>& data);
    };

    std::ostream& operator<<(std::ostream& stream, const Utf8String& string);
}
