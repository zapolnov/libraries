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
#include "GL1Renderer.h"
#include "utility/debug.h"
#include "opengl.h"

namespace Z
{
    GL1Renderer::GL1Renderer()
    {
    }

    GL1Renderer::~GL1Renderer()
    {
    }

    RendererTexturePtr GL1Renderer::createTexture()
    {
        Z_ASSERT(isCurrentThreadARenderThread());
        return createResource<GL1Texture>();
    }

    void GL1Renderer::setClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void GL1Renderer::clearBuffers(int buffers)
    {
        GLbitfield bits = 0;
        if (buffers & ColorBufferBit)
            bits |= GL_COLOR_BUFFER_BIT;
        if (buffers & DepthBufferBit)
            bits |= GL_DEPTH_BUFFER_BIT;
        if (buffers & StencilBufferBit)
            bits |= GL_STENCIL_BUFFER_BIT;
        Z_ASSERT(bits != 0);
        glClear(bits);
    }
}
