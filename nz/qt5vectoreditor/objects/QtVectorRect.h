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
#include "../QtVectorObjectKind.h"

namespace Z
{
    class QtPropertyListItem;

    class QtVectorRectData : public QObject
    {
        Q_OBJECT

    public:
        explicit QtVectorRectData(QtVectorObject* object);

        QtVectorObject* object() const { return m_Object; }

        qreal width() const { return m_Width; }
        qreal height() const { return m_Height; }
        void setWidth(qreal w);
        void setHeight(qreal h);

    private:
        QtVectorObject* m_Object;
        QtPropertyListItem* m_WidthProperty;
        QtPropertyListItem* m_HeightProperty;
        qreal m_Width = 0.0f;
        qreal m_Height = 0.0f;

        Q_SLOT void widthChanged();
        Q_SLOT void heightChanged();
        Q_SLOT void widthEdited();
        Q_SLOT void heightEdited();
    };

    class QtVectorRect : public QtVectorObjectKind
    {
    public:
        QObject* createDataForObject(QtVectorObject* object) const override;
        void initObject(QObject* data) const override;
        void cleanupObject(QObject* data) const override;

        QRectF boundingRectForObject(const QObject *data) const override;
        void paintObject(const QObject* data, QPainter* painter) const override;
    };
}
