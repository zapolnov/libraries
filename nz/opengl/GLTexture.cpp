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
#include "GLTexture.h"
#include "utility/debug.h"

namespace Z
{
    GLTexture::GLTexture(GLResourceManager* manager, GL::Enum type)
        : GLResource(manager)
        , m_Type(type)
    {
    }

    GLTexture::~GLTexture()
    {
        unload();
    }

    bool GLTexture::bind()
    {
        if (m_Handle == 0)
            return false;

        gl::BindTexture(m_Type, m_Handle);
        if (m_Dirty) {
            gl::TexParameteri(m_Type, GL::TEXTURE_MIN_FILTER, m_MinFilter);
            gl::TexParameteri(m_Type, GL::TEXTURE_MAG_FILTER, m_MagFilter);
            gl::TexParameteri(m_Type, GL::TEXTURE_WRAP_S, m_WrapS);
            gl::TexParameteri(m_Type, GL::TEXTURE_WRAP_T, m_WrapT);
            m_Dirty = false;
        }

        return true;
    }

    void GLTexture::reload()
    {
        unload();
        gl::GenTextures(1, &m_Handle);
        m_Dirty = true;
    }

    void GLTexture::unload()
    {
        if (m_Handle != 0) {
            gl::DeleteTextures(1, &m_Handle);
            m_Handle = 0;
        }
    }

    void GLTexture::upload(GL::Int level, const ImagePtr& image)
    {
        bind();
        gl::PixelStorei(GL::UNPACK_ALIGNMENT, 1);

        if (!image) {
            static const uint8_t pixel[4] = { 0xFF, 0x00, 0x00, 0xFF };
            gl::TexImage2D(m_Type, level, GL::LUMINANCE, 2, 2, 0, GL::LUMINANCE, GL::UNSIGNED_BYTE, &pixel);
            return;
        }

        bool isCompressed = false;
        GL::Enum type = GL::NONE;
        GL::Enum internalFormat = GL::NONE;
        GL::Enum format = GL::NONE;
        switch (image->format())
        {
        case Image::Unknown:
            break;

        case Image::Luminance8:
            type = GL::UNSIGNED_BYTE;
            internalFormat = GL::LUMINANCE;
            format = GL::LUMINANCE;
            break;

        case Image::LuminanceAlpha8:
            type = GL::UNSIGNED_BYTE;
            internalFormat = GL::LUMINANCE_ALPHA;
            format = GL::LUMINANCE_ALPHA;
            break;

        case Image::RGB8:
            type = GL::UNSIGNED_BYTE;
            internalFormat = GL::RGB;
            format = GL::RGB;
            break;

        case Image::RGBA8:
            type = GL::UNSIGNED_BYTE;
            internalFormat = GL::RGBA;
            format = GL::RGBA;
            break;
        }

        Z_ASSERT_MSG(type != GL::NONE, "Invalid image format.");

        if (!isCompressed) {
            gl::TexImage2D(m_Type, level, internalFormat, image->width(), image->height(),
                0, format, type, image->data());
        } else {
            gl::CompressedTexImage2D(m_Type, level, internalFormat, image->width(), image->height(),
                0, GL::Sizei(image->dataSize()), image->data());
        }
    }
}
