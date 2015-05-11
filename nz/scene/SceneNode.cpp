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
#include "SceneNode.h"
#include "utility/debug.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Z
{
    static const glm::mat4 g_IdentityMatrix = glm::mat4(1.0f);

    SceneNode::SceneNode()
        : m_Parent(nullptr)
        , m_Position(0.0f)
        , m_Rotation()
        , m_Scale(1.0f)
        , m_Flags(LocalMatrixDirty | WorldMatrixDirty | InverseWorldMatrixDirty | Visible | Renderable)
    {
    }

    SceneNode::~SceneNode()
    {
        for (const auto& children : m_Children) {
            Z_CHECK(children->m_Parent == this);
            children->m_Parent = nullptr;
        }
    }

    const glm::mat4& SceneNode::inverseWorldMatrix() const
    {
        Z_CHECK((m_Flags & WorldMatrixDirty) == 0);

        if (!m_InverseWorldMatrix)
            m_InverseWorldMatrix.reset(new glm::mat4);

        if (m_Flags & InverseWorldMatrixDirty) {
            /*
            glm::vec3 scale;
            glm::quat orientation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            if (!glm::decompose(m_WorldMatrix, scale, orientation, translation, skew, perspective))
                *m_InverseWorldMatrix = glm::inverse(m_WorldMatrix);
            else {
                glm::mat4 m = glm::translate(g_IdentityMatrix, -translation);
                m *= glm::mat4_cast(glm::inverse(orientation));
                m = glm::scale(m, 1.0f / scale);
                *m_InverseWorldMatrix = std::move(m);
            }
            */
            *m_InverseWorldMatrix = glm::inverse(m_WorldMatrix);
            m_Flags &= ~InverseWorldMatrixDirty;
        }

        return *m_InverseWorldMatrix;
    }

    void SceneNode::addChild(const SceneNodePtr& child)
    {
        Z_CHECK(child != nullptr);
        if (!child)
            return;

        Z_CHECK(child->m_Parent == nullptr);
        child->removeFromParent();

        child->m_SelfRef = m_Children.insert(m_Children.end(), child);
        child->m_Parent = this;
    }

    void SceneNode::removeFromParent()
    {
        if (m_Parent) {
            m_Parent->m_Children.erase(m_SelfRef);
            m_Parent = nullptr;
        }
    }

    void SceneNode::updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged)
    {
        if (m_Flags & LocalMatrixDirty) {
            m_LocalMatrix = glm::translate(g_IdentityMatrix, m_Position);
            m_LocalMatrix *= glm::mat4_cast(m_Rotation);
            m_LocalMatrix = glm::scale(m_LocalMatrix, m_Scale);
            m_Flags = (m_Flags & ~LocalMatrixDirty) | WorldMatrixDirty | InverseWorldMatrixDirty;
            parentMatrixChanged = true;
        }

        if (parentMatrixChanged || (m_Flags & WorldMatrixDirty)) {
            Z_CHECK((m_Flags & LocalMatrixDirty) == 0);
            m_WorldMatrix = parentMatrix * m_LocalMatrix;
            m_Flags = (m_Flags & ~WorldMatrixDirty) | InverseWorldMatrixDirty;
            parentMatrixChanged = true;
        }

        for (const auto& child : m_Children)
            child->updateTransform(m_WorldMatrix, parentMatrixChanged);
    }

    void SceneNode::update(double time)
    {
        for (const auto& child : m_Children)
            child->update(time);
    }

    void SceneNode::draw(const Frustum& frustum, GLUniformSet* uniforms) const
    {
        Z_CHECK((m_Flags & (LocalMatrixDirty | WorldMatrixDirty)) == 0);

        if (isInsideFrustum(frustum)) {
            if (m_Flags & Visible) {
                if (m_Flags & Renderable)
                    render(frustum, uniforms);
                for (const auto& child : m_Children)
                    child->draw(frustum, uniforms);
            }
        }
    }

    bool SceneNode::isInsideFrustum(const Frustum&) const
    {
        return true;
    }

    void SceneNode::render(const Frustum&, GLUniformSet*) const
    {
    }
}
