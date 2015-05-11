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
#include "GLMesh.h"
#include "GLResourceManager.h"
#include "utility/debug.h"
#include <unordered_set>

namespace Z
{
    GLMesh::GLMesh(GLResourceManager* manager)
        : GLResource(manager)
    {
    }

    GLMesh::~GLMesh()
    {
        unload();
    }

    void GLMesh::reload()
    {
        unload();
    }

    void GLMesh::unload()
    {
        m_Elements.clear();
        m_VertexBuffers.clear();
        m_IndexBuffer.reset();
    }

    void GLMesh::render(size_t, float, const GLUniformSet* uniforms) const
    {
        if (m_Elements.empty() || m_VertexBuffers.empty() || !m_IndexBuffer)
            return;

        if (!m_IndexBuffer->bind())
            return;

        m_VertexBuffers[0]->enableAttributes();

        GLMaterialPtr currentMaterial;
        for (const auto& element : m_Elements) {
            if (currentMaterial != element.material) {
                if (!element.material || !element.material->bind()) {
                    currentMaterial.reset();
                    continue;
                }

                if (uniforms)
                    uniforms->upload(element.material->program());

                currentMaterial = element.material;
            }

            if (m_VertexBuffers[element.vertexBuffer]->bind()) {
                gl::DrawElements(GL::TRIANGLES, GL::Sizei(element.indexBufferLength), GL::UNSIGNED_SHORT,
                    reinterpret_cast<void*>(sizeof(uint16_t) * element.indexBufferOffset));
            }
        }

        gl::BindBuffer(GL::ARRAY_BUFFER, 0);
        gl::BindBuffer(GL::ELEMENT_ARRAY_BUFFER, 0);

        m_VertexBuffers[0]->disableAttributes();
    }

    void GLMesh::initFromMesh(const MeshPtr& mesh)
    {
        // Keep materials resident until this function returns
        std::vector<GLMaterialPtr> oldMaterials = std::move(m_Materials);
        m_Materials.clear();
        (void)oldMaterials;

        unload();

        Z_CHECK(mesh != nullptr);
        if (!mesh)
            return;

        const auto& elements = mesh->elements();
        m_Elements.reserve(elements.size());
        m_Materials.reserve(elements.size());

        std::unordered_set<GLMaterialPtr> materialSet;
        for (const auto& element : mesh->elements())
        {
            Element elementInfo;
            elementInfo.vertexBuffer = element.vertexBuffer;
            elementInfo.indexBufferOffset = element.indexBufferOffset;
            elementInfo.indexBufferLength = element.indexBufferLength;

            if (!element.material)
                elementInfo.material = resourceManager()->dummyMaterial();
            else {
                std::string name = GLMaterial::fileNameForMaterial(element.material->name());
                elementInfo.material = resourceManager()->loadMaterial(mesh->baseDirectory() + name);
            }

            if (elementInfo.material && materialSet.insert(elementInfo.material).second)
                m_Materials.emplace_back(elementInfo.material);

            m_Elements.push_back(elementInfo);
        }
        materialSet.clear();

        m_VertexBuffers.reserve(mesh->vertexBuffers().size());
        for (const auto& buffer : mesh->vertexBuffers()) {
            GLVertexBufferPtr vertexBuffer = std::make_shared<GLVertexBuffer>(nullptr, mesh->vertexFormat());
            vertexBuffer->reload();
            vertexBuffer->setData(buffer.data(), buffer.size(), GL::STATIC_DRAW);
            m_VertexBuffers.emplace_back(std::move(vertexBuffer));
        }
        gl::BindBuffer(GL::ARRAY_BUFFER, 0);

        const auto& indices = mesh->indexBuffer();
        m_IndexBuffer = std::make_shared<GLBuffer>(nullptr, GL::ELEMENT_ARRAY_BUFFER);
        m_IndexBuffer->reload();
        m_IndexBuffer->setData(indices.data(), indices.size() * sizeof(uint16_t), GL::STATIC_DRAW);
        gl::BindBuffer(GL::ELEMENT_ARRAY_BUFFER, 0);
    }
}
