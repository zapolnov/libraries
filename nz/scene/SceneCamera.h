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
#include "SceneNode.h"

namespace Z
{
    class SceneCamera : public SceneNode
    {
    public:
        SceneCamera();
        ~SceneCamera();

        const Frustum& frustum() const { Z_CHECK((m_Flags & FrustumDirty) == 0); return m_Frustum; }
        const glm::mat4& projectionMatrix() const { Z_CHECK((m_Flags & ProjectionDirty) == 0); return m_Projection; }
        const glm::mat4& viewMatrix() const { return inverseWorldMatrix(); }

        glm::vec3 rightVector() const { return glm::vec3(viewMatrix()[0]); }
        glm::vec3 upVector() const { return glm::vec3(viewMatrix()[1]); }
        glm::vec3 forwardVector() const { return glm::vec3(viewMatrix()[2]); }

        float aspect() const { return m_Aspect; }
        void setAspect(float aspect) { m_Aspect = aspect; invalidateFrustum(); }

        float fieldOfView() const { return m_FieldOfView; }
        void setFieldOfView(float radians) { m_FieldOfView = radians; invalidateFrustum(); }

        float nearZ() const { return m_NearZ; }
        float farZ() const { return m_FarZ; }
        void setNearZ(float z) { m_NearZ = z; invalidateFrustum(); }
        void setFarZ(float z) { m_FarZ = z; invalidateFrustum(); }

    protected:
        enum {
            ProjectionDirty = 0b00000001,
            FrustumDirty    = 0b00000010,
        };

        Frustum m_Frustum;
        glm::mat4 m_Projection;
        float m_Aspect = 1.0f;
        float m_FieldOfView = 3.1415f / 2.0f;
        float m_NearZ = 1.0f;
        float m_FarZ = 100.0f;
        int m_Flags;

        void updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged) override;

        void invalidate() override;
        void invalidateFrustum() { m_Flags |= ProjectionDirty | FrustumDirty; }
    };

    using SceneCameraPtr = std::shared_ptr<SceneCamera>;
}
