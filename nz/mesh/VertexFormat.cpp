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
#include "VertexFormat.h"
#include "utility/debug.h"
#include <cstring>

namespace Z
{
    VertexFormat::VertexFormat()
    {
        memset(m_Attributes, 0, sizeof(m_Attributes));
    }

    void VertexFormat::addAttribute(AttributeID id, unsigned char count, Type type)
    {
        Z_ASSERT(id >= 0 && id < NumAttributes);
        Z_ASSERT(m_Attributes[id].count == 0);

        Z_ASSERT(
            (type == Float && count > 0 && count <= 4) ||
            (type == UnsignedByte && count == 4)
        );

        Attribute& attr = m_Attributes[id];
        attr.id = id;
        attr.type = type;
        attr.count = count;
        attr.offset = static_cast<unsigned char>(m_Size);

        m_Size += sizeOf(type) * count;
        Z_ASSERT(m_Size <= 255);
    }

    bool VertexFormat::hasAttribute(AttributeID id) const
    {
        Z_CHECK(id >= 0 && id < NumAttributes);
        return (id < 0 || id >= NumAttributes ? false : m_Attributes[id].count != 0);
    }

    const VertexFormat::Attribute& VertexFormat::attribute(AttributeID id) const
    {
        Z_CHECK(id >= 0 && id < NumAttributes);
        return m_Attributes[id];
    }

    VertexFormat::UByte4& VertexFormat::refUByte(AttributeID id, void* base, size_t index) const
    {
        Z_ASSERT(hasAttribute(id));

        const auto& attr = attribute(id);
        Z_ASSERT(attr.type == UnsignedByte);
        Z_ASSERT(attr.count == 4);

        char* p = reinterpret_cast<char*>(base);
        return *reinterpret_cast<UByte4*>(p + index * m_Size + attr.offset);
    }

    float& VertexFormat::refFloat(AttributeID id, void* base, size_t index) const
    {
        Z_ASSERT(hasAttribute(id));

        const auto& attr = attribute(id);
        Z_ASSERT(attr.type == Float);
        Z_ASSERT(attr.count == 1);

        char* p = reinterpret_cast<char*>(base);
        return *reinterpret_cast<float*>(p + index * m_Size + attr.offset);
    }

    glm::vec2& VertexFormat::refVec2(AttributeID id, void* base, size_t index) const
    {
        Z_ASSERT(hasAttribute(id));

        const auto& attr = attribute(id);
        Z_ASSERT(attr.type == Float);
        Z_ASSERT(attr.count == 2);

        char* p = reinterpret_cast<char*>(base);
        return *reinterpret_cast<glm::vec2*>(p + index * m_Size + attr.offset);
    }

    glm::vec3& VertexFormat::refVec3(AttributeID id, void* base, size_t index) const
    {
        Z_ASSERT(hasAttribute(id));

        const auto& attr = attribute(id);
        Z_ASSERT(attr.type == Float);
        Z_ASSERT(attr.count == 3);

        char* p = reinterpret_cast<char*>(base);
        return *reinterpret_cast<glm::vec3*>(p + index * m_Size + attr.offset);
    }

    glm::vec4& VertexFormat::refVec4(AttributeID id, void* base, size_t index) const
    {
        Z_ASSERT(hasAttribute(id));

        const auto& attr = attribute(id);
        Z_ASSERT(attr.type == Float);
        Z_ASSERT(attr.count == 4);

        char* p = reinterpret_cast<char*>(base);
        return *reinterpret_cast<glm::vec4*>(p + index * m_Size + attr.offset);
    }

    size_t VertexFormat::sizeOf(Type type)
    {
        switch (type)
        {
        case UnsignedByte: return sizeof(unsigned char);
        case Float: return sizeof(float);
        }

        Z_ASSERT_MSG(false, "Invalid vertex attribute type.");
        return 0;
    }
}
