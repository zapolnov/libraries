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
#include "Frustum.h"

namespace Z
{
    Frustum Frustum::fromMatrix(const glm::mat4& matrix)
    {
        const float* m = &matrix[0][0];
        Frustum frustum;

        auto& l = frustum.planes[Left];
        l.normal.x = m[ 3] + m[ 0];
        l.normal.y = m[ 7] + m[ 4];
        l.normal.z = m[11] + m[ 8];
        l.distance = m[15] + m[12];
        l = l.normalize();

        auto& r = frustum.planes[Right];
        r.normal.x = m[ 3] - m[ 0];
        r.normal.y = m[ 7] - m[ 4];
        r.normal.z = m[11] - m[ 8];
        r.distance = m[15] - m[12];
        r = r.normalize();

        auto& t = frustum.planes[Top];
        t.normal.x = m[ 3] - m[ 1];
        t.normal.y = m[ 7] - m[ 5];
        t.normal.z = m[11] - m[ 9];
        t.distance = m[15] - m[13];
        t = t.normalize();

        auto& b = frustum.planes[Bottom];
        b.normal.x = m[ 3] + m[ 1];
        b.normal.y = m[ 7] + m[ 5];
        b.normal.z = m[11] + m[ 9];
        b.distance = m[15] + m[13];
        b = b.normalize();

        auto& f = frustum.planes[Far];
        f.normal.x = m[ 3] - m[ 2];
        f.normal.y = m[ 7] - m[ 6];
        f.normal.z = m[11] - m[10];
        f.distance = m[15] - m[14];
        f = f.normalize();

        auto& n = frustum.planes[Near];
        n.normal.x = m[ 3] + m[ 2];
        n.normal.y = m[ 7] + m[ 6];
        n.normal.z = m[11] + m[10];
        n.distance = m[15] + m[14];
        n = n.normalize();

        return frustum;
    }

    bool Frustum::intersectsPoint(const glm::vec3& point) const
    {
        for (size_t i = 0; i < NUM_PLANES; i++) {
            const auto& p = planes[i];
            if (glm::dot(p.normal, point) <= -p.distance)
                return false;
        }
        return true;
    }

    float Frustum::pointDistanceToNearPlane(const glm::vec3& point) const
    {
        float d = 0.0f;
        for (size_t i = 0; i < NUM_PLANES; i++) {
            const auto& p = planes[i];
            d = glm::dot(p.normal, point) + p.distance;
            if (d <= 0)
                return -1.0f;
        }
        return d;
    }

    bool Frustum::intersectsSphere(const Sphere& sphere) const
    {
        for (size_t i = 0; i < NUM_PLANES; i++) {
            const auto& p = planes[i];
            if (glm::dot(p.normal, sphere.center) + p.distance <= -sphere.radius)
                return false;
        }
        return true;
    }

    float Frustum::sphereCenterDistanceToNearPlane(const Sphere& sphere) const
    {
        float d = 0.0f;
        for (size_t i = 0; i < NUM_PLANES; i++) {
            const auto& p = planes[i];
            d = glm::dot(p.normal, sphere.center) + p.distance;
            if (d <= -sphere.radius)
                return -1.0f;
        }
        return d;
    }

    bool Frustum::intersectsAABox(const AABox& box) const
    {
        glm::vec3 boxCenter = box.center();
        glm::vec3 boxHalfSize = box.halfSize();

        for (size_t i = 0; i < NUM_PLANES; ++i) {
            const auto& p = planes[i];
            const auto& n = p.normal;

            float distanceToCenter = glm::dot(n, boxCenter) + p.distance;
            float maxDistance = fabs(n.x * boxHalfSize.x) + fabs(n.y * boxHalfSize.y) + fabs(n.z * boxHalfSize.z);
            if (distanceToCenter < -maxDistance)
                return false;
        }

        return true;
    }
}
