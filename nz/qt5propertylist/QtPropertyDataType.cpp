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
#include "QtPropertyDataType.h"
#include <memory>
#include <unordered_map>
#include <cassert>

namespace Z
{
    namespace
    {
        struct BoolType : public QtPropertyDataType {
            QVariant formatForDisplay(const QVariant& data) const override {
                return data.toBool() ? QObject::tr("true") : QObject::tr("false");
            }
        };

        struct IntType : public QtPropertyDataType {
            QVariant formatForDisplay(const QVariant& data) const override {
                return data.toInt();
            }
        };

        struct FloatType : public QtPropertyDataType {
            const int decimals;
            explicit FloatType(int d) : decimals(d) {}

            QVariant formatForDisplay(const QVariant& data) const override {
                return QString::number(data.toFloat(), 'f', decimals);
            }
        };

        struct StringType : public QtPropertyDataType {
            QVariant formatForDisplay(const QVariant& data) const override {
                return data.toString();
            }
        };

        static const BoolType boolType = {};
        static const IntType intType = {};
        static const StringType stringType = {};

        static std::unordered_map<int, std::unique_ptr<FloatType>> floatTypes;
    };

    const QtPropertyDataType* const QtPropertyDataType::Bool = &boolType;
    const QtPropertyDataType* const QtPropertyDataType::Int = &intType;
    const QtPropertyDataType* const QtPropertyDataType::String = &stringType;

    const QtPropertyDataType* QtPropertyDataType::floatType(int precision)
    {
        assert(precision >= 0);

        std::unique_ptr<FloatType>& ptr = floatTypes[precision];
        if (!ptr)
            ptr.reset(new FloatType(precision));

        return ptr.get();
    }
}
