/*
 * Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include "bin2c.h"
#include <zlib.h>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <vector>

bool bin2c(std::ostream& output, const char* ident, const void* data, size_t length, bool compress)
{
    std::vector<char> buf;
    size_t uncompressedSize = 0;
    if (compress)
    {
        uLong packedSize = (uLong)length;
        buf.resize(length);
        if (compress2((Bytef*)&buf[0], &packedSize, (const Bytef*)data, (uLong)length, 9) != 0)
            return false;
        uncompressedSize = length;
        data = &buf[0];
        length = packedSize;
    }

    output << "/* THIS IS AN AUTOMATICALLY GENERATED FILE. DO NOT MODIFY! */\n";
    output << "#include <stddef.h>\n";
    output << "extern const size_t " << ident << "_size;\n";
    if (compress)
        output << "extern const size_t " << ident << "_uncompressed_size;\n";
    output << "extern const unsigned char " << ident << "[];\n";
    output << "const size_t " << ident << "_size = " << length << ";\n";
    if (compress)
        output << "const size_t " << ident << "_uncompressed_size = " << uncompressedSize << ";\n";
    output << "const unsigned char " << ident << "[] = {";

    const unsigned char* p = (const unsigned char*)data;
    for (size_t i = 0; i <= length; i++)
    {
        if (i % 16 == 0)
            output << "\n\t";

        char tmp[16];
        sprintf(tmp, "0x%02x,", i == length ? 0 : p[i]);
        output << tmp;
    }

    output << "\n};\n";

    return true;
}
