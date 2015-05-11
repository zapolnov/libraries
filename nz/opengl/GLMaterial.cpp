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
        // Keep old resources resident until this function returns
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
            InputStream* stream;
            GLResourceManager* resourceManager;
            std::string baseDir;

            std::string nodeToPath(const YamlNode& node) const
            {
                std::string value = node.toString();
                size_t length = value.length();
                if (length > 0 && value[0] == '/')
                    return value.substr(1);
                else {
                    std::string localFileName = baseDir + value;
                    if (resourceManager->fileSystem()->fileExists(localFileName))
                        return localFileName;
                    return value;
                }
            }

            bool nodeToTextureWrap(const YamlNode& node, GL::Enum& value) const
            {
                std::string str = node.toString();
                if (str == "clamp_to_edge") {
                    value = GL::CLAMP_TO_EDGE;
                    return true;
                }
                if (str == "repeat") {
                    value = GL::REPEAT;
                    return true;
                }
                return false;
            }

            void logError(const YamlNode& node, const std::string& message)
            {
                Z_LOG("Unable to parse file \"" << stream->name() << "\": at line " << node.line() << ": " << message);
            }
        };

        static const std::unordered_map<std::string, std::function<bool(Context* C, const YamlNode&)>> handlers = {
            { "shader", [](Context* C, const YamlNode& node) -> bool {
                C->material->m_Program = C->resourceManager->loadProgram(C->nodeToPath(node));
                return true;
            }},

            { "diffuseMap", [](Context* C, const YamlNode& node) -> bool {
                C->material->m_DiffuseMap = C->resourceManager->loadTexture(C->nodeToPath(node));
                if (C->material->m_DiffuseMap) {
                    C->material->m_DiffuseMap->setWrapS(C->material->m_DiffuseWrapS);
                    C->material->m_DiffuseMap->setWrapT(C->material->m_DiffuseWrapT);
                }
                return true;
            }},

            { "diffuseWrapS", [](Context* C, const YamlNode& node) -> bool {
                if (!C->nodeToTextureWrap(node, C->material->m_DiffuseWrapS))
                    return false;
                if (C->material->m_DiffuseMap)
                    C->material->m_DiffuseMap->setWrapS(C->material->m_DiffuseWrapS);
                return true;
            }},

            { "diffuseWrapT", [](Context* C, const YamlNode& node) -> bool {
                if (!C->nodeToTextureWrap(node, C->material->m_DiffuseWrapT))
                    return false;
                if (C->material->m_DiffuseMap)
                    C->material->m_DiffuseMap->setWrapT(C->material->m_DiffuseWrapT);
                return true;
            }},

            { "cullFace", [](Context* C, const YamlNode& node) -> bool {
                auto value = node.toBool();
                if (value == Z::YamlNode::Indeterminate)
                    return false;
                C->material->m_CullFace = (value == Z::YamlNode::True);
                return true;
            }},

            { "depthTest", [](Context* C, const YamlNode& node) -> bool {
                auto value = node.toBool();
                if (value == Z::YamlNode::Indeterminate)
                    return false;
                C->material->m_DepthTest = (value == Z::YamlNode::True);
                return true;
            }},

            { "invisible", [](Context* C, const YamlNode& node) -> bool {
                auto value = node.toBool();
                if (value == Z::YamlNode::Indeterminate)
                    return false;
                C->material->m_Invisible = (value == Z::YamlNode::True);
                return true;
            }},
        };

        Context context;
        context.material = this;
        context.stream = stream;
        context.resourceManager = resourceManager();

        const std::string& fileName = stream->name();
        size_t index = fileName.rfind('/');
        context.baseDir = (index == std::string::npos ? std::string() : fileName.substr(0, index + 1));

        for (const auto& option : rootNode.toMapping()) {
            auto it = handlers.find(option.first);
            if (it == handlers.end()) {
                context.logError(option.second, "unknown option \"" + option.first + "\".");
                return false;
            }

            if (!it->second(&context, option.second)) {
                context.logError(option.second, "invalid value for option \"" + option.first + "\".");
                return false;
            }
        }

        return true;
    }
};
