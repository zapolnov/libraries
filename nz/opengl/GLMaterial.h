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
#include "GLProgram.h"
#include "GLTexture.h"
#include "utility/streams/InputStream.h"

namespace Z
{
    class GLMaterial : public GLResource
    {
    public:
        explicit GLMaterial(GLResourceManager* manager);
        ~GLMaterial();

        const GLProgramPtr& program() const { return m_Program; }
        const GLTexturePtr& diffuseMap() const { return m_DiffuseMap; }
        bool cullFace() const { return m_CullFace; }
        bool depthTest() const { return m_DepthTest; }
        bool invisible() const { return m_Invisible; }

        int getUniformLocation(const char* name) const { return m_Program ? m_Program->getUniformLocation(name) : -1; }
        int getUniformLocation(GLUniform name) const { return m_Program ? m_Program->getUniformLocation(name) : -1; }

        void reload() override;
        void unload() override;

        bool bind() const;
        void unbind() const;
        bool bindProgram() const;
        void unbindProgram() const;
        bool bindDiffuseMap(GL::Enum textureUnit = GL::TEXTURE0) const;
        void unbindDiffuseMap(GL::Enum textureUnit = GL::TEXTURE0) const;

    protected:
        GLProgramPtr m_Program;
        GLTexturePtr m_DiffuseMap;
        GL::Enum m_DiffuseWrapS;
        GL::Enum m_DiffuseWrapT;
        bool m_CullFace;
        bool m_DepthTest;
        bool m_Invisible;

        void reset();
        bool load(InputStream* stream);
    };

    using GLMaterialPtr = std::shared_ptr<GLMaterial>;
}
