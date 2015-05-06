/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include "GLResourceManager.h"
#include "GLResource.h"
#include "utility/streams/FileInputStream.h"
#include "utility/streams/BufferedInputStream.h"
#include "utility/debug.h"
#include "image/ImageReader.h"
#include "mesh/MeshReader.h"
#include "utility/debug.h"
#include <chrono>
#include <vector>

namespace Z
{
    class GLResourceManager::Program : public GLProgram
    {
    public:
        Program(const std::string& fileName, GLResourceManager* resourceManager)
            : GLProgram(resourceManager)
            , m_FileName(fileName)
        {
            reload();
        }

        void reload() override
        {
            GLProgram::reload();

            FileReaderPtr reader = resourceManager()->fileSystem()->openFile(m_FileName);
            if (reader) {
                FileInputStreamPtr stream = std::make_shared<FileInputStream>(reader);
                BufferedInputStream buffer(std::move(stream));
                load(&buffer);
            }
        }

    private:
        std::string m_FileName;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class GLResourceManager::StaticProgram : public GLProgram
    {
    public:
        StaticProgram(const std::string& vertex, const std::string& fragment, GLResourceManager* resourceManager)
            : GLProgram(resourceManager)
            , m_Vertex(vertex)
            , m_Fragment(fragment)
        {
            reload();
        }

        void reload() override
        {
            GLProgram::reload();
            loadSource(m_Vertex, m_Fragment);
        }

    private:
        std::string m_Vertex;
        std::string m_Fragment;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class GLResourceManager::Texture : public GLTexture
    {
    public:
        Texture(const std::string& fileName, GLResourceManager* resourceManager)
            : GLTexture(resourceManager)
            , m_FileName(fileName)
        {
            reload();
        }

        void reload() override
        {
            GLTexture::reload();

            FileReaderPtr reader = resourceManager()->fileSystem()->openFile(m_FileName);
            ImagePtr image = ImageReader::read(reader);
            upload(0, image);
        }

    private:
        std::string m_FileName;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class GLResourceManager::Mesh : public GLMesh
    {
    public:
        Mesh(const std::string& fileName, const VertexFormatPtr& format, GLResourceManager* resourceManager)
            : GLMesh(resourceManager)
            , m_FileName(fileName)
            , m_VertexFormat(format)
        {
            reload();
        }

        void reload() override
        {
            GLMesh::reload();

            FileReaderPtr reader = resourceManager()->fileSystem()->openFile(m_FileName);
            MeshPtr mesh = MeshReader::read(reader, m_VertexFormat, MeshReader::DontReadSkeleton);
            initFromMesh(mesh);
        }

    private:
        std::string m_FileName;
        VertexFormatPtr m_VertexFormat;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class GLResourceManager::SkeletonAnimatedMesh : public GLSkeletonAnimatedMesh
    {
    public:
        SkeletonAnimatedMesh(const std::string& fileName, const VertexFormatPtr& format, GLResourceManager* resourceManager)
            : GLSkeletonAnimatedMesh(resourceManager)
            , m_FileName(fileName)
            , m_VertexFormat(format)
        {
            reload();
        }

        void reload() override
        {
            GLSkeletonAnimatedMesh::reload();

            FileReaderPtr reader = resourceManager()->fileSystem()->openFile(m_FileName);
            MeshPtr mesh = MeshReader::read(reader, m_VertexFormat);
            initFromMesh(mesh);
        }

    private:
        std::string m_FileName;
        VertexFormatPtr m_VertexFormat;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GLResourceManager::GLResourceManager(const FileSystemPtr& fileSystem)
        : m_FileSystem(fileSystem)
    {
    }

    GLResourceManager::~GLResourceManager()
    {
        shutdown();
    }

    void GLResourceManager::unloadAllResources()
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        std::vector<GLResource*> resources;
        resources.reserve(m_Resources.size());
        for (auto resource : m_Resources)
             resources.push_back(resource);

        Z_LOG("Unloading " << resources.size() << " resource" << (resources.size() == 1 ? "" : "s") << '.');

        for (auto resource : resources)
            resource->unload();
    }

    void GLResourceManager::shutdown()
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        m_Shutdown = true;
        unloadAllResources();

        for (auto resource : m_Resources)
            resource->m_Manager = nullptr;

        m_Resources.clear();
    }

    void GLResourceManager::beginReloadResources()
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        Z_CHECK(!m_Shutdown);
        if (m_Shutdown)
            return;

        m_ReloadingResourcesList.clear();
        m_ReloadingResourcesList.reserve(m_Resources.size());
        m_ReloadingResourcesList.insert(m_ReloadingResourcesList.begin(), m_Resources.begin(), m_Resources.end());
        m_ReloadingResourceIndex = 0;
        m_ReloadingResources.store(true);
        m_HaveErasedResources = false;

        Z_LOG("Begin reloading " << m_ReloadingResourcesList.size() << " resource"
            << (m_ReloadingResourcesList.size() == 1 ? "" : "s") << '.');
    }

    bool GLResourceManager::continueReloadResources(float* progress)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        Z_CHECK(!m_Shutdown);
        if (m_Shutdown)
            return false;

        auto startTime = std::chrono::steady_clock::now();

        while (m_ReloadingResourceIndex < m_ReloadingResourcesList.size()) {
            GLResource* resource = m_ReloadingResourcesList[m_ReloadingResourceIndex++];
            if (!m_HaveErasedResources || m_Resources.find(resource) != m_Resources.end())
                resource->reload();

            auto currentTime = std::chrono::steady_clock::now();
            auto timeSpent = std::chrono::duration<float, std::ratio<1>>(currentTime - startTime).count();
            if (timeSpent >= 1.0f / 30.0f)
                break;
        }

        float progressValue = float(m_ReloadingResourceIndex) / float(m_ReloadingResourcesList.size());
        if (progress)
            *progress = progressValue;

        bool stillReloading = m_ReloadingResourceIndex < m_ReloadingResourcesList.size();
        m_ReloadingResources.store(stillReloading);

        if (!stillReloading)
            m_ReloadingResourcesList.clear();

        if (stillReloading)
            Z_LOG("Continue reloading resources (" << int(progressValue * 100.0f) << "% done).");
        else
            Z_LOG("Done reloading resources.");

        return stillReloading;
    }

