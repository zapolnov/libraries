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
#include "Sphere.h"
#include "AABox.h"
#include "Plane.h"
#include <glm/glm.hpp>

namespace Z
{
    class Frustum
    {
    public:
        enum {
            Left = 0,
            Right = 1,
            Top = 2,
            Bottom = 3,
            Far = 4,
            Near = 5,
        };

        static const size_t NUM_PLANES = 6;
        Plane planes[NUM_PLANES];

        Frustum() {}
        static Frustum fromMatrix(const glm::mat4& matrix);

        bool intersectsPoint(const glm::vec3& point) const;
        float pointDistanceToNearPlane(const glm::vec3& point) const;

        bool intersectsSphere(const Sphere& sphere) const;
        float sphereCenterDistanceToNearPlane(const Sphere& sphere) const;

        bool intersectsAABox(const AABox& box) const;
    };
}
