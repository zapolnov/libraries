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
#include <QObject>
#include <QVariant>

class QWidget;
class QPainter;
class QStyle;
class QStyleOptionViewItem;

namespace Z
{
    class QtPropertyDataType
    {
    public:
        static const QtPropertyDataType* const Bool;
        static const QtPropertyDataType* const Int;
        static const QtPropertyDataType* const String;
        static const QtPropertyDataType* floatType(int precision);
        static const QtPropertyDataType* rangedFloatType(int precision, float mn, float mx);

        virtual ~QtPropertyDataType() = default;

        virtual QVariant formatForDisplay(const QVariant& data) const;
        virtual void paint(QPainter* painter, const QVariant& data,
            QStyle* style, const QStyleOptionViewItem& option) const;

        virtual QWidget* createEditor(QWidget* parent) const;
        virtual QVariant getEditorData(QWidget* editor) const;
        virtual void setEditorData(QWidget* editor, const QVariant& data) const;
    };
}
