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
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Z
{
    SceneCamera::SceneCamera()
        : m_Flags(ProjectionDirty | FrustumDirty)
    {
    }

    SceneCamera::~SceneCamera()
    {
    }

    void SceneCamera::updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged)
    {
        SceneNode::updateTransform(parentMatrix, parentMatrixChanged);

        if (parentMatrixChanged || (m_Flags & ProjectionDirty)) {
            m_Projection = glm::perspective(m_FieldOfView, m_Aspect, m_NearZ, m_FarZ);
            m_Flags = (m_Flags & ~ProjectionDirty) | FrustumDirty;
        }

        if (m_Flags & FrustumDirty) {
            m_Frustum = Frustum::fromMatrix(m_Projection * inverseWorldMatrix());
            m_Flags &= ~FrustumDirty;
        }
    }

    void SceneCamera::invalidate()
    {
        SceneNode::invalidate();
        m_Flags |= ProjectionDirty | FrustumDirty;
    }
}
