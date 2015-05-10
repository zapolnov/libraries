/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include "opengl.h"
#include "utility/debug.h"
#include "GLUniform.h"
#include "GLProgram.h"
#include <glm/glm.hpp>
#include <vector>
#include <utility>
#include <memory>

namespace Z
{
    class GLUniformSet
    {
    public:
        GLUniformSet();
        ~GLUniformSet();

        void upload(const GLProgramPtr& program) const;

        void setMatrix4fv(GLUniform uniform, const glm::mat4& matrix, GL::Boolean transpose = GL::FALSE);
        void setMatrix4fv(GLUniform uniform, glm::mat4&& matrix, GL::Boolean transpose = GL::FALSE);

        void unset(GLUniform uniform);

    private:
        struct Value;
        struct Matrix4;

        std::unique_ptr<Value> m_Values[size_t(GLUniform::NumStandardUniforms)];

        GLUniformSet(const GLUniformSet&) = delete;
        GLUniformSet& operator=(const GLUniformSet&) = delete;
    };
}
