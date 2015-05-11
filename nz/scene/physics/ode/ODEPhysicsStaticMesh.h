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
#include <scene/SceneMesh.h>
#include <vector>

namespace Z
{
    class ODEPhysicsStaticMesh : public SceneMesh
    {
    public:
        static const unsigned long CATEGORY_BIT = 0b0000001;

        ODEPhysicsStaticMesh(const ODEPhysicsWorld* world, GLResourceManager& resourceManager, const MeshPtr& mesh);
        ~ODEPhysicsStaticMesh();

    protected:
        void invalidate() override;
        void updateTransform(const glm::mat4& parentMatrix, bool parentMatrixChanged) override;

    private:
        std::shared_ptr<ODEPhysicsWorld::Instance> m_ODE;
        std::vector<dGeomID> m_Geoms;
        std::vector<dTriMeshDataID> m_TriMeshData;
        std::vector<dTriIndex> m_IndicesBuffer;
        MeshPtr m_Mesh;
        bool m_PositionChanged = true;
    };

    using ODEPhysicsStaticMeshPtr = std::shared_ptr<ODEPhysicsStaticMesh>;
}
