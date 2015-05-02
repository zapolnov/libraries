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
#include "GLShader.h"
#include "error.h"
#include "utility/debug.h"

namespace Z
{
    GLShader::GLShader(GL::Enum type)
        : m_Type(type)
    {
        m_Handle = gl::CreateShader(type);
    }

    GLShader::~GLShader()
    {
        gl::DeleteShader(m_Handle);
    }

    void GLShader::setSource(const std::vector<std::string>& source)
    {
        std::vector<const GL::Char*> lines;
        std::vector<GL::Int> lengths;

        size_t count = source.size();
        lines.reserve(count);
        lengths.reserve(count);

        for (const auto& it : source) {
            lengths.push_back(GL::Int(it.length()));
            lines.push_back(reinterpret_cast<const GL::Char*>(it.c_str()));
        }

        gl::ShaderSource(m_Handle, GL::Sizei(count), lines.data(), lengths.data());
    }

    bool GLShader::compile()
    {
        gl::CompileShader(m_Handle);

        GL::Int success = GL::FALSE;
        gl::GetShaderiv(m_Handle, GL::COMPILE_STATUS, &success);

        if (!success) {
            GL::Int logLength = 0;
            gl::GetShaderiv(m_Handle, GL::INFO_LOG_LENGTH, &logLength);

            if (logLength > 0) {
                std::vector<GL::Char> data;
                data.resize(size_t(logLength));
                gl::GetShaderInfoLog(m_Handle, logLength, &logLength, data.data());
                data.resize(size_t(logLength));
                if (logLength > 0)
                    Z_LOG("Unable to compile shader (" << m_Type << ").\n" << std::string(data.data(), data.size()));
            }

            if (logLength <= 0)
                Z_LOG("Unable to compile shader (" << m_Type << "). Info log is not available.");
        }

        return success;
    }
}
