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
#include "Mesh.h"
#include "utility/debug.h"

namespace Z
{
    static SkeletonAnimationPtr g_NullAnimation;

    Mesh::Mesh()
    {
    }

    Mesh::~Mesh()
    {
    }

    const SkeletonAnimationPtr& Mesh::animation(size_t index) const
    {
        if (index >= m_Animations.size())
            return g_NullAnimation;
        return m_Animations[index];
    }

    const SkeletonAnimationPtr& Mesh::animation(const char* name) const
    {
        return animation(Utf8String::fromRawBytes(name));
    }

    const SkeletonAnimationPtr& Mesh::animation(const std::string& name) const
    {
        return animation(Utf8String::fromRawBytes(name));
    }

    const SkeletonAnimationPtr& Mesh::animation(std::string&& name) const
    {
        return animation(Utf8String::fromRawBytes(std::move(name)));
    }

    const SkeletonAnimationPtr& Mesh::animation(const Utf8String& name) const
    {
        auto it = m_AnimationsByName.find(name);
        if (it == m_AnimationsByName.end())
            return g_NullAnimation;
        return it->second;
    }

    SkeletonAnimationPtr Mesh::addAnimation(const Utf8String& name)
    {
        Z_ASSERT(m_Skeleton != nullptr);

        SkeletonAnimationPtr animation = std::make_shared<SkeletonAnimation>(m_Skeleton, name);
        m_Animations.push_back(animation);
        m_AnimationsByName.insert(std::make_pair(name, animation));

        return animation;
    }
}
