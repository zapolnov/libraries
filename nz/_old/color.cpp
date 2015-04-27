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
#include "color.h"
#include <algorithm>

namespace Z
{
    Color colorLuminance(float luminance)
    {
        return { luminance, luminance, luminance, 1.0f };
    }

    Color colorLuminanceAlpha(float luminance, float alpha)
    {
        return { luminance, luminance, luminance, alpha };
    }

    Color colorRGB(float r, float g, float b)
    {
        return { r, g, b, 1.0f };
    }

    Color colorRGBA(float r, float g, float b, float a)
    {
        return { r, g, b, a };
    }

    float colorClampFloat(float value)
    {
        return glm::clamp(value, 0.0f, 1.0f);
    }

    Color colorClamp(const Color& color)
    {
        static const glm::vec4 minValue = { 0.0f, 0.0f, 0.0f, 0.0f };
        static const glm::vec4 maxValue = { 1.0f, 1.0f, 1.0f, 1.0f };
        return glm::clamp(color, minValue, maxValue);
    }

    uint8_t colorFloatToByte(float value)
    {
        value = colorClampFloat(value) * 255.0f;
        int ivalue = int(value);
        return uint8_t(glm::clamp(ivalue, 0, 255));
    }

    float colorByteToFloat(uint8_t value)
    {
        return colorClampFloat(float(value) / 255.0f);
    }

    float colorRGBToLuminance(const Color& color)
    {
        Color c = colorClamp(color);
        return colorClampFloat(0.299f * c.r + 0.587f * c.g + 0.114f * c.b);
    }

    Color colorPremultiplyAlpha(const Color& color)
    {
        Color c = colorClamp(color);
        return { c.r * c.a, c.g * c.a, c.b * c.a, c.a };
    }

    Color colorUnpremultiplyAlpha(const Color& color)
    {
        Color c = colorClamp(color);
        if (c.a == 0.0f)
            return c;
        return { c.r / c.a, c.g / c.a, c.b / c.a, c.a };
    }
}
