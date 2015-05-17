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
#include "MeshMaterial.h"
#include "Skeleton.h"
#include "SkeletonAnimation.h"
#include "VertexFormat.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace Z
{
    class Mesh
    {
    public:
        static const size_t MAX_BONES_PER_VERTEX = 4;

        struct Element
        {
            std::string name;
            MeshMaterialPtr material;
            size_t vertexBuffer;
            size_t indexBufferOffset;
            size_t indexBufferLength;
        };

        using VertexBuffer = std::vector<uint8_t>;
        using IndexBuffer = std::vector<uint16_t>;

        explicit Mesh(const VertexFormatPtr& format);
        ~Mesh();

        void setBaseDirectory(const std::string& directory) { m_BaseDirectory = directory; }
        const std::string& baseDirectory() const { return m_BaseDirectory; }

        std::vector<Element>& elements() { return m_Elements; }
        const std::vector<Element>& elements() const { return m_Elements; }

        const VertexFormatPtr& vertexFormat() const { return m_VertexFormat; }

        const std::vector<VertexBuffer>& vertexBuffers() const { return m_VertexBuffers; }
        void addVertexBuffer(const VertexBuffer& buffer) { m_VertexBuffers.emplace_back(buffer); }
        void addVertexBuffer(VertexBuffer&& buffer) { m_VertexBuffers.emplace_back(std::move(buffer)); }

        IndexBuffer& indexBuffer() { return m_IndexBuffer; }
        const IndexBuffer& indexBuffer() const { return m_IndexBuffer; }

        const SkeletonPtr& skeleton() const { return m_Skeleton; }
        void setSkeleton(const SkeletonPtr& skeleton) { m_Skeleton = skeleton; }
        void setSkeleton(SkeletonPtr&& skeleton) { m_Skeleton = std::move(skeleton); }

        size_t numAnimations() const { return m_Animations.size(); }
        const std::vector<SkeletonAnimationPtr>& animations() const { return m_Animations; }
        const SkeletonAnimationPtr& animation(size_t index) const;
        const SkeletonAnimationPtr& animation(const char* name) const;
        const SkeletonAnimationPtr& animation(const std::string& name) const;
        const SkeletonAnimationPtr& animation(std::string&& name) const;
        const SkeletonAnimationPtr& animation(const Utf8String& name) const;
        SkeletonAnimationPtr addAnimation(const Utf8String& name);

    private:
        std::string m_BaseDirectory;
        IndexBuffer m_IndexBuffer;
        std::vector<VertexBuffer> m_VertexBuffers;
        VertexFormatPtr m_VertexFormat;
        std::vector<SkeletonAnimationPtr> m_Animations;
        std::unordered_map<Utf8String, SkeletonAnimationPtr> m_AnimationsByName;
        std::vector<Element> m_Elements;
        SkeletonPtr m_Skeleton;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
