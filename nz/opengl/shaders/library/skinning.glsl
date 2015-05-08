//
// Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

%vertex

#ifndef Z_MAX_BONES
#define Z_MAX_BONES 113     // FIXME
#endif

#ifdef Z_COMPACT_MATRIX_PALETTE
%include mat4_transpose.glsl
#endif

#ifdef Z_COMPACT_MATRIX_PALETTE
uniform vec4 u_bones[Z_MAX_BONES * 3];
#else
uniform mat4 u_bones[Z_MAX_BONES];
#endif

mat4 getBone(in float idx)
{
    int index = int(idx);
  #ifdef Z_COMPACT_MATRIX_PALETTE
    return mat4_transpose(mat4(
        u_bones[index * 3 + 0],
        u_bones[index * 3 + 1],
        u_bones[index * 3 + 2],
        vec4(0, 0, 0, 1)
    ));
  #else
    return u_bones[index];
  #endif
}


attribute vec4 a_bones;
attribute vec4 a_weights;

void applySkinning(inout vec4 position)
{
    mat4 boneTransform = getBone(a_bones[0]) * a_weights[0];
    boneTransform += getBone(a_bones[1]) * a_weights[1];
    boneTransform += getBone(a_bones[2]) * a_weights[2];
    boneTransform += getBone(a_bones[3]) * a_weights[3];
    position = boneTransform * position;
}
