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
#include "GLUniformSet.h"

namespace Z
{
    struct GLUniformSet::Value
    {
        virtual void bind(int handle) const = 0;
    };

    struct GLUniformSet::Matrix4 : public Value
    {
        std::vector<glm::mat4> matrix;
        GL::Boolean transpose;

        void bind(int handle) const override {
            gl::UniformMatrix4fv(handle, matrix.size(), transpose, &matrix[0][0][0]);
        }
    };


    GLUniformSet::GLUniformSet()
    {
    }

    GLUniformSet::~GLUniformSet()
    {
    }

    void GLUniformSet::upload(const GLProgramPtr& program) const
    {
        for (size_t i = 0; i < size_t(GLUniform::NumStandardUniforms); i++) {
            if (m_Values[i]) {
                int handle = program->getUniformLocation(GLUniform(i));
                if (handle != -1)
                    m_Values[i]->bind(handle);
            }
        }
    }

    void GLUniformSet::setMatrix4fv(GLUniform uniform, const glm::mat4& matrix, GL::Boolean transpose)
    {
        Z_ASSERT(int(uniform) >= 0 && int(uniform) < int(GLUniform::NumStandardUniforms));

        std::unique_ptr<Matrix4> value(new Matrix4);
        value->matrix.emplace_back(matrix);
        value->transpose = transpose;
        m_Values[size_t(uniform)] = std::move(value);
    }

    void GLUniformSet::setMatrix4fv(GLUniform uniform, glm::mat4&& matrix, GL::Boolean transpose)
    {
        Z_ASSERT(int(uniform) >= 0 && int(uniform) < int(GLUniform::NumStandardUniforms));

        std::unique_ptr<Matrix4> value(new Matrix4);
        value->matrix.emplace_back(std::move(matrix));
        value->transpose = transpose;
        m_Values[size_t(uniform)] = std::move(value);
    }

    void GLUniformSet::unset(GLUniform uniform)
    {
        Z_ASSERT(int(uniform) >= 0 && int(uniform) < int(GLUniform::NumStandardUniforms));
        m_Values[size_t(uniform)].reset();
    }
}
