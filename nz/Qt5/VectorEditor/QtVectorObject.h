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
#include "QtVectorSceneItem.h"
#include <vector>

namespace Z
{
    class QtVectorObjectKind;
    class QtVectorControlPoint;
    class QtVectorScene;

    class QtVectorObject : public QtVectorSceneItem
    {
        Q_OBJECT
        Q_INTERFACES(QGraphicsItem)

    public:
        enum { Type = QtVectorSceneItem::ObjectType };

        QtVectorObject(QtVectorObjectKind* kind, QtVectorScene* scene);
        QtVectorObject(QtVectorObjectKind* kind, QtVectorObject* parent);
        ~QtVectorObject();

        int type() const override { return Type; }
        QString name() const override;

        bool isSelectedForUser() const;

        void setNumControlPoints(size_t count);
        QtVectorControlPoint* controlPoint(size_t index);

        QRectF boundingRect() const override;

    protected:
        void initPropertyList(QtPropertyList* propertyList) override;

        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

        bool sceneEventFilter(QGraphicsItem* watched, QEvent* event) override;

    private:
        QtVectorObjectKind* m_Kind;
        QGraphicsRectItem* m_ControlPointsContainer;
        QObject* m_SpecificData;
        std::vector<QtVectorControlPoint*> m_ControlPoints;

        void init();

        Q_DISABLE_COPY(QtVectorObject);

        friend class QtVectorScene;
    };
}
