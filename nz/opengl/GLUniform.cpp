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
#include "GLUniform.h"
#include "utility/debug.h"
#include <unordered_map>

namespace Z
{
    namespace GLUniformName
    {
        const char* forUniform(GLUniform uniform)
        {
            switch (uniform)
            {
            #define Z_GL_UNIFORM(NAME, STRING, TYPE) case GLUniform::NAME: return GLUniformName::NAME;
            #include "GLUniform.def"
            #undef Z_GL_UNIFORM
            case GLUniform::NumStandardUniforms: break;
            }
            Z_ASSERT_MSG(false, "Invalid uniform ID.");
            return nullptr;
        }
    }

    const GLUniformList& allGLUniforms()
    {
        static const GLUniformList allUniforms = {
            #define Z_GL_UNIFORM(NAME, STRING, TYPE) { GLUniform::NAME, GLUniformName::NAME },
            #include "GLUniform.def"
            #undef Z_GL_UNIFORM
        };
        return allUniforms;
    }

    GLUniform nameToGLUniform(const std::string& name)
    {
        static const std::unordered_map<std::string, GLUniform> uniforms = {
            #define Z_GL_UNIFORM(NAME, STRING, TYPE) { GLUniformName::NAME, GLUniform::NAME },
            #include "GLUniform.def"
            #undef Z_GL_UNIFORM
        };

        auto it = uniforms.find(name);
        return (it != uniforms.end() ? it->second : GLUniform::NumStandardUniforms);
    }

    GL::Enum typeOfGLUniform(GLUniform uniform)
    {
        switch (uniform)
        {
        #define Z_GL_UNIFORM(NAME, STRING, TYPE) case GLUniform::NAME: return TYPE;
        #include "GLUniform.def"
        #undef Z_GL_UNIFORM
        case GLUniform::NumStandardUniforms: break;
        }
        Z_ASSERT_MSG(false, "Invalid uniform ID.");
        return GL::NONE;
    }

    const char* typeNameOfGLUniform(GLUniform uniform)
    {
        switch (uniform)
        {
        #define Z_GL_UNIFORM(NAME, STRING, TYPE) case GLUniform::NAME: return #TYPE;
        #include "GLUniform.def"
        #undef Z_GL_UNIFORM
        case GLUniform::NumStandardUniforms: break;
        }
        Z_ASSERT_MSG(false, "Invalid uniform ID.");
        return nullptr;
    }
}