    GLProgramPtr GLResourceManager::createProgram(const std::string& vertex, const std::string& fragment)
    {
        return std::make_shared<StaticProgram>(vertex, fragment, this);
    }

    GLProgramPtr GLResourceManager::loadProgram(const std::string& fileName)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        auto it = m_Programs.find(fileName);
        if (it == m_Programs.end())
            it = m_Programs.insert(std::make_pair(fileName, std::weak_ptr<Program>())).first;
        else {
            std::shared_ptr<Program> program = it->second.lock();
            if (program)
                return program;
        }

        std::shared_ptr<Program> program = std::make_shared<Program>(fileName, this);
        it->second = program;

        return program;
    }

    GLTexturePtr GLResourceManager::loadTexture(const std::string& fileName)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        auto it = m_Textures.find(fileName);
        if (it == m_Textures.end())
            it = m_Textures.insert(std::make_pair(fileName, std::weak_ptr<Texture>())).first;
        else {
            std::shared_ptr<Texture> texture = it->second.lock();
            if (texture)
                return texture;
        }

        std::shared_ptr<Texture> texture = std::make_shared<Texture>(fileName, this);
        it->second = texture;

        return texture;
    }

    const VertexFormatPtr& GLResourceManager::defaultStaticVertexFormat()
    {
        if (!m_DefaultStaticVertexFormat) {
            VertexFormatPtr format = std::make_shared<VertexFormat>();
            format->addAttribute(VertexFormat::Position, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::TexCoord, 2, VertexFormat::Float);
            format->addAttribute(VertexFormat::Normal, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::Tangent, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::Bitangent, 3, VertexFormat::Float);
            m_DefaultStaticVertexFormat = std::move(format);
        }
        return m_DefaultStaticVertexFormat;
    }

    const VertexFormatPtr& GLResourceManager::defaultAnimatedVertexFormat()
    {
        if (!m_DefaultAnimatedVertexFormat) {
            VertexFormatPtr format = std::make_shared<VertexFormat>();
            format->addAttribute(VertexFormat::Position, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::TexCoord, 2, VertexFormat::Float);
            format->addAttribute(VertexFormat::Normal, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::Tangent, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::Bitangent, 3, VertexFormat::Float);
            format->addAttribute(VertexFormat::BoneIndices, 4, VertexFormat::UnsignedByte);
            format->addAttribute(VertexFormat::BoneWeights, 4, VertexFormat::Float);
            m_DefaultAnimatedVertexFormat = std::move(format);
        }
        return m_DefaultAnimatedVertexFormat;
    }

    GLMeshPtr GLResourceManager::loadMesh(const std::string& fileName)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        auto it = m_Meshes.find(fileName);
        if (it == m_Meshes.end())
            it = m_Meshes.insert(std::make_pair(fileName, std::weak_ptr<Mesh>())).first;
        else {
            std::shared_ptr<Mesh> mesh = it->second.lock();
            if (mesh)
                return mesh;
        }

        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(fileName, defaultStaticVertexFormat(), this);
        it->second = mesh;

        return mesh;
    }

    GLSkeletonAnimatedMeshPtr GLResourceManager::loadSkeletonAnimatedMesh(const std::string& fileName)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        auto it = m_SkeletonAnimatedMeshes.find(fileName);
        if (it == m_SkeletonAnimatedMeshes.end())
            it = m_SkeletonAnimatedMeshes.insert(std::make_pair(fileName, std::weak_ptr<SkeletonAnimatedMesh>())).first;
        else {
            std::shared_ptr<SkeletonAnimatedMesh> mesh = it->second.lock();
            if (mesh)
                return mesh;
        }

        std::shared_ptr<SkeletonAnimatedMesh> mesh =
            std::make_shared<SkeletonAnimatedMesh>(fileName, defaultAnimatedVertexFormat(), this);
        it->second = mesh;

        return mesh;
    }

    void GLResourceManager::onResourceCreated(GLResource* resource)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);

        Z_CHECK(!m_Shutdown);
        if (m_Shutdown)
            return;

        m_Resources.insert(resource);
    }

    void GLResourceManager::onResourceDestroyed(GLResource* resource)
    {
        std::lock_guard<decltype(m_Mutex)> lock(m_Mutex);
        m_Resources.erase(resource);
        m_HaveErasedResources = true;
    }
}
