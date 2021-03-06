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
#include <functional>

namespace Z
{
    class QtVectorObject;

    class QtVectorControlPoint : public QtVectorSceneItem
    {
        Q_OBJECT

    public:
        enum { Type = ControlPointType };

        std::function<void(qreal& x, qreal& y)> movementValidator;

        explicit QtVectorControlPoint(QtVectorScene* scene);
        explicit QtVectorControlPoint(QGraphicsItem* parent);
        ~QtVectorControlPoint() = default;

        int type() const override { return Type; }
        QString name() const override;

        QtVectorObject* ownerObject() const { return m_OwnerObject; }

        QRectF boundingRect() const override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    private:
        QtVectorObject* m_OwnerObject = nullptr;

        void init();

        friend class QtVectorObject;
    };
}
