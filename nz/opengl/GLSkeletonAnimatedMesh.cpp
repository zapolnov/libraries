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
        : GLMesh(manager)
    {
    }

    GLSkeletonAnimatedMesh::~GLSkeletonAnimatedMesh()
    {
        unload();
    }

    void GLSkeletonAnimatedMesh::unload()
    {
        GLMesh::unload();
        m_Animations.clear();
    }

    void GLSkeletonAnimatedMesh::render(size_t animationIndex, float animationTime) const
    {
        if (animationIndex >= m_Animations.size()) {
            Z_LOG("Mesh has no animation #" << animationIndex);
            return;
        }
        const SkeletonAnimationPtr& animation = m_Animations[animationIndex];

        std::vector<glm::mat4> boneMatrices;
        boneMatrices.reserve(animation->skeleton()->numBones());
        animation->getPoseForTime(animationTime, boneMatrices);

        if (!boneMatrices.empty()) {
            GL::Int program = gl::GetInteger(GL::CURRENT_PROGRAM);
            int uniform = gl::GetUniformLocation(program, "u_bones");
            gl::UniformMatrix4fv(uniform, boneMatrices.size(), GL::FALSE, &boneMatrices[0][0][0]);
        }

        GLMesh::render();
    }

    void GLSkeletonAnimatedMesh::initFromMesh(const MeshPtr& mesh)
    {
        Z_CHECK(mesh != nullptr);
        if (!mesh)
            return;

        GLMesh::initFromMesh(mesh);
        m_Animations = mesh->animations();
    }
}
