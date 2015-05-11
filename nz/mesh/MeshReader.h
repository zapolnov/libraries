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
#include "utility/streams/InputStream.h"
#include "utility/streams/FileReader.h"
#include "Mesh.h"
#include <memory>

namespace Z
{
    class MeshReader;
    using MeshReaderPtr = std::shared_ptr<MeshReader>;

    class MeshReader
    {
    public:
        enum ReadFlags
        {
            DontReadSkeleton    = 0b00000001,
            DontReadMaterials   = 0b00000010,
        };

        virtual ~MeshReader() = default;

        static void add(MeshReader* reader);
        static void add(const MeshReaderPtr& reader);
        static void add(MeshReaderPtr&& reader);

        static MeshPtr read(const FileReaderPtr& file, const VertexFormatPtr& format, unsigned readFlags = 0);

    protected:
        virtual bool canReadMesh(InputStream* stream) const = 0;
        virtual MeshPtr readMesh(InputStream* stream, const VertexFormatPtr& format, unsigned readFlags) const = 0;
    };
}
