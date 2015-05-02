/*
 * Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include <opengl/opengl.h>
#include <opengl/GLResourceManager.h>
#include <glm/mat4x4.hpp>
#include <Box2D/Common/b2Draw.h>
#include <vector>

namespace Z
{
    class DebugRenderer
    {
    public:
        enum Primitives
        {
            Points = 0b0001,
            Lines = 0b0010,
            Triangles = 0b0100,
        };

        DebugRenderer(GLResourceManager& resourceManager);
        ~DebugRenderer();

        void setupForRendering(const glm::mat4& projectionMatrix);
        void cleanupAfterRendering();

        void render(size_t mode, size_t numVertices, const b2Color& color, float ptSize = 1.0f) const;

        size_t rescaleVertices(const b2Vec2* vertices, int vertexCount, float scale = 1.0f);
        size_t initCircleVertices(const b2Vec2& center, float radius, float scale = 1.0f);

        void drawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
        void drawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
        void drawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
        void drawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    private:
        GLResourceManager& m_ResourceManager;
        std::vector<GL::Float> m_Vertices;
        GLProgramPtr m_Program;
        int m_UniformProjection;
        int m_UniformColor;
        int m_UniformPointSize;
        int m_AttribPosition;
        bool m_Initialized;

        DebugRenderer(const DebugRenderer&) = delete;
        DebugRenderer & operator=(const DebugRenderer&) = delete;
    };
}
