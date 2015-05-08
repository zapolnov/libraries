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
#include "SkeletonAnimation.h"
#include "utility/debug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Z
{
    static const glm::mat4 g_IdentityMatrix = glm::mat4(1.0f);
    static const SkeletonAnimation::Channel g_DummyChannel = { 0, 0, 0, 0, 0, 0 };

    template <class T> T getInterpolatedValue(float time, float duration, const std::vector<T>& keys, size_t keysOffset,
        size_t keysLength, const T& defaultValue, const std::function<T(const T&, const T&, float)>& interpolate)
    {
        if (keysLength < 2) {
            if (keysLength == 0)
                return defaultValue;
            return keys[keysOffset];
        }

        typename std::vector<T>::const_iterator begin = keys.begin() + keysOffset;
        typename std::vector<T>::const_iterator end = begin + keysLength;
        typename std::vector<T>::const_iterator key1;
        typename std::vector<T>::const_iterator key2 = std::lower_bound(begin, end, time,
            [](const T& value, float time) -> bool { return value.time < time; });

        if (key2 != end && key2->time == time)
            return *key2;

        float key1Time;
        float key2Time;
        if (key2 != begin && key2 != end) {
            key1 = key2 - 1;
            key1Time = key1->time;
            key2Time = key2->time;
        } else {
            key1 = end - 1;
            if (key2 == begin) {
                key1Time = -(duration - key1->time);
                Z_CHECK(key1Time <= 0.0f);
                key2Time = key2->time;
            } else {
                key2 = begin;
                key1Time = key1->time;
                key2Time = duration;
            }
        }

        float timeDelta = key2Time - key1Time;
        float factor = (timeDelta != 0.0f ? (time - key1Time) / timeDelta : 0.0f);
        Z_CHECK(factor >= 0.0f && factor <= 1.0f);

        T value = interpolate(*key1, *key2, factor);
        value.time = time;

        return value;
    }

    SkeletonAnimation::SkeletonAnimation(const SkeletonPtr& skeleton, const Utf8String& name)
        : m_Skeleton(skeleton)
        , m_Name(name)
    {
    }

    SkeletonAnimation::~SkeletonAnimation()
    {
    }

    SkeletonAnimation::Channel& SkeletonAnimation::addChannel(size_t boneIndex, size_t numPositionKeys,
        size_t numScaleKeys, size_t numRotationKeys)
    {
        if (boneIndex >= m_Channels.size())
            m_Channels.resize(boneIndex + 1, g_DummyChannel);

        Channel& channel = m_Channels[boneIndex];
        channel.positionKeysOffset = m_PositionKeys.size();
        channel.positionKeysLength = numPositionKeys;
        channel.scaleKeysOffset = m_ScaleKeys.size();
        channel.scaleKeysLength = numScaleKeys;
        channel.rotationKeysOffset = m_RotationKeys.size();
        channel.rotationKeysLength = numRotationKeys;

        m_PositionKeys.resize(channel.positionKeysOffset + numPositionKeys);
        m_ScaleKeys.resize(channel.scaleKeysOffset + numScaleKeys);
        m_RotationKeys.resize(channel.rotationKeysOffset + numRotationKeys);

        return channel;
    }

    void SkeletonAnimation::getPoseForTime(float time, std::vector<glm::mat4>& boneMatrices) const
    {
        Z_CHECK(m_Duration > 0.0);
        if (m_Duration <= 0.0) {
            boneMatrices.clear();
            return;
        }

        float durationInTicks = m_Duration;
        float ticksPerSecond = float(m_TicksPerSecond > 0.0 ? m_TicksPerSecond : 25.0);
        float timeInTicks = fmod(time * m_TicksPerSecond, durationInTicks);

        size_t numBones = m_Skeleton->numBones();
        boneMatrices.resize(numBones);

        for (size_t boneIndex = 0; boneIndex < numBones; boneIndex++) {
            const Channel& channel = (boneIndex < m_Channels.size() ? m_Channels[boneIndex] : g_DummyChannel);
            glm::mat4 transform;

            if (channel.positionKeysLength == 0 && channel.scaleKeysLength == 0 && channel.rotationKeysLength == 0)
                transform = g_IdentityMatrix;
            else {
                PositionKey pos = getInterpolatedValue<PositionKey>(timeInTicks, durationInTicks, m_PositionKeys,
                    channel.positionKeysOffset, channel.positionKeysLength, { timeInTicks, glm::vec3(0.0f) },
                    [](const PositionKey& key1, const PositionKey& key2, float factor) -> PositionKey {
                        PositionKey r;
                        r.position = key1.position + (key2.position - key1.position) * factor;
                        return r;
                    });

                ScaleKey scale = getInterpolatedValue<ScaleKey>(timeInTicks, durationInTicks, m_ScaleKeys,
                    channel.scaleKeysOffset, channel.scaleKeysLength, { timeInTicks, glm::vec3(1.0f) },
                    [](const ScaleKey& key1, const ScaleKey& key2, float factor) -> ScaleKey {
                        ScaleKey r;
                        r.scale = key1.scale + (key2.scale - key1.scale) * factor;
                        return r;
                    });

                RotationKey rotation = getInterpolatedValue<RotationKey>(timeInTicks, durationInTicks, m_RotationKeys,
                    channel.rotationKeysOffset, channel.rotationKeysLength, { timeInTicks, glm::quat() },
                    [](const RotationKey& key1, const RotationKey& key2, float factor) -> RotationKey {
                        RotationKey r;
                        r.rotation = glm::slerp(key1.rotation, key2.rotation, factor);
                        return r;
                    });

                transform = glm::translate(g_IdentityMatrix, pos.position);
                transform *= glm::mat4_cast(rotation.rotation);
                transform = glm::scale(transform, scale.scale);
            }

            size_t parentBoneIndex = m_Skeleton->bone(boneIndex).parentIndex;
            if (parentBoneIndex == size_t(-1)) {
                transform = m_Skeleton->globalInverseTransform() * transform;
            } else {
                Z_CHECK(parentBoneIndex < boneIndex);
                transform = boneMatrices[parentBoneIndex] * transform;
            }

            boneMatrices[boneIndex] = transform;
        }

        for (size_t boneIndex = 0; boneIndex < numBones; boneIndex++)
            boneMatrices[boneIndex] *= m_Skeleton->bone(boneIndex).matrix();
    }
}
