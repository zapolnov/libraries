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
#include "Skeleton.h"
#include "utility/Utf8String.h"
#include <map>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

namespace Z
{
    class SkeletonAnimation
    {
    public:
        struct PositionKey
        {
            float time;
            glm::vec3 position;
        };

        struct ScaleKey
        {
            float time;
            glm::vec3 scale;
        };

        struct RotationKey
        {
            float time;
            glm::quat rotation;
        };

        struct Channel
        {
            size_t positionKeysOffset;
            size_t positionKeysLength;
            size_t scaleKeysOffset;
            size_t scaleKeysLength;
            size_t rotationKeysOffset;
            size_t rotationKeysLength;
        };

        SkeletonAnimation(const SkeletonPtr& skeleton, const Utf8String& name);
        ~SkeletonAnimation();

        const SkeletonPtr& skeleton() const { return m_Skeleton; }
        const Utf8String& name() const { return m_Name; }

        float durationInTicks() const { return m_Duration; }
        void setDurationInTicks(float duration) { m_Duration = duration; }

        float ticksPerSecond() const { return m_TicksPerSecond; }
        void setTicksPerSecond(float ticks) { m_TicksPerSecond = ticks; }

        Channel& addChannel(size_t boneIndex, size_t numPositionKeys, size_t numScaleKeys,
            size_t numRotationKeys);

        const std::vector<PositionKey>& positionKeys() const { return m_PositionKeys; }
        std::vector<PositionKey>& positionKeys() { return m_PositionKeys; }

        const std::vector<ScaleKey>& scaleKeys() const { return m_ScaleKeys; }
        std::vector<ScaleKey>& scaleKeys() { return m_ScaleKeys; }

        const std::vector<RotationKey>& rotationKeys() const { return m_RotationKeys; }
        std::vector<RotationKey>& rotationKeys() { return m_RotationKeys; }

        void getPoseForTime(float time, const glm::mat4& globalInverseTransform,
            std::vector<glm::mat4>& boneMatrices) const;

    private:
        SkeletonPtr m_Skeleton;
        Utf8String m_Name;
        float m_TicksPerSecond = 0.0;
        float m_Duration = 0.0;
        std::vector<Channel> m_Channels;
        std::vector<PositionKey> m_PositionKeys;
        std::vector<ScaleKey> m_ScaleKeys;
        std::vector<RotationKey> m_RotationKeys;

        SkeletonAnimation(const SkeletonAnimation&) = delete;
        SkeletonAnimation& operator=(const SkeletonAnimation&) = delete;
    };

    using SkeletonAnimationPtr = std::shared_ptr<SkeletonAnimation>;
}
