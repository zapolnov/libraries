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
        auto propertyList = m_Object->propertyList();
        int group = propertyList->addGroup(tr("Rectangle"));

        m_WidthProperty = propertyList->addProperty(group, tr("width"), QtPropertyDataType::floatType(2));
        connect(m_WidthProperty, SIGNAL(valueChanged()), SLOT(widthChanged()));
        connect(m_WidthProperty, SIGNAL(valueEdited()), SLOT(widthEdited()));

        m_HeightProperty = propertyList->addProperty(group, tr("height"), QtPropertyDataType::floatType(2));
        connect(m_HeightProperty, SIGNAL(valueChanged()), SLOT(heightChanged()));
        connect(m_HeightProperty, SIGNAL(valueEdited()), SLOT(heightEdited()));
     }

    void QtVectorRectData::setWidth(qreal w)
    {
        m_WidthProperty->setValue(w);
    }

    void QtVectorRectData::setHeight(qreal h)
    {
        m_HeightProperty->setValue(h);
    }

    void QtVectorRectData::widthChanged()
    {
        float w = m_WidthProperty->value().toFloat();
        if (m_Width != w) {
            m_Object->prepareGeometryChange();
            m_Width = w;
        }
    }

    void QtVectorRectData::heightChanged()
    {
        float h = m_HeightProperty->value().toFloat();
        if (m_Height != h) {
            m_Object->prepareGeometryChange();
            m_Height = h;
        }
    }

    void QtVectorRectData::widthEdited()
    {
        float w = m_WidthProperty->value().toFloat();
        if (m_Width != w) {
            addUndoCommandForProperty(m_Object->scene(), m_WidthProperty, m_Width, w, false);
            m_Object->prepareGeometryChange();
            m_Width = w;
        }
    }

    void QtVectorRectData::heightEdited()
    {
        float h = m_HeightProperty->value().toFloat();
        if (m_Height != h) {
            addUndoCommandForProperty(m_Object->scene(), m_HeightProperty, m_Height, h, false);
            m_Object->prepareGeometryChange();
            m_Height = h;
        }
    }


    QObject* QtVectorRect::createDataForObject(QtVectorObject* object) const
    {
        return new QtVectorRectData(object);
    }

    void QtVectorRect::initObject(QObject* data) const
    {
        auto r = static_cast<QtVectorRectData*>(data);
        r->setWidth(50.0f);
        r->setHeight(50.0f);
    }

    void QtVectorRect::cleanupObject(QObject*) const
    {
    }

    QRectF QtVectorRect::boundingRectForObject(const QObject* data) const
    {
        auto r = static_cast<const QtVectorRectData*>(data);
        return QRectF(0.0f, 0.0f, r->width(), r->height());
    }

    void QtVectorRect::paintObject(const QObject* data, QPainter* painter) const
    {
        auto r = static_cast<const QtVectorRectData*>(data);
        painter->setPen(Qt::green);
        painter->setBrush(Qt::blue);
        painter->drawRect(0.0f, 0.0f, r->width(), r->height());
    }
}
