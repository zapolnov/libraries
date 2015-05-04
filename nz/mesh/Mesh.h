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
        static constexpr size_t MAX_BONES_PER_VERTEX = 4;

        struct BoneWeights
        {
            float boneWeight[MAX_BONES_PER_VERTEX];
            size_t boneIndex[MAX_BONES_PER_VERTEX];
        };

        struct Element
        {
            std::string name;
            MeshMaterialPtr material;
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec3> bitangents;
            std::vector<glm::vec2> texCoords;
            std::vector<BoneWeights> boneWeights;
            std::vector<uint16_t> indices;
        };

        using ElementPtr = std::shared_ptr<Element>;
        using ElementList = std::vector<ElementPtr>;

        Mesh();
        ~Mesh();

        ElementList& elements() { return m_Elements; }
        const ElementList& elements() const { return m_Elements; }

        const SkeletonPtr& skeleton() const { return m_Skeleton; }
        void setSkeleton(const SkeletonPtr& skeleton) { m_Skeleton = skeleton; }
        void setSkeleton(SkeletonPtr&& skeleton) { m_Skeleton = std::move(skeleton); }

        size_t numAnimations() const { return m_Animations.size(); }
        const SkeletonAnimationPtr& animation(size_t index) const;
        const SkeletonAnimationPtr& animation(const char* name) const;
        const SkeletonAnimationPtr& animation(const std::string& name) const;
        const SkeletonAnimationPtr& animation(std::string&& name) const;
        const SkeletonAnimationPtr& animation(const Utf8String& name) const;
        SkeletonAnimationPtr addAnimation(const Utf8String& name);

    private:
        std::vector<SkeletonAnimationPtr> m_Animations;
        std::unordered_map<Utf8String, SkeletonAnimationPtr> m_AnimationsByName;
        ElementList m_Elements;
        SkeletonPtr m_Skeleton;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    };

    using MeshPtr = std::shared_ptr<Mesh>;
}
