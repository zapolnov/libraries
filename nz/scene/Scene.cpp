/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com)
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
#include "Scene.h"
#include <opengl/GLUniformSet.h>

namespace Z
{
    static const glm::mat4 g_IdentityMatrix = glm::mat4(1.0f);

    Scene::Scene()
        : m_ClearBits(GL::COLOR_BUFFER_BIT | GL::DEPTH_BUFFER_BIT)
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::setCamera(const SceneCameraPtr& camera)
    {
        if (m_Camera != camera)
            m_Camera = camera;
    }

    void Scene::runFrame(double time, GLUniformSet* uniforms)
    {
        if (!uniforms) {
            GLUniformSet defaultUniforms;
            runFrame(time, &defaultUniforms);
            return;
        }

        if (m_ClearBits != 0) {
            gl::ClearColor(0.1f, 0.3f, 0.7f, 1.0f);
            gl::Clear(m_ClearBits);
        }

        updateTransform(g_IdentityMatrix, false);

        update(time);
        updateTransform(g_IdentityMatrix, false);

        Z_CHECK(m_Camera != nullptr);
        if (m_Camera) {
            uniforms->setMatrix4fv(GLUniform::ProjectionMatrix, m_Camera->projectionMatrix());
            uniforms->setMatrix4fv(GLUniform::ViewMatrix, m_Camera->viewMatrix());
            draw(m_Camera->frustum(), uniforms);
        }

    }
}
