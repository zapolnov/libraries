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
#include "ODEPhysicsWorld.h"
#include "mesh/Mesh.h"
#include <scene/SceneNode.h>
#include <vector>

namespace Z
{
    class ODEPhysicsBody : public SceneNode
    {
    public:
        explicit ODEPhysicsBody(const ODEPhysicsWorld* world);
        ~ODEPhysicsBody();

        dBodyID odeBody() const { return m_Body; }

        void addBox(float density, float x, float y, float z);

    protected:
        void invalidate() override;

        void updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged) override;
        void update(double time) override;

    private:
        std::shared_ptr<ODEPhysicsWorld::Instance> m_ODE;
        dSpaceID m_Space;
        dBodyID m_Body;
        std::vector<dGeomID> m_Geoms;
        bool m_PositionChanged = true;
    };

    using ODEPhysicsBodyPtr = std::shared_ptr<ODEPhysicsBody>;
}
