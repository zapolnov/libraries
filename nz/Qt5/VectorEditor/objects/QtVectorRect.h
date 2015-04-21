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
#include "QtVectorObjectKind.h"

namespace Z
{
    class QtVectorControlPoint;
    class QtPropertyList;
    class QtPropertyListItem;

    class QtVectorRectData : public QObject
    {
        Q_OBJECT

    public:
        explicit QtVectorRectData(QtVectorObject* object);

        QtVectorObject* object() const { return m_Object; }

        void init(qreal x, qreal y, qreal w, qreal h);
        Q_SLOT void updateControlPoints();

        qreal x() const { return m_X; }
        qreal y() const { return m_Y; }
        qreal width() const { return m_Width; }
        qreal height() const { return m_Height; }

        void setX(qreal x);
        void setY(qreal y);
        void setWidth(qreal w);
        void setHeight(qreal h);

    signals:
        void setXValue(const QVariant&);
        void setYValue(const QVariant&);
        void setWidthValue(const QVariant&);
        void setHeightValue(const QVariant&);

    private:
        enum {
            ChangingX       = 0b00000000001,
            ChangingY       = 0b00000000010,
            ChangingWidth   = 0b00000000100,
            ChangingHeight  = 0b00000001000,
            ChangingTL      = 0b00000010000,
            ChangingTR      = 0b00000100000,
            ChangingBL      = 0b00001000000,
            ChangingBR      = 0b00010000000,
        };

        QtVectorObject* m_Object;
        qreal m_X = 0.0f;
        qreal m_Y = 0.0f;
        qreal m_Width = 0.0f;
        qreal m_Height = 0.0f;
        int m_Flags = 0;

        void rawSetX(qreal x);
        void rawSetY(qreal y);
        void rawSetWidth(qreal w);
        void rawSetHeight(qreal h);

        Q_SLOT void xChanged();
        Q_SLOT void yChanged();
        Q_SLOT void widthChanged();
        Q_SLOT void heightChanged();

        Q_SLOT void xEdited();
        Q_SLOT void yEdited();
        Q_SLOT void widthEdited();
        Q_SLOT void heightEdited();

        Q_SLOT void topLeftControlPointPositionChanged();
        Q_SLOT void topRightControlPointPositionChanged();
        Q_SLOT void bottomLeftControlPointPositionChanged();
        Q_SLOT void bottomRightControlPointPositionChanged();
    };

    class QtVectorRect : public QtVectorObjectKind
    {
    public:
        QObject* createDataForObject(QtVectorObject* object) const override;
        void initObject(QObject* data) const override;
        void initPropertyListForObject(QObject* data, QtPropertyList* propertyList) const override;
        void cleanupObject(QObject* data) const override;

        QString nameForObject(const QObject* data) const override;
        bool objectCanBeRotated(const QObject* data) const override { return true; }
        bool objectCanBeScaled(const QObject* data) const override { return true; }
        QRectF boundingRectForObject(const QObject *data) const override;

        void paintObject(const QObject* data, QPainter* painter) const override;

    protected:
        void addXProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const;
        void addYProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const;
        void addWidthProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const;
        void addHeightProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const;
    };
}
