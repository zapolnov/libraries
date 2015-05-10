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
#include "math/Frustum.h"
#include "opengl/GLUniformSet.h"
#include "utility/debug.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <list>
#include <functional>
#include <memory>
#include <vector>

namespace Z
{
    class SceneNode;
    using SceneNodePtr = std::shared_ptr<SceneNode>;

    class SceneNode : public std::enable_shared_from_this<SceneNode>
    {
    public:
        SceneNode();
        virtual ~SceneNode();

        SceneNode* parent() const { return m_Parent; }

        const glm::vec3& position() const { return m_Position; }
        void setPosition(const glm::vec3& position) { m_Position = position; invalidate(); }

        const glm::quat& rotation() const { return m_Rotation; }
        void setRotation(const glm::quat& rotation) { m_Rotation = rotation; invalidate(); }

        const glm::vec3& scale() const { return m_Scale; }
        void setScale(const glm::vec3& scale) { m_Scale = scale; invalidate(); }

        const glm::mat4& localMatrix() const { Z_CHECK((m_Flags & LocalMatrixDirty) == 0); return m_LocalMatrix; }
        const glm::mat4& worldMatrix() const { Z_CHECK((m_Flags & WorldMatrixDirty) == 0); return m_WorldMatrix; }
        const glm::mat4& inverseWorldMatrix() const;

        void addChild(const SceneNodePtr& child);
        void removeFromParent();
        const std::list<SceneNodePtr>& children() const { return m_Children; }

        virtual void updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged);
        virtual void update(double time);
        virtual void draw(const Frustum& frustum, GLUniformSet* uniforms = nullptr) const;

        void setVisible(bool flag = true) { m_Flags = (flag ? m_Flags | Visible : m_Flags &~ Visible); }
        bool isVisible() const { return (m_Flags & Visible) != 0; }

        void setRenderable(bool flag = true) { m_Flags = (flag ? m_Flags | Renderable : m_Flags &~ Renderable); }
        bool isRenderable() const { return (m_Flags & Renderable) != 0; }

    protected:
        virtual bool isInsideFrustum(const Frustum& frustum) const;
        virtual void render(const Frustum& frustum, GLUniformSet* uniforms = nullptr) const;

        virtual void invalidate() { m_Flags |= LocalMatrixDirty | WorldMatrixDirty | InverseWorldMatrixDirty; }

    private:
        enum Flag {
            LocalMatrixDirty        = 0b000000000001,
            WorldMatrixDirty        = 0b000000000010,
            InverseWorldMatrixDirty = 0b000000000100,
            Visible                 = 0b000000001000,
            Renderable              = 0b000000010000,
        };

        SceneNode* m_Parent;
        std::list<SceneNodePtr> m_Children;
        std::list<SceneNodePtr>::iterator m_SelfRef;
        glm::vec3 m_Position;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;
        glm::mat4 m_LocalMatrix;
        glm::mat4 m_WorldMatrix;
        mutable std::unique_ptr<glm::mat4> m_InverseWorldMatrix;
        mutable int m_Flags;

        SceneNode(const SceneNode&) = delete;
        SceneNode& operator=(const SceneNode&) = delete;
    };
}
