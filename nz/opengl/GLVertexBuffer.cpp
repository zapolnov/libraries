/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com).
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
#include "GLVertexBuffer.h"

namespace Z
{
    GLVertexBuffer::GLVertexBuffer(GLResourceManager* manager, const VertexFormatPtr& format)
        : GLBuffer(manager, GL::ARRAY_BUFFER)
        , m_Format(format)
    {
    }

    void GLVertexBuffer::enableAttributes()
    {
        for (int i = 0; i < VertexFormat::NumAttributes; i++) {
            if (m_Format->hasAttribute(VertexFormat::AttributeID(i))) {
                gl::EnableVertexAttribArray(i);
            }
        }
    }

    void GLVertexBuffer::disableAttributes()
    {
        for (int i = 0; i < VertexFormat::NumAttributes; i++) {
            if (m_Format->hasAttribute(VertexFormat::AttributeID(i)))
                gl::DisableVertexAttribArray(i);
        }
    }

    bool GLVertexBuffer::bind()
    {
        if (!GLBuffer::bind())
            return false;

        for (int i = 0; i < VertexFormat::NumAttributes; i++) {
            if (m_Format->hasAttribute(VertexFormat::AttributeID(i))) {
                const auto& attr = m_Format->attribute(VertexFormat::AttributeID(i));

                GL::Enum type = GL::NONE;
                switch (attr.type)
                {
                case VertexFormat::UnsignedByte: type = GL::UNSIGNED_BYTE; break;
                case VertexFormat::Float: type = GL::FLOAT; break;
                }

                auto offset = reinterpret_cast<void*>(attr.offset);
                gl::VertexAttribPointer(i, attr.count, type, GL::FALSE, GL::Sizei(m_Format->size()), offset);
            }
        }

        return true;
    }
}
