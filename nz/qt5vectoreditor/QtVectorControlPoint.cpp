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
#include "QtVectorControlPoint.h"
#include <QPainter>
#include <QPen>
#include <QCursor>

namespace Z
{
    static const float RADIUS = 3.0f;

    QtVectorControlPoint::QtVectorControlPoint(QtVectorScene* scene)
        : QtVectorSceneItem(scene)
    {
        init();
    }

    QtVectorControlPoint::QtVectorControlPoint(QGraphicsItem* parent)
        : QtVectorSceneItem(parent)
    {
        init();
    }

    void QtVectorControlPoint::init()
    {
        setFlag(ItemIgnoresTransformations);
        setVisible(true);
        setCursor(Qt::SizeAllCursor);
    }

    QString QtVectorControlPoint::name() const
    {
        return tr("control point");
    }

    QRectF QtVectorControlPoint::boundingRect() const
    {
        return QRectF(-RADIUS, -RADIUS, 2.0 * RADIUS, 2.0 * RADIUS);
    }

    void QtVectorControlPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
    {
        painter->setPen(QPen(Qt::white));
        painter->setBrush(Qt::yellow);
        painter->drawEllipse(boundingRect());
    }

    QVariant QtVectorControlPoint::itemChange(GraphicsItemChange change, const QVariant& value)
    {
        if (change == QGraphicsItem::ItemPositionChange && movementValidator)
        {
            QPointF newPos = value.value<QPointF>();
            movementValidator(newPos.rx(), newPos.ry());
            return QtVectorSceneItem::itemChange(change, QVariant(newPos));
        }
        return QtVectorSceneItem::itemChange(change, value);
    }
}
