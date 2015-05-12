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
#include "ODEPhysicsBody.h"
#include <glm/glm.hpp>

namespace Z
{
    ODEPhysicsBody::ODEPhysicsBody(const ODEPhysicsWorld* world)
    {
        Z_ASSERT(world != nullptr);
        m_ODE = world->m_Instance;

        m_Space = m_ODE->m_Space;
        m_Body = dBodyCreate(m_ODE->m_World);
        dBodySetData(m_Body, this);
    }

    ODEPhysicsBody::~ODEPhysicsBody()
    {
        for (dGeomID geom : m_Geoms)
            dGeomDestroy(geom);
        dBodyDestroy(m_Body);
    }

    void ODEPhysicsBody::addBox(float density, float x, float y, float z)
    {
        // FIXME
        dMass m;
        dMassSetBox(&m, density, x, y, z);

        dGeomID geom = dCreateBox(m_Space, x, y, z);
        dGeomSetBody(geom, m_Body);
        dGeomSetData(geom, this);

        dBodySetMass(m_Body, &m);

        m_Geoms.push_back(geom);
    }

    void ODEPhysicsBody::invalidate()
    {
        SceneNode::invalidate();
        m_PositionChanged = true;
    }

    void ODEPhysicsBody::updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged)
    {
        SceneNode::updateTransform(parentMatrix, parentMatrixChanged);

        if (m_PositionChanged) {
            glm::vec4 pos = worldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            dBodySetPosition(m_Body, pos.x, pos.y, pos.z);

            /* FIXME
            glm::quat rot = rotation();
            dQuaternion quat = { rot.w, rot.x, rot.y, rot.z };
            dBodySetQuaternion(m_Body, quat);
            */

            m_PositionChanged = false;
        }
    }

    void ODEPhysicsBody::update(double time)
    {
        const dReal* p = dBodyGetPosition(m_Body);
        glm::vec4 pos{ float(p[0]), float(p[1]), float(p[2]), 1.0f };

        const dReal* r = dBodyGetQuaternion(m_Body);
        glm::quat quat{ float(r[3]), float(r[0]), float(r[1]), float(r[2]) };

        if (parent())
            pos = parent()->inverseWorldMatrix() * pos;

        setPosition(glm::vec3(pos));
        // FIXME
        // setRotation(quat);
        m_PositionChanged = false;

        SceneNode::update(time);
    }
}
