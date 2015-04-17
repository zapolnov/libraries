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
#include "PNGImageReader.h"
#include "utility/debug.h"
#include <png.h>
#include <cstring>

namespace Z
{
    static void pngError(png_structp pngp, png_const_charp message)
    {
        InputStream* stream = reinterpret_cast<InputStream*>(png_get_error_ptr(pngp));
        Z_LOG("Unable to decode PNG file \"" << stream->name() << "\": " << message);
        longjmp(png_jmpbuf(pngp), 1);
    }

    static void pngWarning(png_structp pngp, png_const_charp message)
    {
        InputStream* stream = reinterpret_cast<InputStream*>(png_get_error_ptr(pngp));
        Z_LOG("In PNG file \"" << stream->name() << "\": warning: " << message);
    }

    static void pngRead(png_structp pngp, png_bytep data, png_size_t length)
    {
        InputStream* stream = reinterpret_cast<InputStream*>(png_get_io_ptr(pngp));
        size_t bytesRead = stream->read(data, length);
        if (bytesRead != length) {
            Z_LOG("Unable to decode PNG file \"" << stream->name() << "\": unexpected end of stream.");
            longjmp(png_jmpbuf(pngp), 1);
        }
    }

    bool PNGImageReader::canReadImage(InputStream* stream) const
    {
        if (!stream)
            return false;

        png_byte buf[8];
        if (stream->read(buf, sizeof(buf)) != sizeof(buf))
            return false;

        return png_sig_cmp(buf, 0, 8) == 0;
    }

    Image* PNGImageReader::readImage(InputStream* stream) const
    {
        struct Context
        {
            png_structp pngp;
            png_infop infop;
            ~Context() { png_destroy_read_struct(&pngp, &infop, nullptr); }
        };

        if (!stream)
            return nullptr;

        volatile Context context;
        png_structp pngp = context.pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!pngp) {
            Z_LOG("Unable to decode PNG file \"" << stream->name() << "\": out of memory.");
            return nullptr;
        }

        if (setjmp(png_jmpbuf(pngp)))
            return nullptr;

        png_set_error_fn(pngp, stream, pngError, pngWarning);
        png_set_read_fn(pngp, stream, pngRead);

        png_infop infop = context.infop = png_create_info_struct(pngp);
        if (!infop) {
            Z_LOG("Unable to decode PNG file \"" << stream->name() << "\": out of memory.");
            return nullptr;
        }

        png_read_info(pngp, infop);

        int bitdepth = png_get_bit_depth(pngp, infop);
        switch (png_get_color_type(pngp, infop))
        {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngp);
            break;

        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (bitdepth < 8)
                png_set_expand_gray_1_2_4_to_8(pngp);
            break;
        }

        if (png_get_valid(pngp, infop, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(pngp);

        if (bitdepth == 16)
            png_set_scale_16(pngp);
        else if (bitdepth < 8)
            png_set_packing(pngp);

        double gamma = 1.0;
        if (png_get_gAMA(pngp, infop, &gamma))
            png_set_gamma(pngp, 2.2, gamma);

        int numPasses = 0;
        if (png_get_interlace_type(pngp, infop) != PNG_INTERLACE_NONE)
            numPasses = png_set_interlace_handling(pngp);

        png_read_update_info(pngp, infop);

        png_uint_32 width = png_get_image_width(pngp, infop);
        png_uint_32 height = png_get_image_height(pngp, infop);
        png_size_t rowbytes = png_get_rowbytes(pngp, infop);
        png_byte depth = png_get_bit_depth(pngp, infop);
        png_byte channels = png_get_channels(pngp, infop);

        if (depth != 8) {
            Z_LOG("Unable to decode PNG file \"" << stream->name()
                << "\": image has invalid bit depth (" << depth << ").");
            return nullptr;
        }

        Image::Format format;
        size_t rowLength;
        switch (channels)
        {
        case 1: format = Image::Format::Luminance8; rowLength = width; break;
        case 2: format = Image::Format::LuminanceAlpha8; rowLength = width * 2; break;
        case 3: format = Image::Format::RGB8; rowLength = width * 3; break;
        case 4: format = Image::Format::RGBA8; rowLength = width * 4; break;
        default:
            Z_LOG("Unable to decode PNG file \"" << stream->name()
                << "\": image has unsupported number of channels (" << channels << ").");
            return nullptr;
        }

        std::unique_ptr<Image> image(new Image(int(width), int(height), format, rowLength * height));
        if (rowLength >= rowbytes) {
            for (int i = 0; i < numPasses; i++) {
                for (png_uint_32 y = 0; y < height; y++) {
                    uint8_t* row = image->data() + y * rowLength;
                    png_read_row(pngp, reinterpret_cast<png_bytep>(row), nullptr);
                }
            }
        } else {
            std::unique_ptr<uint8_t[]> row(new uint8_t[rowbytes]);
            for (int i = 0; i < numPasses; i++) {
                for (png_uint_32 y = 0; y < height; y++) {
                    png_read_row(pngp, reinterpret_cast<png_bytep>(row.get()), nullptr);
                    uint8_t* dst = image->data() + y * rowLength;
                    memcpy(dst, row.get(), rowLength);
                }
            }
        }

        png_read_end(pngp, NULL);

        return image.release();
    }
}
