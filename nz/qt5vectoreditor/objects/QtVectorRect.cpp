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
#include "QtVectorRect.h"
#include "QtVectorObject.h"
#include "../QtVectorControlPoint.h"
#include "../QtVectorUndo.h"
#include "qt5propertylist/QtPropertyDataType.h"
#include "qt5propertylist/QtPropertyListItem.h"
#include "qt5propertylist/QtPropertyList.h"
#include <QPainter>

namespace Z
{
    QtVectorRectData::QtVectorRectData(QtVectorObject* object)
        : m_Object(object)
    {
        m_Object->setNumControlPoints(4);
    }

    void QtVectorRectData::init(qreal x, qreal y, qreal w, qreal h)
    {
        m_Object->prepareGeometryChange();
        m_X = x;
        m_Y = y;
        m_Width = w;
        m_Height = h;

        connect(m_Object->controlPoint(0), SIGNAL(positionChanged()), SLOT(topLeftControlPointPositionChanged()));
        connect(m_Object->controlPoint(1), SIGNAL(positionChanged()), SLOT(topRightControlPointPositionChanged()));
        connect(m_Object->controlPoint(2), SIGNAL(positionChanged()), SLOT(bottomLeftControlPointPositionChanged()));
        connect(m_Object->controlPoint(3), SIGNAL(positionChanged()), SLOT(bottomRightControlPointPositionChanged()));

        auto adjust = [](qreal& value, qreal prev, qreal size) {
            float newSize = size + (prev - value);
            if (newSize < 0.0f)
                value = prev + size;
        };

        auto limit = [](qreal& value, qreal start, qreal size) {
            float newSize = value - start;
            if (newSize < 0.0f)
                value = start;
        };

        m_Object->controlPoint(0)->movementValidator = [this, adjust](qreal& x, qreal& y)
            { adjust(x, m_X, m_Width); adjust(y, m_Y, m_Height); };
        m_Object->controlPoint(1)->movementValidator = [this, adjust, limit](qreal& x, qreal& y)
            { limit(x, m_X, m_Width); adjust(y, m_Y, m_Height); };
        m_Object->controlPoint(2)->movementValidator = [this, adjust, limit](qreal& x, qreal& y)
            { adjust(x, m_X, m_Width); limit(y, m_Y, m_Height); };
        m_Object->controlPoint(3)->movementValidator = [this, limit](qreal& x, qreal& y)
            { limit(x, m_X, m_Width); limit(y, m_Y, m_Height); };

        updateControlPoints();
    }

    void QtVectorRectData::updateControlPoints()
    {
        qreal x1 = m_X;
        qreal y1 = m_Y;
        qreal x2 = x1 + m_Width;
        qreal y2 = y1 + m_Height;

        if (!(m_Flags & ChangingTL)) {
            m_Flags |= ChangingTL;
            m_Object->controlPoint(0)->setPosWithoutUndo(x1, y1);
            m_Flags &= ~ChangingTL;
        }

        if (!(m_Flags & ChangingTR)) {
            m_Flags |= ChangingTR;
            m_Object->controlPoint(1)->setPosWithoutUndo(x2, y1);
            m_Flags &= ~ChangingTR;
        }

        if (!(m_Flags & ChangingBL)) {
            m_Flags |= ChangingBL;
            m_Object->controlPoint(2)->setPosWithoutUndo(x1, y2);
            m_Flags &= ~ChangingBL;
        }

        if (!(m_Flags & ChangingBR)) {
            m_Flags |= ChangingBR;
            m_Object->controlPoint(3)->setPosWithoutUndo(x2, y2);
            m_Flags &= ~ChangingBR;
        }
    }

    void QtVectorRectData::setX(qreal x)
    {
        rawSetX(x);

        m_Flags |= ChangingX;
        emit setXValue(x);
        m_Flags &= ~ChangingX;
    }

    void QtVectorRectData::setY(qreal y)
    {
        rawSetY(y);

        m_Flags |= ChangingY;
        emit setYValue(y);
        m_Flags &= ~ChangingY;
    }

