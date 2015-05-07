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
#include "GLProgram.h"
#include "GLShader.h"
#include "GLAttribute.h"
#include "utility/debug.h"
#include <vector>

namespace Z
{
    GLProgram::GLProgram(GLResourceManager* manager)
        : GLResource(manager)
    {
        for (int i = 0; i < int(GLUniform::NumStandardUniforms); i++)
            m_UniformHandles[i] = -1;
    }

    GLProgram::~GLProgram()
    {
        unload();
    }

    void GLProgram::use()
    {
        gl::UseProgram(m_Handle);
    }

    void GLProgram::reload()
    {
        unload();
        m_Handle = gl::CreateProgram();
    }

    void GLProgram::unload()
    {
        if (m_Handle != 0) {
            gl::DeleteProgram(m_Handle);
            m_Handle = 0;
        }
    }

    void GLProgram::bindAttribLocations()
    {
        for (const auto& attr : allGLAttributes())
            bindAttribLocation(attr.first, attr.second);
    }

    bool GLProgram::load(InputStream* input)
    {
        std::vector<std::string>* what = nullptr;
        std::vector<std::string> vertex;
        std::vector<std::string> fragment;
        bool success = true;
        int lineNumber = 0;

        Z_CHECK(input != nullptr);
        if (!input)
            return false;

        Z_LOG("Loading OpenGL program \"" << input->name() << "\".");

        static const std::string VERTEX = "%vertex";
        static const std::string VERTEX_LF = "%vertex\n";
        static const std::string FRAGMENT = "%fragment";
        static const std::string FRAGMENT_LF = "%fragment\n";
        static const std::string LF = "\n";

        while (!input->atEnd()) {
            std::string line = input->readLine(true);
            ++lineNumber;

            if (*line.c_str() == '%') {
                if (line == VERTEX_LF || line == VERTEX)
                    what = &vertex;
                else if (line == FRAGMENT_LF || line == FRAGMENT)
                    what = &fragment;
                else {
                    Z_LOG(input->name() << "(" << lineNumber << "): invalid directive.");
                    what = nullptr;
                }
                vertex.emplace_back(LF);
                fragment.emplace_back(LF);
                continue;
            }

            if (what != &vertex)
                vertex.emplace_back(LF);
            if (what != &fragment)
                fragment.emplace_back(LF);

            if (what)
                what->emplace_back(std::move(line));
            else if (line.length() > 0 && line != LF)
                Z_LOG(input->name() << "(" << lineNumber << "): ignored non-empty line.");
        }

        GLShader vertexShader(GL::VERTEX_SHADER);
        vertexShader.setSource(vertex);
        success = vertexShader.compile() && success;
        gl::AttachShader(m_Handle, vertexShader.handle());

        GLShader fragmentShader(GL::FRAGMENT_SHADER);
        fragmentShader.setSource(fragment);
        success = fragmentShader.compile() && success;
        gl::AttachShader(m_Handle, fragmentShader.handle());

        bindAttribLocations();

        return success && link() && enumerateStandardUniforms();
    }

    bool GLProgram::loadSource(const std::string& vertex, const std::string& fragment)
    {
        bool success = true;

        GLShader vertexShader(GL::VERTEX_SHADER);
        vertexShader.setSource(vertex);
        success = vertexShader.compile() && success;
        gl::AttachShader(m_Handle, vertexShader.handle());

        GLShader fragmentShader(GL::FRAGMENT_SHADER);
        fragmentShader.setSource(fragment);
        success = fragmentShader.compile() && success;
        gl::AttachShader(m_Handle, fragmentShader.handle());

        bindAttribLocations();

        return success && link() && enumerateStandardUniforms();
    }

    bool GLProgram::link()
    {
        gl::LinkProgram(m_Handle);

        GL::Int success = GL::FALSE;
        gl::GetProgramiv(m_Handle, GL::LINK_STATUS, &success);

        if (!success) {
            GL::Int logLength = 0;
            gl::GetProgramiv(m_Handle, GL::INFO_LOG_LENGTH, &logLength);

            if (logLength > 0) {
                std::vector<GL::Char> data;
                data.resize(size_t(logLength));
                gl::GetProgramInfoLog(m_Handle, logLength, &logLength, data.data());
                data.resize(size_t(logLength));
                if (logLength > 0) {
                    Z_LOG("Unable to link program.\n"
                        << std::string(data.data(), data.size()));
                }
            }

            if (logLength <= 0)
                Z_LOG("Unable to link program. Info log is not available.");

            return false;
        }

        return true;
    }

    bool GLProgram::enumerateStandardUniforms()
    {
        GL::Int numUniforms = 0;
        gl::GetProgramiv(m_Handle, GL::ACTIVE_UNIFORMS, &numUniforms);

        GL::Int maxNameLength = 0;
        gl::GetProgramiv(m_Handle, GL::ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

        if (numUniforms == 0 || maxNameLength == 0)
            return true;

        std::vector<GL::Char> buf(maxNameLength + 1);
        for (GL::UInt i = 0; i < GL::UInt(numUniforms); i++) {
            GL::Sizei nameLength = 0;
            GL::Int size = 0;
            GL::Enum type = GL::NONE;
            gl::GetActiveUniform(m_Handle, i, maxNameLength, &nameLength, &size, &type, buf.data());

            if (nameLength > 0) {
                std::string name(buf.data(), nameLength);

                size_t index = name.find('[');
                if (index != std::string::npos)
                    name.resize(index);

                GLUniform id = nameToGLUniform(name);
                if (id != GLUniform::NumStandardUniforms) {
                    if (type != typeOfGLUniform(id)) {
                        Z_LOG("Uniform \"" << name
                            << "\" has invalid type (should be " << typeNameOfGLUniform(id) << ").");
                    }
                    m_UniformHandles[int(id)] = gl::GetUniformLocation(m_Handle, name.c_str());
                }
            }
        }

        return true;
    }
}
