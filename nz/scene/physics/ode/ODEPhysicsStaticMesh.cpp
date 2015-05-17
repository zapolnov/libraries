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
#include "ODEPhysicsStaticMesh.h"
#include "opengl/GLResourceManager.h"
#include "utility/debug.h"

namespace Z
{
    ODEPhysicsStaticMesh::ODEPhysicsStaticMesh(const ODEPhysicsWorld* world,
            GLResourceManager& resourceManager, const MeshPtr& mesh)
        : SceneMesh(resourceManager.createMesh(mesh))
        , m_Mesh(mesh)
    {
        Z_ASSERT(world != nullptr);
        m_ODE = world->m_Instance;

        Z_ASSERT(mesh->vertexFormat()->hasAttribute(VertexFormat::Position));
        Z_ASSERT(mesh->vertexFormat()->attribute(VertexFormat::Position).type == VertexFormat::Float);
        Z_ASSERT(mesh->vertexFormat()->attribute(VertexFormat::Position).count >= 3);

        size_t vertexStride = mesh->vertexFormat()->size();
        bool hasNormals = mesh->vertexFormat()->hasAttribute(VertexFormat::Normal);
        if (hasNormals) {
            Z_ASSERT(mesh->vertexFormat()->attribute(VertexFormat::Normal).type == VertexFormat::Float);
            Z_ASSERT(mesh->vertexFormat()->attribute(VertexFormat::Normal).count >= 3);
        }

        const dTriIndex* indexBuffer;
        if (sizeof(dTriIndex) == sizeof(mesh->indexBuffer()[0]))
            indexBuffer = reinterpret_cast<const dTriIndex*>(mesh->indexBuffer().data());
        else {
            m_IndicesBuffer.resize(mesh->indexBuffer().size());
            for (size_t i = 0; i < mesh->indexBuffer().size(); i++)
                m_IndicesBuffer[i] = dTriIndex(mesh->indexBuffer()[i]);
            indexBuffer = m_IndicesBuffer.data();
        }

        for (const auto& element : mesh->elements())
        {
            std::string name = GLMaterial::fileNameForMaterial(element.material->name());
            GLMaterialPtr material = resourceManager.loadMaterial(mesh->baseDirectory() + name);
            if (material && material->invisible())
                continue;

            const Mesh::VertexBuffer& vertexBuffer = mesh->vertexBuffers()[element.vertexBuffer];
            size_t vertexCount = vertexBuffer.size() / vertexStride;
            const void* vertexData = vertexBuffer.data() +
                mesh->vertexFormat()->attribute(VertexFormat::Position).offset;

            const dTriIndex* indices = indexBuffer + element.indexBufferOffset;
            size_t indexCount = element.indexBufferLength;
            const size_t indexTriStride = 3 * sizeof(dTriIndex);

            dTriMeshDataID data = dGeomTriMeshDataCreate();
            m_TriMeshData.push_back(data);

            if (!hasNormals) {
                dGeomTriMeshDataBuildSingle(data, vertexData, int(vertexStride), int(vertexCount),
                    indices, int(indexCount), int(indexTriStride));
            } else {
                const void* normalsData = vertexBuffer.data() +
                    mesh->vertexFormat()->attribute(VertexFormat::Normal).offset;

                dGeomTriMeshDataBuildSingle1(data, vertexData, int(vertexStride), int(vertexCount),
                    indices, int(indexCount), int(indexTriStride), normalsData);
            }

            dGeomID triMesh = dCreateTriMesh(m_ODE->m_Space, data, nullptr, nullptr, nullptr);
            dGeomSetCategoryBits(triMesh, CATEGORY_BIT);
            dGeomSetCollideBits(triMesh, ~CATEGORY_BIT);
            dGeomSetData(triMesh, this);
            m_Geoms.push_back(triMesh);
        }
    }

    ODEPhysicsStaticMesh::~ODEPhysicsStaticMesh()
    {
        for (dGeomID geom : m_Geoms)
            dGeomDestroy(geom);
        for (dTriMeshDataID data : m_TriMeshData)
            dGeomTriMeshDataDestroy(data);
    }

    void ODEPhysicsStaticMesh::invalidate()
    {
        SceneNode::invalidate();
        m_PositionChanged = true;
    }

    void ODEPhysicsStaticMesh::updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged)
    {
        SceneNode::updateTransform(parentMatrix, parentMatrixChanged);

        if (m_PositionChanged) {
            glm::vec4 pos = worldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            for (auto geom : m_Geoms)
                dGeomSetPosition(geom, pos.x, pos.y, pos.z);
            m_PositionChanged = false;
        }
    }
}
