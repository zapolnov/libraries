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
#include "utility/Utf8String.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Z
{
    class Skeleton
    {
    public:
        struct Bone
        {
            Skeleton* const skeleton;
            const Utf8String name;
            const size_t index;
            size_t parentIndex = size_t(-1);

            Bone(Skeleton* s, const Utf8String& n, size_t i) : skeleton(s), name(n), index(i) {}

            const glm::mat4& matrix() const { return skeleton->m_BoneMatrices[index]; }
            glm::mat4& matrix() { return skeleton->m_BoneMatrices[index]; }
        };

        Skeleton();
        ~Skeleton();

        glm::mat4& globalInverseTransform() { return m_GlobalInverseTransform; }
        const glm::mat4& globalInverseTransform() const { return m_GlobalInverseTransform; }

        size_t numBones() const { return m_Bones.size(); }

        const Bone& bone(size_t index) const;
        const Bone* bone(const char* name) const { return bone(Utf8String::fromRawBytes(name)); }
        const Bone* bone(const std::string& name) const { return bone(Utf8String::fromRawBytes(name)); }
        const Bone* bone(std::string&& name) const { return bone(Utf8String::fromRawBytes(std::move(name))); }
        const Bone* bone(const Utf8String& name) const;

        Bone& getOrAddBone(const char* name) { return getOrAddBone(Utf8String::fromRawBytes(name)); }
        Bone& getOrAddBone(const std::string& name) { return getOrAddBone(Utf8String::fromRawBytes(name)); }
        Bone& getOrAddBone(std::string&& name) { return getOrAddBone(Utf8String::fromRawBytes(std::move(name))); }
        Bone& getOrAddBone(const Utf8String& name);

    private:
        std::vector<Bone> m_Bones;
        std::unordered_map<Utf8String, size_t> m_BonesByName;
        std::vector<glm::mat4> m_BoneMatrices;
        glm::mat4 m_GlobalInverseTransform;

        Skeleton(const Skeleton&) = delete;
        Skeleton& operator=(const Skeleton&) = delete;

        friend class Bone;
    };

    using SkeletonPtr = std::shared_ptr<Skeleton>;
}
