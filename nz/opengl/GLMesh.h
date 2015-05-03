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

#pragma once
#include "GLResource.h"
#include "GLBuffer.h"
#include "GLAttribute.h"
#include "opengl.h"
#include "mesh/Mesh.h"
#include <memory>

namespace Z
{
    class GLMesh : public GLResource
    {
    public:
        explicit GLMesh(GLResourceManager* manager);
        ~GLMesh();

        void reload() override;
        void unload() override;

        void render() const;

    protected:
        struct Vertex
        {
            GL::Float position[3];
            GL::Float texCoord[2];
            GL::Float normal[3];
            GL::Float tangent[3];
            GL::Float bitangent[3];
        };

        struct Element
        {
            size_t vertexBuffer;
            size_t indexBuffer;
            size_t indexBufferOffset;
            size_t indexBufferLength;
        };

        std::vector<Element> m_Elements;
        std::vector<GLBufferPtr> m_VertexBuffers;
        std::vector<GLBufferPtr> m_IndexBuffers;

        void initFromMesh(const MeshPtr& mesh);
    };

    using GLMeshPtr = std::shared_ptr<GLMesh>;
}
