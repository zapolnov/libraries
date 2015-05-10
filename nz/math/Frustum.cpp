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
        l[0] = m[ 3] + m[ 0];
        l[1] = m[ 7] + m[ 4];
        l[2] = m[11] + m[ 8];
        l[3] = m[15] + m[12];
        l = l.normalize();

        auto& r = frustum.planes[Right];
        r[0] = m[ 3] - m[ 0];
        r[1] = m[ 7] - m[ 4];
        r[2] = m[11] - m[ 8];
        r[3] = m[15] - m[12];
        r = r.normalize();

        auto& t = frustum.planes[Top];
        t[0] = m[ 3] - m[ 1];
        t[1] = m[ 7] - m[ 5];
        t[2] = m[11] - m[ 9];
        t[3] = m[15] - m[13];
        t = t.normalize();

        auto& b = frustum.planes[Bottom];
        b[0] = m[ 3] + m[ 1];
        b[1] = m[ 7] + m[ 5];
        b[2] = m[11] + m[ 9];
        b[3] = m[15] + m[13];
        b = b.normalize();

        auto& f = frustum.planes[Far];
        f[0] = m[ 3] - m[ 2];
        f[1] = m[ 7] - m[ 6];
        f[2] = m[11] - m[10];
        f[3] = m[15] - m[14];
        f = f.normalize();

        auto& n = frustum.planes[Near];
        n[0] = m[ 3] + m[ 2];
        n[1] = m[ 7] + m[ 6];
        n[2] = m[11] + m[10];
        n[3] = m[15] + m[14];
        n = n.normalize();

        return frustum;
    }
}
