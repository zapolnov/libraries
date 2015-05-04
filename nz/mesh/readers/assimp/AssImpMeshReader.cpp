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
#include <assimp/scene.h>
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


    static inline void appendVec2(std::vector<glm::vec2>& out, const aiVector3D& in)
    {
        out.emplace_back(in.x, in.y);
    }

    static inline void appendVec3(std::vector<glm::vec3>& out, const aiVector3D& in)
    {
        out.emplace_back(in.x, in.y, in.z);
    }

    static inline void convertMatrix(glm::mat4& out, const aiMatrix4x4& in)
    {
        out[0][0] = in.a1; out[1][0] = in.a2; out[2][0] = in.a3; out[3][0] = in.a4;
        out[0][1] = in.b1; out[1][1] = in.b2; out[2][1] = in.b3; out[3][1] = in.b4;
        out[0][2] = in.c1; out[1][2] = in.c2; out[2][2] = in.c3; out[3][2] = in.c4;
        out[0][3] = in.d1; out[1][3] = in.d2; out[2][3] = in.d3; out[3][3] = in.d4;
    }


    static void readNodeHierarchy(SkeletonPtr& skeleton, const aiNode* rootNode, size_t parentBoneIndex)
    {
        std::string nodeName(rootNode->mName.data, rootNode->mName.length);
        Skeleton::Bone& bone = skeleton->getOrAddBone(nodeName);

        Z_CHECK(bone.index != parentBoneIndex);
        Z_CHECK(bone.parentIndex == size_t(-1) || bone.parentIndex == parentBoneIndex);
        if (bone.index != parentBoneIndex)
            bone.parentIndex = parentBoneIndex;

        for (int i = 0; i < rootNode->mNumChildren; i++)
            readNodeHierarchy(skeleton, rootNode->mChildren[i], bone.index);
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

    MeshPtr AssImpMeshReader::readMesh(InputStream* stream, unsigned readFlags) const
    {
        if (!stream)
            return nullptr;

        bool readNormals = (readFlags & DontReadNormals) == 0;
        bool readTangents = readNormals && (readFlags & DontReadTangents) == 0;
        bool readTexCoords = (readFlags & DontReadTexCoords) == 0;
        bool readSkeleton = (readFlags & DontReadSkeleton) == 0;

        const aiScene* scene = nullptr;
        const unsigned flags =
            aiProcess_Triangulate |
            (!readNormals ? 0 : aiProcess_GenSmoothNormals) |
            (!readTangents ? 0 : aiProcess_CalcTangentSpace) |
            (!readTexCoords ? 0 : aiProcess_FlipUVs);

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

        if (!scene) {
            Z_LOG("Unable to load file \"" << stream->name() << "\": " << importer.GetErrorString());
            return nullptr;
        }

        if (scene->mNumMeshes == 0) {
            Z_LOG("File \"" << stream->name() << "\" does not have meshes.");
            return nullptr;
        }

        MeshPtr mesh = std::make_shared<Mesh>();
        mesh->elements().reserve(scene->mNumMeshes);

        SkeletonPtr skeleton;
        if (readSkeleton) {
            skeleton = std::make_shared<Skeleton>();
            readNodeHierarchy(skeleton, scene->mRootNode, size_t(-1));
            mesh->setSkeleton(skeleton);
        }

        for (int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* sceneMesh = scene->mMeshes[i];

            if (sceneMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
                if (sceneMesh->mName.length == 0)
                    Z_LOG("In \"" << stream->name() << "\": ignoring non-triangulated mesh #" << i << ".");
                else {
                    Z_LOG("In \"" << stream->name() << "\": ignoring non-triangulated mesh #" << i
                        << " (\"" << sceneMesh->mName.C_Str() << "\").");
                }
                continue;
            }

            Mesh::ElementPtr element = std::make_shared<Mesh::Element>();
            element->name.assign(sceneMesh->mName.data, sceneMesh->mName.length);

            element->positions.reserve(sceneMesh->mNumVertices);
            for (int j = 0; j < sceneMesh->mNumVertices; j++)
                appendVec3(element->positions, sceneMesh->mVertices[j]);

            if (readNormals) {
                element->normals.reserve(sceneMesh->mNumVertices);
                for (int j = 0; j < sceneMesh->mNumVertices; j++)
                    appendVec3(element->normals, sceneMesh->mNormals[j]);
            }

            if (readTangents) {
                element->tangents.reserve(sceneMesh->mNumVertices);
                element->bitangents.reserve(sceneMesh->mNumVertices);
                for (int j = 0; j < sceneMesh->mNumVertices; j++) {
                    appendVec3(element->tangents, sceneMesh->mTangents[j]);
                    appendVec3(element->bitangents, sceneMesh->mBitangents[j]);
                }
            }

            if (readTexCoords && sceneMesh->HasTextureCoords(0)) {
                element->texCoords.reserve(sceneMesh->mNumVertices);
                for (int j = 0; j < sceneMesh->mNumVertices; j++)
                    appendVec2(element->texCoords, sceneMesh->mTextureCoords[0][j]);
            }

            if (readSkeleton && sceneMesh->HasBones()) {
                element->boneWeights.resize(sceneMesh->mNumVertices);
                for (int j = 0; j < sceneMesh->mNumBones; j++) {
                    const aiBone* sceneMeshBone = sceneMesh->mBones[j];
                    std::string boneName(sceneMeshBone->mName.data, sceneMeshBone->mName.length);

                    Skeleton::Bone& bone = skeleton->getOrAddBone(boneName);
                    convertMatrix(bone.matrix(), sceneMeshBone->mOffsetMatrix);

                    for (int k = 0; k < sceneMeshBone->mNumWeights; k++) {
                        Mesh::BoneWeights& weights = element->boneWeights[sceneMeshBone->mWeights[k].mVertexId];
                        for (int index = 0; index < Mesh::MAX_BONES_PER_VERTEX; index++) {
                            if (weights.boneWeight[index] == 0.0f) {
                                weights.boneWeight[index] = sceneMeshBone->mWeights[k].mWeight;
                                weights.boneIndex[index] = bone.index;
                                break;
                            }
                        }
                    }
                }
            }

            element->indices.reserve(sceneMesh->mNumFaces * 3);
            for (int j = 0; j < sceneMesh->mNumFaces; j++) {
                if (sceneMesh->mFaces[j].mNumIndices != 3)
                    continue;
                element->indices.emplace_back(sceneMesh->mFaces[j].mIndices[0]);
                element->indices.emplace_back(sceneMesh->mFaces[j].mIndices[1]);
                element->indices.emplace_back(sceneMesh->mFaces[j].mIndices[2]);
            }

            mesh->elements().push_back(std::move(element));
        }

        return mesh;
    }
}
