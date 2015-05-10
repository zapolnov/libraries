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
#include "SceneNode.h"
#include "opengl/GLMesh.h"

namespace Z
{
    class SceneMesh : public SceneNode
    {
    public:
        SceneMesh();
        explicit SceneMesh(const GLMeshPtr& mesh);
        explicit SceneMesh(GLMeshPtr&& mesh);
        ~SceneMesh();

        const GLMeshPtr& mesh() const { return m_Mesh; }
        void setMesh(const GLMeshPtr& mesh);
        void setMesh(GLMeshPtr&& mesh);

        void setAnimation(size_t index);

    protected:
        GLMeshPtr m_Mesh;
        float m_AnimationTime = 0.0f;
        size_t m_AnimationIndex = 0;

        void update(double time) override;
        bool isVisible(const Frustum& frustum) const override;
        void render(const Frustum& frustum, GLUniformSet* uniforms = nullptr) const override;
    };

    using SceneMeshPtr = std::shared_ptr<SceneMesh>;
}
