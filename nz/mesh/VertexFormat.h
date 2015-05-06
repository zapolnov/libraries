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
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Z
{
    class VertexFormat
    {
    public:
        enum AttributeID : unsigned char
        {
            Position = 0,
            TexCoord,
            Normal,
            Tangent,
            Bitangent,
            BoneIndices,
            BoneWeights,
            NumAttributes,
        };

        enum Type : unsigned char
        {
            UnsignedByte = 0,
            Float,
        };

        struct Attribute
        {
            AttributeID id;
            Type type;
            unsigned char count;
            unsigned char offset;
        };

        typedef unsigned char UByte4[4];

        VertexFormat();
        ~VertexFormat() = default;

        size_t size() const { return m_Size; }

        void addAttribute(AttributeID id, unsigned char count, Type type);

        bool hasAttribute(AttributeID id) const;
        const Attribute& attribute(AttributeID id) const;

        UByte4& refUByte(AttributeID id, void* base, size_t index = 0) const;
        float& refFloat(AttributeID id, void* base, size_t index = 0) const;
        glm::vec2& refVec2(AttributeID id, void* base, size_t index = 0) const;
        glm::vec3& refVec3(AttributeID id, void* base, size_t index = 0) const;
        glm::vec4& refVec4(AttributeID id, void* base, size_t index = 0) const;

        static size_t sizeOf(Type type);

    private:
        Attribute m_Attributes[NumAttributes];
        size_t m_Size = 0;
    };

    using VertexFormatPtr = std::shared_ptr<VertexFormat>;
}
