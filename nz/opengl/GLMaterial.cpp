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
#include "GLMaterial.h"
#include "GLResourceManager.h"
#include "yaml/YamlParser.h"
#include "yaml/YamlMappingVisitor.h"
#include "utility/debug.h"
#include <functional>
#include <unordered_map>

namespace Z
{
    GLMaterial::GLMaterial(GLResourceManager* manager)
        : GLResource(manager)
    {
        reset();
    }

    GLMaterial::~GLMaterial()
    {
    }

    void GLMaterial::reload()
    {
    }

    void GLMaterial::unload()
    {
    }

    bool GLMaterial::bind() const
    {
        bool success = true;

        if (m_Invisible)
            return false;

        if (!bindProgram())
            return false;

        if (!bindDiffuseMap())
            success = false;

        gl::EnableOrDisable(GL::CULL_FACE, m_CullFace);
        gl::EnableOrDisable(GL::DEPTH_TEST, m_DepthTest);

        return success;
    }

    void GLMaterial::unbind() const
    {
        unbindDiffuseMap();
        unbindProgram();
    }

    bool GLMaterial::bindProgram() const
    {
        if (!m_Program)
            return false;
        m_Program->use();
        return true;
    }

    void GLMaterial::unbindProgram() const
    {
        gl::UseProgram(0);
    }

    bool GLMaterial::bindDiffuseMap(GL::Enum textureUnit) const
    {
        bool success = true;

        if (m_DiffuseMap) {
            gl::ActiveTexture(textureUnit);
            if (!m_DiffuseMap->bind()) {
                success = false;
                gl::BindTexture(GL::TEXTURE_2D, 0);
            }

            int textureUniform = m_Program->getUniformLocation(GLUniform::Texture);
            if (textureUniform != -1)
                gl::Uniform1i(textureUniform, 0);
        }

        return success;
    }

    void GLMaterial::unbindDiffuseMap(GL::Enum textureUnit) const
    {
        if (m_DiffuseMap) {
            gl::ActiveTexture(textureUnit);
            gl::BindTexture(GL::TEXTURE_2D, 0);
        }
    }

    std::string GLMaterial::fileNameForMaterial(std::string name)
    {
        size_t length = name.length();

        for (auto& ch : name) {
            unsigned char c = static_cast<unsigned char>(ch);
            switch (c)
            {
            case '?':
            case '*':
            case '<':
            case '>':
            case ':':
                ch = '_';
                break;
            case '\\':
                ch = '/';
                break;
            default:
                if (c < 32 || c >= 127)
                    ch = '_';
            }
        }

        static const std::string SUFFIX = "material";
        if (length <= SUFFIX.length() || name.substr(length - SUFFIX.length()) != SUFFIX) {
            if (length > 0 && name[length - 1] == '.')
                name = name + SUFFIX;
            else
                name = name + '.' + SUFFIX;
        } else {
            size_t index = length - SUFFIX.length() - 1;
            if (name[index] != '.') {
                if (name[index] == '-' || name[index] == '_')
                    name[index] = '.';
                else
                    name = name + '.' + SUFFIX;
            }
        }

        return name;
    }

    void GLMaterial::reset()
    {
        m_Program.reset();
        m_DiffuseMap.reset();
        m_DiffuseWrapS = GL::CLAMP_TO_EDGE;
        m_DiffuseWrapT = GL::CLAMP_TO_EDGE;
        m_CullFace = true;
        m_DepthTest = true;
        m_Invisible = false;
    }

    bool GLMaterial::load(InputStream* stream)
    {
        // Keep old resources resident until this method returns
        GLProgramPtr oldProgram = std::move(m_Program);
        GLTexturePtr oldDiffuseMap = std::move(m_DiffuseMap);
        (void)oldProgram;
        (void)oldDiffuseMap;

        reset();

        YamlErrorPtr error;
        YamlNode rootNode = yamlParseStream(stream, error);
        if (error) {
            Z_LOG("Unable to parse file \"" << stream->name() << "\": " << *error);
            return false;
        }

        struct Context
        {
            GLMaterial* material;
            GLResourceManager* resourceManager;
            FileSystemPtr fileSystem;
        };

        static const YamlMappingVisitor::EnumValues<GL::Enum> enumTextureWrap = {
            { "clamp_to_edge", GL::CLAMP_TO_EDGE },
            { "repeat", GL::REPEAT },
        };

        typedef YamlMappingVisitorEx<Context> Visitor;
        static const Visitor::HandlerMap handlers = {
            { "shader", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                C->material->m_Program = C->resourceManager->loadProgram(V->nodeToPath(C->fileSystem, node));
                return true;
            }},

            { "diffuseMap", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                C->material->m_DiffuseMap = C->resourceManager->loadTexture(V->nodeToPath(C->fileSystem, node));
                if (C->material->m_DiffuseMap) {
                    C->material->m_DiffuseMap->setWrapS(C->material->m_DiffuseWrapS);
                    C->material->m_DiffuseMap->setWrapT(C->material->m_DiffuseWrapT);
                }
                return true;
            }},

            { "diffuseWrapS", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                if (!V->nodeToEnum(node, C->material->m_DiffuseWrapS, enumTextureWrap))
                    return false;
                if (C->material->m_DiffuseMap)
                    C->material->m_DiffuseMap->setWrapS(C->material->m_DiffuseWrapS);
                return true;
            }},

            { "diffuseWrapT", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                if (!V->nodeToEnum(node, C->material->m_DiffuseWrapT, enumTextureWrap))
                    return false;
                if (C->material->m_DiffuseMap)
                    C->material->m_DiffuseMap->setWrapT(C->material->m_DiffuseWrapT);
                return true;
            }},

            { "cullFace", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                return V->nodeToBool(node, C->material->m_CullFace);
            }},

            { "depthTest", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                return V->nodeToBool(node, C->material->m_DepthTest);
            }},

            { "invisible", [](Context* C, Visitor* V, const YamlNode& node) -> bool {
                return V->nodeToBool(node, C->material->m_Invisible);
            }},
        };

        Context context;
        context.material = this;
        context.resourceManager = resourceManager();
        context.fileSystem = context.resourceManager->fileSystem();

        Visitor visitor(&context, stream->name(), rootNode, handlers);
        return visitor.visit();
    }
};
