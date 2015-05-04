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
#include "GLSkeletonAnimatedMesh.h"
#include "utility/debug.h"
#include <unordered_set>

namespace Z
{
    GLSkeletonAnimatedMesh::GLSkeletonAnimatedMesh(GLResourceManager* manager)
        : GLResource(manager)
    {
    }

    GLSkeletonAnimatedMesh::~GLSkeletonAnimatedMesh()
    {
        unload();
    }

    void GLSkeletonAnimatedMesh::reload()
    {
        unload();
    }

    void GLSkeletonAnimatedMesh::unload()
    {
        m_Elements.clear();
        m_VertexBuffers.clear();
        m_IndexBuffers.clear();
    }

    void GLSkeletonAnimatedMesh::render() const
    {
        gl::EnableVertexAttribArray(GLAttribute::Position);
        gl::EnableVertexAttribArray(GLAttribute::TexCoord);
        gl::EnableVertexAttribArray(GLAttribute::Normal);
        gl::EnableVertexAttribArray(GLAttribute::Tangent);
        gl::EnableVertexAttribArray(GLAttribute::Bitangent);

        for (const auto& element : m_Elements) {
            if (!m_VertexBuffers[element.vertexBuffer]->bind())
                continue;
            if (!m_IndexBuffers[element.indexBuffer]->bind())
                continue;

            gl::VertexAttribPointer(GLAttribute::Position, 3, GL::FLOAT, GL::FALSE,
                sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
            gl::VertexAttribPointer(GLAttribute::TexCoord, 2, GL::FLOAT, GL::FALSE,
                sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
            gl::VertexAttribPointer(GLAttribute::Normal, 3, GL::FLOAT, GL::FALSE,
                sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
            gl::VertexAttribPointer(GLAttribute::Tangent, 3, GL::FLOAT, GL::FALSE,
                sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
            gl::VertexAttribPointer(GLAttribute::Bitangent, 3, GL::FLOAT, GL::FALSE,
                sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

            gl::DrawElements(GL::TRIANGLES, element.indexBufferLength, GL::UNSIGNED_SHORT,
                reinterpret_cast<void*>(sizeof(uint16_t) * element.indexBufferOffset));
        }

        gl::BindBuffer(GL::ARRAY_BUFFER, 0);
        gl::BindBuffer(GL::ELEMENT_ARRAY_BUFFER, 0);

        gl::DisableVertexAttribArray(GLAttribute::Position);
        gl::DisableVertexAttribArray(GLAttribute::TexCoord);
        gl::DisableVertexAttribArray(GLAttribute::Normal);
        gl::DisableVertexAttribArray(GLAttribute::Tangent);
        gl::DisableVertexAttribArray(GLAttribute::Bitangent);
    }

    void GLSkeletonAnimatedMesh::initFromMesh(const MeshPtr& mesh)
    {
        unload();

        Z_CHECK(mesh != nullptr);
        if (!mesh)
            return;

        const auto& elements = mesh->elements();
        m_Elements.reserve(elements.size());

        size_t vertexBufferIndex = 0;
        size_t indexBufferIndex = 0;
        std::vector<Vertex> vertexData;
        std::vector<uint16_t> indexData;

std::vector<glm::mat4> boneMatrices;
mesh->animation(size_t(0))->getPoseForTime(1.0f, boneMatrices);

        std::unordered_set<Mesh::ElementPtr> processedElements;
        while (processedElements.size() < elements.size())
        {
            for (const auto& element : mesh->elements())
            {
                if (processedElements.find(element) != processedElements.end())
                    continue;

                if (element->positions.empty() || element->indices.empty()) {
                    processedElements.insert(element);
                    continue;
                }

                if (vertexData.size() + element->positions.size() > 65535) {
                    Z_CHECK(element->positions.size() <= 65535);
                    if (element->positions.size() > 65535)
                        processedElements.insert(element);
                    continue;
                }

                size_t indexOffset = vertexData.size();
                size_t numVertices = element->positions.size();
                size_t numIndices = element->indices.size();

                vertexData.reserve(indexOffset + numVertices);
                indexData.reserve(indexData.size() + numIndices);

                for (size_t i = 0; i < numVertices; i++) {
                    Vertex vertex;

glm::vec4 v = glm::vec4(element->positions[i], 1.0f);
const Mesh::BoneWeights& b = element->boneWeights[i];
glm::mat4 transform;
bool first = true;
for (int j = 0; j < Mesh::MAX_BONES_PER_VERTEX; j++) {
    if (b.boneWeight[j] != 0.0f) {
        glm::mat4 m = boneMatrices[b.boneIndex[j]] * b.boneWeight[j];
        if (first)
            transform = m;
        else
            transform += m;
        first = false;
    }
}
if (!first)
    v = transform * v;

                    vertex.position[0] = v.x;
                    vertex.position[1] = v.y;
                    vertex.position[2] = v.z;
                    if (element->texCoords.empty()) {
                        vertex.texCoord[0] = 0.0f;
                        vertex.texCoord[1] = 0.0f;
                    } else {
                        vertex.texCoord[0] = element->texCoords[i].x;
                        vertex.texCoord[1] = element->texCoords[i].y;
                    }
                    vertex.normal[0] = element->normals[i].x;
                    vertex.normal[1] = element->normals[i].y;
                    vertex.normal[2] = element->normals[i].z;
                    vertex.tangent[0] = element->tangents[i].x;
                    vertex.tangent[1] = element->tangents[i].y;
                    vertex.tangent[2] = element->tangents[i].z;
                    vertex.bitangent[0] = element->bitangents[i].x;
                    vertex.bitangent[1] = element->bitangents[i].y;
                    vertex.bitangent[2] = element->bitangents[i].z;
                    vertexData.push_back(vertex);
                }

                for (size_t i = 0; i < numIndices; i++)
                    indexData.push_back(element->indices[i] + indexOffset);

                Element elementInfo;
                elementInfo.vertexBuffer = vertexBufferIndex;
                elementInfo.indexBuffer = indexBufferIndex;
                elementInfo.indexBufferOffset = indexOffset;
                elementInfo.indexBufferLength = numIndices;
                m_Elements.push_back(elementInfo);

                processedElements.insert(element);
            }

            if (!vertexData.empty() && !indexData.empty()) {
                GLBufferPtr vertexBuffer = std::make_shared<GLBuffer>(resourceManager(), GL::ARRAY_BUFFER);
                GLBufferPtr indexBuffer = std::make_shared<GLBuffer>(resourceManager(), GL::ELEMENT_ARRAY_BUFFER);

                vertexBuffer->reload();
                vertexBuffer->setData(vertexData.data(), vertexData.size() * sizeof(Vertex), GL::STATIC_DRAW);
                vertexData.clear();

                indexBuffer->reload();
                indexBuffer->setData(indexData.data(), indexData.size() * sizeof(uint16_t), GL::STATIC_DRAW);
                indexData.clear();

                m_VertexBuffers.push_back(std::move(vertexBuffer));
                m_IndexBuffers.push_back(std::move(indexBuffer));
                ++vertexBufferIndex;
                ++indexBufferIndex;
            }
        }

        gl::BindBuffer(GL::ARRAY_BUFFER, 0);
        gl::BindBuffer(GL::ELEMENT_ARRAY_BUFFER, 0);
    }
}
