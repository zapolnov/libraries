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
#include <glm/glm.hpp>
#include <memory>

namespace Z
{
    class SceneElement
    {
    public:
        SceneElement();
        virtual ~SceneElement();

        SceneElement* parent() const { return m_Parent; }
        SceneElement* firstChild() const { return m_FirstChild; }
        SceneElement* lastChild() const { return m_LastChild; }
        SceneElement* nextSibling() const { return m_NextSibling; }
        SceneElement* prevSibling() const { return m_PrevSibling; }

        void addChild(SceneElement* child);
        void removeFromParent();

        const glm::mat4& localMatrix() const;
        const glm::mat4& worldMatrix() const;

    protected:
        void setLocalMatrixDirty();
        void setWorldMatrixDirty();

        virtual void calculateLocalMatrix(glm::mat4& matrix) const;
        virtual void calculateWorldMatrix(glm::mat4& matrix) const;

        virtual void onAddedToParent(SceneElement* newParent) { (void)newParent; }
        virtual void onRemovedFromParent(SceneElement* oldParent) { (void)oldParent; }
        virtual void onChildAdded(SceneElement* child) { (void)child; }
        virtual void onChildRemoved(SceneElement* child) { (void)child; }

    private:
        enum Flags
        {
            LocalMatrixDirty = 0b00000000001,
            WorldMatrixDirty = 0b00000000010,
        };

        mutable glm::mat4 m_LocalMatrix;
        mutable glm::mat4 m_WorldMatrix;
        SceneElement* m_Parent;
        SceneElement* m_FirstChild;
        SceneElement* m_LastChild;
        SceneElement* m_NextSibling;
        SceneElement* m_PrevSibling;
        mutable int m_Flags;

        void addToParent();
    };
}
