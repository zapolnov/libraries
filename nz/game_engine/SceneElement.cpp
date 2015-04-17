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
#include "SceneElement.h"
#include "utility/debug.h"

namespace Z
{
    SceneElement::SceneElement()
        : m_Parent(nullptr)
        , m_FirstChild(nullptr)
        , m_LastChild(nullptr)
        , m_NextSibling(nullptr)
        , m_PrevSibling(nullptr)
        , m_Flags(LocalMatrixDirty | WorldMatrixDirty)
    {
    }

    SceneElement::~SceneElement()
    {
        while (m_FirstChild) {
            auto child = m_FirstChild;
            child->removeFromParent();
            delete child;
        }

        removeFromParent();
    }

    void SceneElement::addChild(SceneElement* child)
    {
        Z_CHECK(child != nullptr);
        if (child) {
            Z_CHECK(child->m_Parent == nullptr);
            child->removeFromParent();
            Z_ASSERT(child->m_Parent == nullptr);
            child->m_Parent = this;
            child->addToParent();
        }
    }

    void SceneElement::removeFromParent()
    {
        if (m_Parent) {
            SceneElement* oldParent = m_Parent;

            if (m_PrevSibling)
                m_PrevSibling->m_NextSibling = m_NextSibling;
            else {
                Z_ASSERT(m_Parent->m_FirstChild == this);
                m_Parent->m_FirstChild = m_NextSibling;
            }

            if (m_NextSibling)
                m_NextSibling->m_PrevSibling = m_PrevSibling;
            else {
                Z_ASSERT(m_Parent->m_LastChild == this);
                m_Parent->m_LastChild = m_PrevSibling;
            }

            m_NextSibling = nullptr;
            m_PrevSibling = nullptr;
            m_Parent = nullptr;

            oldParent->onChildRemoved(this);
            onRemovedFromParent(oldParent);
            setWorldMatrixDirty();
        }
    }

    const glm::mat4& SceneElement::localMatrix() const
    {
        if (m_Flags & LocalMatrixDirty) {
            calculateLocalMatrix(m_LocalMatrix);
            m_Flags &= ~LocalMatrixDirty;
        }
        return m_LocalMatrix;
    }

    const glm::mat4& SceneElement::worldMatrix() const
    {
        if (m_Flags & WorldMatrixDirty) {
            calculateWorldMatrix(m_WorldMatrix);
            m_Flags &= ~WorldMatrixDirty;
        }
        return m_WorldMatrix;
    }

    void SceneElement::setLocalMatrixDirty()
    {
        m_Flags |= LocalMatrixDirty;
    }

    void SceneElement::setWorldMatrixDirty()
    {
        m_Flags |= WorldMatrixDirty;

        for (SceneElement* child = m_FirstChild; child; child = child->m_NextSibling)
            child->setWorldMatrixDirty();
    }

    void SceneElement::calculateLocalMatrix(glm::mat4& matrix) const
    {
        matrix = glm::mat4(1.0f);
    }

    void SceneElement::calculateWorldMatrix(glm::mat4& matrix) const
    {
        if (!m_Parent)
            matrix = localMatrix();
        else
            matrix = m_Parent->worldMatrix() * localMatrix();
    }

    void SceneElement::addToParent()
    {
        if (m_Parent) {
            if (m_Parent->m_LastChild) {
                m_PrevSibling = m_Parent->m_LastChild;
                m_Parent->m_LastChild->m_NextSibling = this;
            } else {
                Z_ASSERT(m_Parent->m_FirstChild == nullptr);
                m_Parent->m_FirstChild = this;
            }
            m_Parent->m_LastChild = this;

            m_Parent->onChildAdded(this);
            onAddedToParent(m_Parent);
            setWorldMatrixDirty();
        }
    }
}