    void QtVectorRectData::setWidth(qreal w)
    {
        rawSetWidth(w);

        m_Flags |= ChangingWidth;
        emit setWidthValue(w);
        m_Flags &= ~ChangingWidth;
    }

    void QtVectorRectData::setHeight(qreal h)
    {
        rawSetHeight(h);

        m_Flags |= ChangingHeight;
        emit setHeightValue(h);
        m_Flags &= ~ChangingHeight;
    }

    void QtVectorRectData::rawSetX(qreal x)
    {
        m_Object->prepareGeometryChange();
        m_X = x;
        updateControlPoints();
    }

    void QtVectorRectData::rawSetY(qreal y)
    {
        m_Object->prepareGeometryChange();
        m_Y = y;
        updateControlPoints();
    }

    void QtVectorRectData::rawSetWidth(qreal w)
    {
        m_Object->prepareGeometryChange();
        m_Width = w;
        updateControlPoints();
    }

    void QtVectorRectData::rawSetHeight(qreal h)
    {
        m_Object->prepareGeometryChange();
        m_Height = h;
        updateControlPoints();
    }

    void QtVectorRectData::xChanged()
    {
        if (!(m_Flags & ChangingX)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            rawSetX(property->value().toFloat());
        }
    }

    void QtVectorRectData::yChanged()
    {
        if (!(m_Flags & ChangingY)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            rawSetY(property->value().toFloat());
        }
    }

    void QtVectorRectData::widthChanged()
    {
        if (!(m_Flags & ChangingWidth)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            rawSetWidth(property->value().toFloat());
        }
    }

    void QtVectorRectData::heightChanged()
    {
        if (!(m_Flags & ChangingHeight)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            rawSetHeight(property->value().toFloat());
        }
    }

