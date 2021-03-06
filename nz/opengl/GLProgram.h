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
#include "GLResource.h"
#include "GLUniform.h"
#include "opengl.h"
#include "utility/streams/InputStream.h"
#include <memory>

namespace Z
{
    class GLProgram : public GLResource
    {
    public:
        explicit GLProgram(GLResourceManager* manager);
        ~GLProgram();

        bool use() const;

        void reload() override;
        void unload() override;

        int getAttribLocation(const char* name) const { return gl::GetAttribLocation(m_Handle, name); }

        int getUniformLocation(const char* name) const { return gl::GetUniformLocation(m_Handle, name); }
        int getUniformLocation(GLUniform uniform) const { return m_UniformHandles[int(uniform)]; }

        InputStreamPtr openIncludeFile(std::string filename, const std::string& parentFileName) const;
        bool parseProgramSource(InputStream* input, std::vector<std::string>& vertex,
            std::vector<std::string>& fragment, std::vector<std::string>* what = nullptr) const;

        void printSource() const;

    protected:
        GL::UInt handle() const { return m_Handle; }

        void bindAttribLocation(int index, const char* name) { gl::BindAttribLocation(m_Handle, index, name); }
        void bindAttribLocation(GLAttribute index, const char* name) { gl::BindAttribLocation(m_Handle, index, name); }
        void bindAttribLocations();

        bool load(InputStream* input);
        bool loadSource(const std::string& vertex, const std::string& fragment);

        bool link();

        bool enumerateStandardUniforms();

    private:
        GL::UInt m_Handle = 0;
        int m_UniformHandles[int(GLUniform::NumStandardUniforms)];
    };

    using GLProgramPtr = std::shared_ptr<GLProgram>;
}
