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

class QStandardItem;

namespace Z
{
    class QtPropertyList;
    class QtPropertyDataType;

    class QtPropertyListItem : public QObject
    {
        Q_OBJECT

    public:
        QtPropertyList* list() const;
        const QtPropertyDataType* dataType() const { return m_DataType; }

        const QVariant& value() const { return m_Value; }
        void setValue(const QVariant& data) { m_Value = data; emit valueChanged(); }

    signals:
        void valueChanged();
        void valueEdited();

    private:
        QStandardItem* m_NameItem = nullptr;
        QStandardItem* m_ValueItem = nullptr;
        const QtPropertyDataType* m_DataType = nullptr;
        QVariant m_Value;

        explicit QtPropertyListItem(QtPropertyList* list);
        Q_DISABLE_COPY(QtPropertyListItem)

        friend class QtPropertyList;
    };
}
