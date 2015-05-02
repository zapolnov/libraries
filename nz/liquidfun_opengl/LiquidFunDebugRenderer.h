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
#include "DebugRenderer.h"
#include <Box2D/Dynamics/b2World.h>

namespace Z
{
    class LiquidFunDebugRenderer : public b2Draw, public DebugRenderer
    {
    public:
        LiquidFunDebugRenderer(GLResourceManager& resourceManager, unsigned flags = b2Draw::e_particleBit |
            b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
        ~LiquidFunDebugRenderer();

        inline const glm::mat4& projectionMatrix() const { return m_ProjectionMatrix; }
        inline void setProjectionMatrix(const glm::mat4& matrix) { m_ProjectionMatrix = matrix; }

        inline float pixelsPerMeter() const { return m_PixelsPerMeter; }
        inline void setPixelsPerMeter(float v) { m_PixelsPerMeter = v; }

        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
        void DrawSolidCircle(const b2Vec2& center, float32 r, const b2Vec2& axis, const b2Color& color) override;
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
        void DrawParticles(const b2Vec2* centers, float32 radius, const b2ParticleColor* colors, int32 count) override;
        void DrawTransform(const b2Transform& transform) override;

        inline void setupForRendering() { DebugRenderer::setupForRendering(m_ProjectionMatrix); }

        void render(b2World* world);
        inline void render(const std::unique_ptr<b2World>& world) { render(world.get()); }
        inline void render(const std::shared_ptr<b2World>& world) { render(world.get()); }

    private:
        float m_PixelsPerMeter;
        glm::mat4 m_ProjectionMatrix;

        LiquidFunDebugRenderer(const LiquidFunDebugRenderer&) = delete;
        LiquidFunDebugRenderer & operator=(const LiquidFunDebugRenderer&) = delete;
    };
}
