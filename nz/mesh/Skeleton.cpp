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
#include "Skeleton.h"

namespace Z
{
    Skeleton::Skeleton()
    {
    }

    Skeleton::~Skeleton()
    {
    }

    const Skeleton::Bone* Skeleton::getBone(const Utf8String& name) const
    {
        auto it = m_BonesByName.find(name);
        return (it != m_BonesByName.end() ? &m_Bones[it->second] : nullptr);
    }

    Skeleton::Bone& Skeleton::getOrAddBone(const Utf8String& name)
    {
        auto it = m_BonesByName.lower_bound(name);
        if (it == m_BonesByName.end() || it->first != name) {
            size_t boneIndex = m_Bones.size();

            m_Bones.emplace_back(this, name, boneIndex);
            m_BoneMatrices.emplace_back(1.0f);

            it = m_BonesByName.insert(it, std::make_pair(name, boneIndex));
        }
        return m_Bones[it->second];
    }
}