    void QtVectorRectData::xEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        float x = property->value().toFloat();
        if (m_X != x) {
            addUndoCommandForProperty(m_Object->scene(), property, m_X, x, false);
            rawSetX(x);
        }
    }

    void QtVectorRectData::yEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        float y = property->value().toFloat();
        if (m_Y != y) {
            addUndoCommandForProperty(m_Object->scene(), property, m_Y, y, false);
            rawSetY(y);
        }
    }

    void QtVectorRectData::widthEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        float w = property->value().toFloat();
        if (m_Width != w) {
            addUndoCommandForProperty(m_Object->scene(), property, m_Width, w, false);
            rawSetWidth(w);
        }
    }

    void QtVectorRectData::heightEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        float h = property->value().toFloat();
        if (m_Height != h) {
            addUndoCommandForProperty(m_Object->scene(), property, m_Height, h, false);
            rawSetHeight(h);
        }
    }

    void QtVectorRectData::topLeftControlPointPositionChanged()
    {
        if (!(m_Flags & ChangingTL)) {
            auto point = qobject_cast<QtVectorControlPoint*>(sender());
            m_Flags |= ChangingTL;
            setWidth(m_Width + (m_X - point->x()));
            setHeight(m_Height + (m_Y - point->y()));
            setX(point->x());
            setY(point->y());
            m_Flags &= ~ChangingTL;
        }
    }

    void QtVectorRectData::topRightControlPointPositionChanged()
    {
        if (!(m_Flags & ChangingTR)) {
            auto point = qobject_cast<QtVectorControlPoint*>(sender());
            m_Flags |= ChangingTR;
            setWidth(point->x() - m_X);
            setHeight(m_Height + (m_Y - point->y()));
            setY(point->y());
            m_Flags &= ~ChangingTR;
        }
    }

    void QtVectorRectData::bottomLeftControlPointPositionChanged()
    {
        if (!(m_Flags & ChangingBL)) {
            auto point = qobject_cast<QtVectorControlPoint*>(sender());
            m_Flags |= ChangingBL;
            setWidth(m_Width + (m_X - point->x()));
            setHeight(point->y() - m_Y);
            setX(point->x());
            m_Flags &= ~ChangingBL;
        }
    }

    void QtVectorRectData::bottomRightControlPointPositionChanged()
    {
        if (!(m_Flags & ChangingBR)) {
            auto point = qobject_cast<QtVectorControlPoint*>(sender());
            m_Flags |= ChangingBR;
            setWidth(point->x() - m_X);
            setHeight(point->y() - m_Y);
            m_Flags &= ~ChangingBR;
        }
    }


    QObject* QtVectorRect::createDataForObject(QtVectorObject* object) const
    {
        return new QtVectorRectData(object);
    }

    void QtVectorRect::initObject(QObject* data) const
    {
        auto r = static_cast<QtVectorRectData*>(data);
        r->init(-25, -25, 50, 50);
    }

    void QtVectorRect::initPropertyListForObject(QObject* data, QtPropertyList* propertyList) const
    {
        auto r = static_cast<QtVectorRectData*>(data);

        int group = propertyList->addGroup(QObject::tr("Position"));
        r->object()->addXProperty(propertyList, group);
        r->object()->addYProperty(propertyList, group);
        r->object()->addRotationProperty(propertyList, group);
        r->object()->addScaleProperty(propertyList, group);

        int group2 = propertyList->addGroup(QObject::tr("Bounding rectangle"));
        addXProperty(r, propertyList, group2);
        addYProperty(r, propertyList, group2);
        addWidthProperty(r, propertyList, group2);
        addHeightProperty(r, propertyList, group2);
    }

    void QtVectorRect::cleanupObject(QObject*) const
    {
    }

    QString QtVectorRect::nameForObject(const QObject* data) const
    {
        return QObject::tr("rectangle");
    }

    QRectF QtVectorRect::boundingRectForObject(const QObject* data) const
    {
        auto r = static_cast<const QtVectorRectData*>(data);
        return QRectF(r->x(), r->y(), r->width(), r->height());
    }

    void QtVectorRect::paintObject(const QObject* data, QPainter* painter) const
    {
        auto r = static_cast<const QtVectorRectData*>(data);
        painter->setPen(Qt::green);
        painter->setBrush(Qt::blue);
        painter->drawRect(boundingRectForObject(data));
    }

    void QtVectorRect::addXProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const
    {
        auto property = propertyList->addProperty(group, QObject::tr("x"), QtPropertyDataType::floatType(2));
        property->setValue(r->x());
        r->connect(r, SIGNAL(setXValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        r->connect(property, SIGNAL(valueChanged()), r, SLOT(xChanged()));
        r->connect(property, SIGNAL(valueEdited()), r, SLOT(xEdited()));
    }

    void QtVectorRect::addYProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const
    {
        auto property = propertyList->addProperty(group, QObject::tr("y"), QtPropertyDataType::floatType(2));
        property->setValue(r->y());
        r->connect(r, SIGNAL(setYValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        r->connect(property, SIGNAL(valueChanged()), r, SLOT(yChanged()));
        r->connect(property, SIGNAL(valueEdited()), r, SLOT(yEdited()));
    }

    void QtVectorRect::addWidthProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const
    {
        auto property = propertyList->addProperty(group, QObject::tr("width"),
            QtPropertyDataType::rangedFloatType(2, 0.0f, std::numeric_limits<float>::max()));
        property->setValue(r->width());
        r->connect(r, SIGNAL(setWidthValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        r->connect(property, SIGNAL(valueChanged()), r, SLOT(widthChanged()));
        r->connect(property, SIGNAL(valueEdited()), r, SLOT(widthEdited()));
    }

    void QtVectorRect::addHeightProperty(QtVectorRectData* r, QtPropertyList* propertyList, int group) const
    {
        auto property = propertyList->addProperty(group, QObject::tr("height"),
            QtPropertyDataType::rangedFloatType(2, 0.0f, std::numeric_limits<float>::max()));
        property->setValue(r->height());
        r->connect(r, SIGNAL(setHeightValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        r->connect(property, SIGNAL(valueChanged()), r, SLOT(heightChanged()));
        r->connect(property, SIGNAL(valueEdited()), r, SLOT(heightEdited()));
    }
}
