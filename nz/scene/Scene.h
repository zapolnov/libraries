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

#pragma once
#include "SceneNode.h"
#include "SceneCamera.h"
#include <opengl/opengl.h>
#include <memory>

namespace Z
{
    class GLUniformSet;

    class Scene : public SceneNode
    {
    public:
        Scene();
        ~Scene();

        const SceneCameraPtr& camera() const { return m_Camera; }
        void setCamera(const SceneCameraPtr& camera);

        GL::Bitfield clearBits() const { return m_ClearBits; }
        void setClearBits(GL::Bitfield bits) { m_ClearBits = bits; }

        void runFrame(double time, GLUniformSet* uniforms = nullptr);

    private:
        SceneCameraPtr m_Camera;
        GL::Bitfield m_ClearBits;
    };

    using ScenePtr = std::shared_ptr<Scene>;
}
