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
#include "AssImpMeshReader.h"
#include "utility/debug.h"
#include <assimp/Importer.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/postprocess.h>
#include <png.h>
#include <cstring>

namespace Z
{
    namespace
    {
      #if Z_LOGGING_ENABLED
        class AssImpLogStream : public Assimp::LogStream
        {
        public:
            void write(const char* message) override
            {
                size_t length = strlen(message);
                if (length > 0 && message[length - 1] == '\n')
                    --length;
                Z_LOG("ASSIMP: " << std::string(message, length));
            }
        };
      #endif

        class AssImpIOStream : public Assimp::IOStream
        {
        public:
            size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override
            {
                return 0;
            }

            aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
            {
                return aiReturn_FAILURE;
            }

            size_t Tell() const override
            {
                return 0;
            }

            size_t FileSize() const override
            {
                return 0;
            }

            size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override
            {
                Z_ASSERT(false);
                (void)pvBuffer;
                (void)pSize;
                (void)pCount;
                return 0;
            }

            void Flush() override
            {
                Z_ASSERT(false);
            }
        };

        class AssImpIOSystem : public Assimp::IOSystem
        {
        public:
            char getOsSeparator() const override
            {
                return '/';
            }

            bool Exists(const char* pFile) const override
            {
                return false;
            }

            Assimp::IOStream* Open(const char* pFile, const char* pMode = "rb") override
            {
                (void)pFile;
                (void)pMode;
                return nullptr;
            }

            void Close(Assimp::IOStream* pFile) override
            {
                delete pFile;
            }
        };
    }


  #if Z_LOGGING_ENABLED
    static std::once_flag g_InitOnce;
  #endif

    AssImpMeshReader::AssImpMeshReader()
    {
      #if Z_LOGGING_ENABLED
        std::call_once(g_InitOnce, []() {
            auto severity = (Z_ASSERTIONS_ENABLED ? Assimp::Logger::VERBOSE : Assimp::Logger::NORMAL);
            Assimp::DefaultLogger::create(nullptr, severity, 0, nullptr);

            const unsigned int levels =
                Assimp::Logger::Debugging |
                Assimp::Logger::Info |
                Assimp::Logger::Err |
                Assimp::Logger::Warn;
            Assimp::DefaultLogger::get()->attachStream(new AssImpLogStream, levels);
        });
      #endif
    }

    bool AssImpMeshReader::canReadMesh(InputStream* stream) const
    {
        if (!stream)
            return false;

        // FIXME
        return true;
    }

    MeshPtr AssImpMeshReader::readMesh(InputStream* stream) const
    {
        if (!stream)
            return nullptr;

        const aiScene* scene = nullptr;
        const unsigned flags =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs;

        Assimp::Importer importer;

//        FileReader* inputFile = stream->associatedFile();
//        if (!inputFile) {
        {
            std::vector<char> buffer = stream->readAll();
            scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), flags);
        }
//        } else {
//            importer.SetIOHandler(new AssImpIOSystem);
//        }

        return nullptr;
    }
}
