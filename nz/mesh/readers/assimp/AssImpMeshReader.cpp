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
#include <unordered_set>
#include <mutex>
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
                Z_LOG(" - AssImp: " << std::string(message, length));
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


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static inline void convertVec3(glm::vec3& out, const aiVector3D& in)
    {
        out.x = in.x;
        out.y = in.y;
        out.z = in.z;
    }

    static inline void convertQuat(glm::quat& out, const aiQuaternion& in)
    {
        out.x = in.x;
        out.y = in.y;
        out.z = in.z;
        out.w = in.w;
    }

    static inline void convertMatrix(glm::mat4& out, const aiMatrix4x4& in)
    {
        out[0][0] = in.a1; out[1][0] = in.a2; out[2][0] = in.a3; out[3][0] = in.a4;
        out[0][1] = in.b1; out[1][1] = in.b2; out[2][1] = in.b3; out[3][1] = in.b4;
        out[0][2] = in.c1; out[1][2] = in.c2; out[2][2] = in.c3; out[3][2] = in.c4;
        out[0][3] = in.d1; out[1][3] = in.d2; out[2][3] = in.d3; out[3][3] = in.d4;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Reader
    {
        virtual ~Reader() = default;
        virtual void read(glm::vec4& out) = 0;
    };

    struct Vector3DReader : public Reader
    {
        const aiVector3D* p;
        const float w;
        Vector3DReader(const aiVector3D* p, float w) : p(p), w(w) {}
        void read(glm::vec4& out) override { out.x = p->x; out.y = p->y; out.z = p->z; out.w = w; ++p; }
    };

    struct Vec4Reader : public Reader
    {
        const glm::vec4* p;
        explicit Vec4Reader(const glm::vec4* p) : p(p) {}
        void read(glm::vec4& out) override { out = *p++; }
    };


    struct Writer
    {
        const size_t stride;
        explicit Writer(const VertexFormatPtr& format) : stride(format->size()) {}
        virtual ~Writer() = default;
        virtual void write(const glm::vec4& in) = 0;
        template <class T> void advance(T*& p) { p = reinterpret_cast<T*>(reinterpret_cast<char*>(p) + stride); }
    };

    struct UnsignedByteWriter : public Writer
    {
        VertexFormat::UByte4* p;
        UnsignedByteWriter(VertexFormat::UByte4* p, const VertexFormatPtr& format) : Writer(format), p(p) {}
        void write(const glm::vec4& in) override {
            (*p)[0] = static_cast<unsigned char>(glm::clamp(in.x, 0.0f, 255.0f));
            (*p)[1] = static_cast<unsigned char>(glm::clamp(in.y, 0.0f, 255.0f));
            (*p)[2] = static_cast<unsigned char>(glm::clamp(in.z, 0.0f, 255.0f));
            (*p)[3] = static_cast<unsigned char>(glm::clamp(in.w, 0.0f, 255.0f));
            advance(p);
        }
    };

    struct UnsignedByteNormWriter : public Writer
    {
        VertexFormat::UByte4* p;
        UnsignedByteNormWriter(VertexFormat::UByte4* p, const VertexFormatPtr& format) : Writer(format), p(p) {}
        void write(const glm::vec4& in) override {
            (*p)[0] = static_cast<unsigned char>(glm::clamp(in.x * 255.0f, 0.0f, 255.0f));
            (*p)[1] = static_cast<unsigned char>(glm::clamp(in.y * 255.0f, 0.0f, 255.0f));
            (*p)[2] = static_cast<unsigned char>(glm::clamp(in.z * 255.0f, 0.0f, 255.0f));
            (*p)[3] = static_cast<unsigned char>(glm::clamp(in.w * 255.0f, 0.0f, 255.0f));
            advance(p);
        }
    };

    struct FloatWriter : public Writer
    {
        float* p;
        FloatWriter(float* p, const VertexFormatPtr& format) : Writer(format), p(p) {}
        void write(const glm::vec4& in) override { *p = in.x; advance(p); }
    };

    struct Vec2Writer : public Writer
    {
        glm::vec2* p;
        Vec2Writer(glm::vec2* p, const VertexFormatPtr& format) : Writer(format), p(p) {}
        void write(const glm::vec4& in) override { p->x = in.x; p->y = in.y; advance(p); }
    };

    struct Vec3Writer : public Writer
    {
        glm::vec3* p;
        Vec3Writer(glm::vec3* p, const VertexFormatPtr& format) : Writer(format), p(p) {}
        void write(const glm::vec4& in) override { p->x = in.x; p->y = in.y; p->z = in.z; advance(p); }
    };

    struct Vec4Writer : public Writer
    {
        glm::vec4* p;
        Vec4Writer(glm::vec4* p, const VertexFormatPtr& format) : Writer(format), p(p) {}
        void write(const glm::vec4& in) override { *p = in; advance(p); }
    };


    static void copy(Writer& writer, Reader& reader, size_t count)
    {
        glm::vec4 v;
        while (count--) {
            reader.read(v);
            writer.write(v);
        }
    }


    static void copyAttribute(const VertexFormatPtr& format, VertexFormat::AttributeID attributeID,
        void* destination, Reader& reader, size_t count)
    {
        if (!format->hasAttribute(attributeID))
            return;

        const VertexFormat::Attribute& attribute = format->attribute(attributeID);
        switch (attribute.type)
        {
        case VertexFormat::UnsignedByte: {
            Z_ASSERT(attribute.count == 4);
            VertexFormat::UByte4* ub = &format->refUByte(attribute.id, destination);
            switch (attributeID)
            {
            case VertexFormat::Normal:
            case VertexFormat::Tangent:
            case VertexFormat::Bitangent:
            case VertexFormat::TexCoord:
            case VertexFormat::BoneWeights: {
                UnsignedByteNormWriter writer(ub, format);
                copy(writer, reader, count);
              } return;

            case VertexFormat::Position:
            case VertexFormat::BoneIndices: {
                UnsignedByteWriter writer(ub, format);
                copy(writer, reader, count);
              } return;

            case VertexFormat::NumAttributes:
                break;
            }
            Z_ASSERT_MSG(false, "Invalid attribute ID.");
          } return;

        case VertexFormat::Float:
            Z_ASSERT(attribute.count > 0 && attribute.count <= 4);
            switch (attribute.count)
            {
            case 1: {
                float* f = &format->refFloat(attribute.id, destination);
                FloatWriter writer(f, format);
                copy(writer, reader, count);
              } break;

            case 2: {
                glm::vec2* v = &format->refVec2(attribute.id, destination);
                Vec2Writer writer(v, format);
                copy(writer, reader, count);
              } break;

            case 3: {
                glm::vec3* v = &format->refVec3(attribute.id, destination);
                Vec3Writer writer(v, format);
                copy(writer, reader, count);
              } break;

            case 4: {
                glm::vec4* v = &format->refVec4(attribute.id, destination);
                Vec4Writer writer(v, format);
                copy(writer, reader, count);
              } break;
            }
            return;
        }

        Z_ASSERT_MSG(false, "Invalid vertex attribute format.");
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static void readNodeHierarchy(SkeletonPtr& skeleton, const aiNode* rootNode, size_t parentBoneIndex)
    {
        auto nodeName = Utf8String::fromRawBytes(rootNode->mName.data, rootNode->mName.length);
        Skeleton::Bone& bone = skeleton->getOrAddBone(nodeName);

        Z_CHECK(bone.index != parentBoneIndex);
        Z_CHECK(bone.parentIndex == size_t(-1) || bone.parentIndex == parentBoneIndex);
        if (bone.index != parentBoneIndex)
            bone.parentIndex = parentBoneIndex;

        convertMatrix(bone.matrix(), rootNode->mTransformation);

        if (nodeName.empty())
            Z_LOG(" - Bone #" << bone.index << '.');
        else
            Z_LOG(" - Bone #" << bone.index << " (\"" << nodeName << "\").");

        size_t boneIndex = bone.index;
        for (int i = 0; i < rootNode->mNumChildren; i++)
            readNodeHierarchy(skeleton, rootNode->mChildren[i], boneIndex);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    MeshPtr AssImpMeshReader::readMesh(InputStream* stream, const VertexFormatPtr& format, unsigned readFlags) const
    {
        if (!stream)
            return nullptr;

        Z_LOG("Loading mesh \"" << stream->name() << "\".");

        bool readPositions = format->hasAttribute(VertexFormat::Position);
        bool readTexCoords = format->hasAttribute(VertexFormat::TexCoord);
        bool readNormals = format->hasAttribute(VertexFormat::Normal);
        bool readTangents = format->hasAttribute(VertexFormat::Tangent);
        bool readBitangents = format->hasAttribute(VertexFormat::Bitangent);
        bool readBoneIndices = format->hasAttribute(VertexFormat::BoneIndices);
        bool readBoneWeights = format->hasAttribute(VertexFormat::BoneWeights);
        bool readSkeleton = (readFlags & DontReadSkeleton) == 0;

        const aiScene* scene = nullptr;
        const unsigned flags =
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_SortByPType |
            aiProcess_FindInvalidData |
            aiProcess_SplitLargeMeshes |
            aiProcess_OptimizeMeshes |
            //aiProcess_OptimizeGraph |
            (!readBoneIndices && !readBoneWeights ? 0 : aiProcess_LimitBoneWeights) |
            (!readNormals && !readTangents && !readBitangents ? 0 : aiProcess_GenSmoothNormals) |
            (!readTangents && !readBitangents ? 0 : aiProcess_CalcTangentSpace) |
            (!readTexCoords ? 0 : aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_FlipUVs);

        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 65535);
        importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1000000000);
        importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

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

        MeshPtr mesh = std::make_shared<Mesh>(format);
        mesh->elements().reserve(scene->mNumMeshes);

        SkeletonPtr skeleton;
        if (readSkeleton) {
            skeleton = std::make_shared<Skeleton>();
            readNodeHierarchy(skeleton, scene->mRootNode, size_t(-1));
            mesh->setSkeleton(skeleton);
            Z_LOG(" - Total " << skeleton->numBones() << " bone" << (skeleton->numBones() == 1 ? "" : "s") << '.');

            aiMatrix4x4 globalInverseTransform = scene->mRootNode->mTransformation;
            globalInverseTransform.Inverse();
            convertMatrix(skeleton->globalInverseTransform(), globalInverseTransform);
        }

        std::vector<uint8_t> vertexData;
        std::vector<uint16_t> indexData;
        size_t vertexDataLength = 0;
        size_t vertexBufferIndex = 0;

        std::unordered_set<const aiMesh*> processedMeshes;
        int firstUnprocessedMesh = 0;
        while (processedMeshes.size() < scene->mNumMeshes)
        {
            for (int i = firstUnprocessedMesh; i < scene->mNumMeshes; i++)
            {
                const aiMesh* sceneMesh = scene->mMeshes[i];

                if (processedMeshes.find(sceneMesh) != processedMeshes.end())
                    continue;

                if (sceneMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
                    if (sceneMesh->mName.length == 0)
                        Z_LOG("In \"" << stream->name() << "\": ignoring non-triangulated mesh #" << i << ".");
                    else {
                        Z_LOG("In \"" << stream->name() << "\": ignoring non-triangulated mesh #" << i
                            << " (\"" << sceneMesh->mName.C_Str() << "\").");
                    }
                } else if (vertexDataLength + sceneMesh->mNumVertices > 65535) {
                    if (sceneMesh->mNumVertices <= 65535)
                        continue;

                    if (sceneMesh->mName.length == 0)
                        Z_LOG("In \"" << stream->name() << "\": mesh #" << i << " has too many vertices.");
                    else {
                        Z_LOG("In \"" << stream->name() << "\": mesh #" << i
                            << " (\"" << sceneMesh->mName.C_Str() << "\") has too many vertices.");
                    }
                } else {
                    Mesh::Element element;
                    element.name.assign(sceneMesh->mName.data, sceneMesh->mName.length);

                    // FIXME: read material

                    element.vertexBuffer = vertexBufferIndex;
                    size_t firstIndex = vertexDataLength;
                    vertexDataLength += sceneMesh->mNumVertices;
                    vertexData.resize(format->size() * vertexDataLength);
                    uint8_t* destination = vertexData.data() + firstIndex * format->size();

                    if (sceneMesh->HasPositions()) {
                        Vector3DReader reader(sceneMesh->mVertices, 1.0f);
                        copyAttribute(format, VertexFormat::Position, destination, reader, sceneMesh->mNumVertices);
                    }

                    if (sceneMesh->HasNormals()) {
                        Vector3DReader reader(sceneMesh->mNormals, 0.0f);
                        copyAttribute(format, VertexFormat::Normal, destination, reader, sceneMesh->mNumVertices);
                    }

                    if (sceneMesh->HasTangentsAndBitangents()) {
                        Vector3DReader treader(sceneMesh->mTangents, 0.0f);
                        Vector3DReader breader(sceneMesh->mBitangents, 0.0f);
                        copyAttribute(format, VertexFormat::Tangent, destination, treader, sceneMesh->mNumVertices);
                        copyAttribute(format, VertexFormat::Bitangent, destination, breader, sceneMesh->mNumVertices);
                    }

                    if (sceneMesh->HasTextureCoords(0)) {
                        Vector3DReader reader(sceneMesh->mTextureCoords[0], 0.0f);
                        copyAttribute(format, VertexFormat::TexCoord, destination, reader, sceneMesh->mNumVertices);
                    }

                    bool needWeights = (format->hasAttribute(VertexFormat::BoneWeights));
                    bool needIndices = (format->hasAttribute(VertexFormat::BoneIndices));
                    if (sceneMesh->HasBones() && (readSkeleton || needWeights || needIndices)) {
                        std::vector<glm::vec4> weights;
                        std::vector<glm::vec4> indices;

                        if (needWeights)
                            weights.resize(sceneMesh->mNumVertices);
                        if (needIndices)
                            indices.resize(sceneMesh->mNumVertices);

                        for (size_t j = 0; j < sceneMesh->mNumBones; j++) {
                            const aiBone* sceneMeshBone = sceneMesh->mBones[j];
                            std::string boneName(sceneMeshBone->mName.data, sceneMeshBone->mName.length);

                            Skeleton::Bone& bone = skeleton->getOrAddBone(boneName);
                            convertMatrix(bone.matrix(), sceneMeshBone->mOffsetMatrix);

                            if (needWeights || needIndices) {
                                for (int k = 0; k < sceneMeshBone->mNumWeights; k++) {
                                    size_t v = sceneMeshBone->mWeights[k].mVertexId;
                                    for (int index = 0; index < 4; index++) {
                                        if (weights[v][index] == 0.0f) {
                                            weights[v][index] = sceneMeshBone->mWeights[k].mWeight;
                                            indices[v][index] = bone.index;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        if (needWeights) {
                            Vec4Reader r(weights.data());
                            copyAttribute(format, VertexFormat::BoneWeights, destination, r, sceneMesh->mNumVertices);
                        }

                        if (needIndices) {
                            Vec4Reader r(indices.data());
                            copyAttribute(format, VertexFormat::BoneIndices, destination, r, sceneMesh->mNumVertices);
                        }
                    }

                    element.indexBufferOffset = indexData.size();
                    element.indexBufferLength = sceneMesh->mNumFaces * 3;
                    indexData.reserve(element.indexBufferOffset + element.indexBufferLength);
                    for (int j = 0; j < sceneMesh->mNumFaces; j++) {
                        if (sceneMesh->mFaces[j].mNumIndices != 3)
                            continue;
                        indexData.emplace_back(sceneMesh->mFaces[j].mIndices[0] + firstIndex);
                        indexData.emplace_back(sceneMesh->mFaces[j].mIndices[1] + firstIndex);
                        indexData.emplace_back(sceneMesh->mFaces[j].mIndices[2] + firstIndex);
                    }

                    if (element.name.empty()) {
                        Z_LOG(" - Element #" << i << ": "
                            << sceneMesh->mNumVertices << " vertices, " << sceneMesh->mNumFaces << " faces.");
                    } else {
                        Z_LOG(" - Element #" << i << " (\"" << element.name << "\"): "
                            << sceneMesh->mNumVertices << " vertices, " << sceneMesh->mNumFaces << " faces.");
                    }

                    mesh->elements().emplace_back(std::move(element));
                }

                processedMeshes.insert(sceneMesh);
                if (i == firstUnprocessedMesh)
                    firstUnprocessedMesh = i + 1;
            }

            if (!vertexData.empty()) {
                mesh->addVertexBuffer(std::move(vertexData));
                vertexData.clear();
                vertexDataLength = 0;
                ++vertexBufferIndex;
            }
        }

        mesh->indexBuffer() = std::move(indexData);

        Z_LOG(" - Total "
            << mesh->elements().size() << " element" << (mesh->elements().size() == 1 ? "" : "s") << " ("
            << mesh->vertexBuffers().size() << " vertex buffer" << (mesh->vertexBuffers().size() == 1 ? "" : "s")
            << ").");

        if (readSkeleton) {
            for (int i = 0; i < scene->mNumAnimations; i++) {
                const aiAnimation* sceneAnimation = scene->mAnimations[i];
                auto animationName = Utf8String::fromRawBytes(sceneAnimation->mName.data, sceneAnimation->mName.length);

                auto animation = mesh->addAnimation(animationName);
                animation->setDurationInTicks(sceneAnimation->mDuration);
                animation->setTicksPerSecond(sceneAnimation->mTicksPerSecond);

                if (animationName.empty()) {
                    Z_LOG(" - Animation #" << i << ": affects "
                        << sceneAnimation->mNumChannels << " bone" << (sceneAnimation->mNumChannels == 1 ? "" : "s")
                        << ", " << animation->durationInTicks() << " ticks (with " << animation->ticksPerSecond()
                        << " ticks/sec).");
                } else {
                    Z_LOG(" - Animation #" << i << " (\"" << animationName << "\"): affects "
                        << sceneAnimation->mNumChannels << " bone" << (sceneAnimation->mNumChannels == 1 ? "" : "s")
                        << ", " << animation->durationInTicks() << " ticks (with " << animation->ticksPerSecond()
                        << " ticks/sec).");
                }

                for (int j = 0; j < sceneAnimation->mNumChannels; j++) {
                    const aiNodeAnim* channel = sceneAnimation->mChannels[j];
                    auto boneName = Utf8String::fromRawBytes(channel->mNodeName.data, channel->mNodeName.length);

                    Skeleton::Bone& bone = skeleton->getOrAddBone(boneName);
                    SkeletonAnimation::Channel& keys = animation->addChannel(bone.index,
                        channel->mNumPositionKeys, channel->mNumScalingKeys, channel->mNumRotationKeys);

                    for (size_t k = 0; k < channel->mNumPositionKeys; k++) {
                        const aiVectorKey& key = channel->mPositionKeys[k];
                        auto& ref = animation->positionKeys()[keys.positionKeysOffset + k];
                        ref.time = key.mTime;
                        convertVec3(ref.position, key.mValue);
                    }

                    for (size_t k = 0; k < channel->mNumScalingKeys; k++) {
                        const aiVectorKey& key = channel->mScalingKeys[k];
                        auto& ref = animation->scaleKeys()[keys.scaleKeysOffset + k];
                        ref.time = key.mTime;
                        convertVec3(ref.scale, key.mValue);
                    }

                    for (size_t k = 0; k < channel->mNumRotationKeys; k++) {
                        const aiQuatKey& key = channel->mRotationKeys[k];
                        auto& ref = animation->rotationKeys()[keys.rotationKeysOffset + k];
                        ref.time = key.mTime;
                        convertQuat(ref.rotation, key.mValue);
                    }
                }
            }

            Z_LOG(" - Total " << mesh->numAnimations() << " animation"
                << (mesh->numAnimations() == 1 ? "" : "s") << '.');
        }

        return mesh;
    }
}
