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
#include "ODEPhysicsWorld.h"

namespace Z
{
    static const int MAX_CONTACTS = 50;
    static std::once_flag g_ODEInitFlag;

    ODEPhysicsWorld::ODEPhysicsWorld()
    {
        std::call_once(g_ODEInitFlag, []() {
            dInitODE2(0);
        });

        m_World = dWorldCreate();
        m_Space = dSimpleSpaceCreate(0);
        m_JointGroup = dJointGroupCreate(0);

        dWorldSetGravity(m_World, 0.0f, -1.0f, 0.0f);
        dWorldSetERP(m_World, 0.2f);
        dWorldSetCFM(m_World, 1.0e-5);
        dWorldSetContactMaxCorrectingVel(m_World, 0.9f);
        dWorldSetContactSurfaceLayer(m_World, 0.001f);
        dWorldSetAutoDisableFlag(m_World, 1);
    }

    ODEPhysicsWorld::~ODEPhysicsWorld()
    {
        dJointGroupDestroy(m_JointGroup);
        dSpaceDestroy(m_Space);
        dWorldDestroy(m_World);
    }

    void ODEPhysicsWorld::update(double time)
    {
        m_Time += time;
        while (m_Time > 1.0 / 30.0) {
            dSpaceCollide(m_Space, this, &handleCollision);
            dWorldStep(m_World, 0.05);
            dJointGroupEmpty(m_JointGroup);
            m_Time -= 1.0 / 30.0;
        }

        SceneNode::update(time);
    }

    void ODEPhysicsWorld::handleCollision(void* data, dGeomID geom1, dGeomID geom2)
    {
        ODEPhysicsWorld* self = reinterpret_cast<ODEPhysicsWorld*>(data);

        dContact contact[MAX_CONTACTS];
        for (int i = 0; i < MAX_CONTACTS; i++) {
            contact[i].surface.mode = dContactBounce | dContactSoftCFM;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.mu2 = 0;
            contact[i].surface.bounce = 0.01f;
            contact[i].surface.bounce_vel = 0.1f;
            contact[i].surface.soft_cfm = 0.01f;
        }

        int numContacts = dCollide(geom1, geom2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
        if (numContacts > 0) {
            dBodyID body1 = dGeomGetBody(geom1);
            dBodyID body2 = dGeomGetBody(geom2);

            for (int i = 0; i < numContacts; i++) {
                dJointID joint = dJointCreateContact(self->m_World, self->m_JointGroup, &contact[i]);
                dJointAttach(joint, body1, body2);
            }
        }
    }
}
