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
#include <mutex>

namespace Z
{
    static const int MAX_CONTACTS = 40;
    static std::once_flag g_PhysicsFormatInitFlag;
    static std::once_flag g_ODEInitFlag;


    ODEPhysicsWorld::Instance::Instance()
    {
        std::call_once(g_ODEInitFlag, []() {
            dInitODE();
        });

        m_World = dWorldCreate();
        m_Space = dSimpleSpaceCreate(0);
        m_ContactGroup = dJointGroupCreate(0);
    }

    ODEPhysicsWorld::Instance::~Instance()
    {
        dJointGroupDestroy(m_ContactGroup);
        dSpaceDestroy(m_Space);
        dWorldDestroy(m_World);
    }


    ODEPhysicsWorld::ODEPhysicsWorld()
    {
        m_Instance = std::make_shared<Instance>();

        dWorldSetGravity(m_Instance->m_World, 0.0f, -9.8f, 0.0f);
        dWorldSetCFM(m_Instance->m_World, 0.0f);
        dWorldSetAutoDisableFlag(m_Instance->m_World, 1);
    }

    ODEPhysicsWorld::~ODEPhysicsWorld()
    {
    }

    void ODEPhysicsWorld::update(double time)
    {
        beforeUpdatePhysics();

        m_Time += std::min(time, 1.0 / 60.0);
        if (m_Time > 1.0 / 60.0) {
            dSpaceCollide(m_Instance->m_Space, this, &odeCollisionCallback);
            dWorldStep(m_Instance->m_World, 1.0 / 60.0);
            dJointGroupEmpty(m_Instance->m_ContactGroup);
            m_Time -= 1.0 / 60.0;
        }

        afterUpdatePhysics();

        SceneNode::update(time);
    }

    bool ODEPhysicsWorld::handleCollision(dGeomID geom1, dGeomID geom2)
    {
        dBodyID body1 = dGeomGetBody(geom1);
        dBodyID body2 = dGeomGetBody(geom2);
        if (body1 && body2 && dAreConnectedExcluding(body1, body2, dJointTypeContact))
            return false;

        dContact contact[MAX_CONTACTS];
        memset(contact, 0, sizeof(contact));

        int numContacts = dCollide(geom1, geom2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
        if (numContacts > 0) {
            for (int i = 0; i < numContacts; i++) {
                dJointID joint = dJointCreateContact(m_Instance->m_World, m_Instance->m_ContactGroup, &contact[i]);
                dJointAttach(joint, body1, body2);
            }
            return true;
        }

        return false;
    }

    void ODEPhysicsWorld::odeCollisionCallback(void* data, dGeomID geom1, dGeomID geom2)
    {
        ODEPhysicsWorld* self = reinterpret_cast<ODEPhysicsWorld*>(data);
        self->handleCollision(geom1, geom2);
    }
}
