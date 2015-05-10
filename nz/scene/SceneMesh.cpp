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
#include "SceneMesh.h"

namespace Z
{
    SceneMesh::SceneMesh()
    {
    }

    SceneMesh::SceneMesh(const GLMeshPtr& mesh)
        : m_Mesh(mesh)
    {
    }

    SceneMesh::SceneMesh(GLMeshPtr&& mesh)
        : m_Mesh(std::move(mesh))
    {
    }

    SceneMesh::~SceneMesh()
    {
    }

    void SceneMesh::setMesh(const GLMeshPtr& mesh)
    {
        if (m_Mesh != mesh)
            m_Mesh = mesh;
    }

    void SceneMesh::setMesh(GLMeshPtr&& mesh)
    {
        if (m_Mesh != mesh)
            m_Mesh = std::move(mesh);
    }

    void SceneMesh::setAnimation(size_t index)
    {
        if (m_AnimationIndex != index) {
            m_AnimationIndex = index;
            m_AnimationTime = 0.0f;
        }
    }

    void SceneMesh::update(double time)
    {
        m_AnimationTime += float(time);
        SceneNode::update(time);
    }

    bool SceneMesh::isVisible(const Frustum&) const
    {
        // FIXME
        return true;
    }

    void SceneMesh::render(const Frustum& frustum, GLUniformSet* uniforms) const
    {
        if (m_Mesh) {
            uniforms->setMatrix4fv(GLUniform::ModelMatrix, worldMatrix());
            m_Mesh->render(m_AnimationIndex, m_AnimationTime, uniforms);
        }
    }
}
