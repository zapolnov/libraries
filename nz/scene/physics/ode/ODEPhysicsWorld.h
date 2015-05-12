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
#include <scene/SceneNode.h>
#include <ode/ode.h>

namespace Z
{
    class ODEPhysicsBody;

    class ODEPhysicsWorld : public SceneNode
    {
    public:
        ODEPhysicsWorld();
        ~ODEPhysicsWorld();

        dWorldID odeWorld() const { return m_Instance->m_World; }
        dSpaceID odeSpace() const { return m_Instance->m_Space; }
        dJointGroupID odeJointGroup() const { return m_Instance->m_ContactGroup; }

    protected:
        void update(double time) override;

        virtual void beforeUpdatePhysics() {}
        virtual void afterUpdatePhysics() {}

        virtual bool handleCollision(dGeomID geom1, dGeomID geom2);

    private:
        struct Instance
        {
            dWorldID m_World;
            dSpaceID m_Space;
            dJointGroupID m_ContactGroup;

            Instance();
            ~Instance();
        };

        double m_Time = 0.0;
        std::shared_ptr<Instance> m_Instance;

        static void odeCollisionCallback(void* data, dGeomID geom1, dGeomID geom2);

        friend class ODEPhysicsBody;
        friend class ODEPhysicsStaticMesh;
    };

    using ODEPhysicsWorldPtr = std::shared_ptr<ODEPhysicsWorld>;
}
