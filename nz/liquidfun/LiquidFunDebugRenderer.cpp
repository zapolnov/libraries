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
 *
 * ----------------------------------------------------------------------------
 * Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
#include "LiquidFunDebugRenderer.h"
#include <stdarg.h>

namespace Z
{
    LiquidFunDebugRenderer::LiquidFunDebugRenderer(GLResourceManager& resourceManager, unsigned flags)
        : DebugRenderer(resourceManager)
        , m_PixelsPerMeter(50.0f)
    {
        SetFlags(flags);
    }

    LiquidFunDebugRenderer::~LiquidFunDebugRenderer()
    {
    }

    void LiquidFunDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        size_t n = rescaleVertices(vertices, vertexCount, m_PixelsPerMeter);
        DebugRenderer::render(DebugRenderer::Lines, n, color);
    }

    void LiquidFunDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
    {
        size_t n = rescaleVertices(vertices, vertexCount, m_PixelsPerMeter);
        DebugRenderer::render(DebugRenderer::Lines | DebugRenderer::Triangles, n, color);
    }

    void LiquidFunDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
    {
        size_t n = initCircleVertices(center, radius, m_PixelsPerMeter);
        DebugRenderer::render(DebugRenderer::Lines, n, color);
    }

    void LiquidFunDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis,
        const b2Color& color)
    {
        size_t n = initCircleVertices(center, radius, m_PixelsPerMeter);
        DebugRenderer::render(DebugRenderer::Lines | DebugRenderer::Triangles, n, color);
        DrawSegment(center, center + radius * axis, color);
    }

    void LiquidFunDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
    {
        b2Vec2 v[2] = { p1, p2 };
        size_t n = rescaleVertices(v, 2, m_PixelsPerMeter);
        DebugRenderer::render(DebugRenderer::Lines, n, color);
    }

    void LiquidFunDebugRenderer::DrawParticles(const b2Vec2* centers, float32 radius, const b2ParticleColor* colors,
        int32 count)
    {
        (void)colors;
        if (count == 0)
            return;

        size_t n = rescaleVertices(centers, count, m_PixelsPerMeter);
        const float particle_size_multiplier = 3;  // because of falloff
        DebugRenderer::render(DebugRenderer::Points, n, b2Color(1.0f, 1.0f, 1.0f), radius * particle_size_multiplier);
    }

    void LiquidFunDebugRenderer::DrawTransform(const b2Transform& transform)
    {
        const float32 AXIS_SCALE = 0.4f;
        DrawSegment(transform.p, transform.p + AXIS_SCALE * transform.q.GetXAxis(), b2Color(1, 0, 0));
        DrawSegment(transform.p, transform.p + AXIS_SCALE * transform.q.GetYAxis(), b2Color(0, 1, 0));
    }

    void LiquidFunDebugRenderer::render(b2World* world)
    {
        DebugRenderer::setupForRendering(m_ProjectionMatrix);
        world->SetDebugDraw(this);
        world->DrawDebugData();
        world->SetDebugDraw(nullptr);
        cleanupAfterRendering();
    }
}
