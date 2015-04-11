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
#include "QtVectorObject.h"
#include "QtVectorObjectKind.h"
#include "QtVectorScene.h"
#include "qt5propertylist/QtPropertyDataType.h"
#include "qt5propertylist/QtPropertyList.h"
#include <cassert>

namespace Z
{
    QtVectorObject::QtVectorObject(QtVectorObjectKind* kind, QtVectorScene* scene)
        : QObject(scene)
        , m_Kind(kind)
        , m_SpecificData(nullptr)
    {
        assert(kind != nullptr);
        assert(scene != nullptr);
        scene->addItem(this);
        init();
    }

    QtVectorObject::QtVectorObject(QtVectorObjectKind* kind, QtVectorObject* parent)
        : QObject((assert(parent != nullptr), parent->scene()))
        , QGraphicsItem(parent)
        , m_Kind(kind)
        , m_SpecificData(nullptr)
    {
        assert(kind != nullptr);
        init();
    }

    QtVectorObject::~QtVectorObject()
    {
        m_Kind->cleanupObject(m_SpecificData);
        delete m_SpecificData;
    }

    void QtVectorObject::init()
    {
        m_PropertyList = new QtPropertyList(this);
        int group = m_PropertyList->addGroup(tr("Object"));

        m_XProperty = m_PropertyList->addProperty(group, tr("x"), QtPropertyDataType::floatType(2));
        m_XProperty->setValue(x());
        connect(m_XProperty, SIGNAL(valueEdited()), SLOT(xEdited()));

        m_YProperty = m_PropertyList->addProperty(group, tr("y"), QtPropertyDataType::floatType(2));
        m_YProperty->setValue(y());
        connect(m_YProperty, SIGNAL(valueEdited()), SLOT(yEdited()));

        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);

        m_SpecificData = m_Kind->createDataForObject(this);
        m_Kind->initObject(m_SpecificData);
    }

    QtVectorScene* QtVectorObject::scene() const
    {
        return qobject_cast<QtVectorScene*>(parent());
    }

    QtVectorObject* QtVectorObject::parent() const
    {
        auto p = parentItem();
        if (p && p->type() == Type)
            return static_cast<QtVectorObject*>(p);
        return nullptr;
    }

    void QtVectorObject::setParent(QtVectorObject* parent)
    {
        setParentItem(parent);
    }

    void QtVectorObject::prepareGeometryChange()
    {
        QGraphicsItem::prepareGeometryChange();
    }

    QRectF QtVectorObject::boundingRect() const
    {
        return m_Kind->boundingRectForObject(m_SpecificData);
    }

    void QtVectorObject::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
    {
        m_Kind->paintObject(m_SpecificData, painter);
    }

    QVariant QtVectorObject::itemChange(GraphicsItemChange change, const QVariant& value)
    {
        switch (change)
        {
        case QGraphicsItem::ItemPositionHasChanged:
            m_XProperty->setValue(x());
            m_YProperty->setValue(y());
            break;

        default:
            break;
        }

        return QGraphicsItem::itemChange(change, value);
    }

    void QtVectorObject::xEdited()
    {
        qreal pos = m_XProperty->value().toFloat();
        if (pos != x())
            setX(pos);
    }

    void QtVectorObject::yEdited()
    {
        qreal pos = m_YProperty->value().toFloat();
        if (pos != y())
            setY(pos);
    }
}
