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
#include "QtVectorControlPoint.h"

namespace Z
{
    QtVectorObject::QtVectorObject(QtVectorObjectKind* kind, QtVectorScene* scene)
        : QtVectorSceneItem(scene)
        , m_Kind(kind)
    {
        init();
    }

    QtVectorObject::QtVectorObject(QtVectorObjectKind* kind, QtVectorObject* parent)
        : QtVectorSceneItem(parent)
        , m_Kind(kind)
    {
        init();
    }

    void QtVectorObject::init()
    {
        setFlag(ItemSendsScenePositionChanges);

        m_ControlPointsContainer = new QGraphicsRectItem;
        m_ControlPointsContainer->setFlags(ItemHasNoContents);
        m_ControlPointsContainer->setVisible(false);
        scene()->addItem(m_ControlPointsContainer);
        m_ControlPointsContainer->installSceneEventFilter(this);

        Q_ASSERT(m_Kind != nullptr);
        m_SpecificData = m_Kind->createDataForObject(this);
        m_Kind->initObject(m_SpecificData);
    }

    QtVectorObject::~QtVectorObject()
    {
        m_Kind->cleanupObject(m_SpecificData);
        delete m_SpecificData;

        auto s = scene();
        if (s->currentControlPoints() == m_ControlPointsContainer)
            s->setCurrentControlPoints(nullptr);

        delete m_ControlPointsContainer;
        m_ControlPointsContainer = nullptr;
    }

    QString QtVectorObject::name() const
    {
        return m_Kind->nameForObject(m_SpecificData);
    }

    bool QtVectorObject::isSelectedForUser() const
    {
        if (isSelected())
            return true;

        for (auto cp : m_ControlPoints) {
            if (cp && cp->isSelected())
                return true;
        }

        return false;
    }

    void QtVectorObject::setNumControlPoints(size_t count)
    {
        if (count < m_ControlPoints.size()) {
            for (size_t i = count; i < m_ControlPoints.size(); i++)
                delete m_ControlPoints[i];
        }
        m_ControlPoints.resize(count);
    }

    QtVectorControlPoint* QtVectorObject::controlPoint(size_t index)
    {
        Q_ASSERT(index < m_ControlPoints.size());
        QtVectorControlPoint*& point = m_ControlPoints[index];
        if (!point) {
            point = new QtVectorControlPoint(m_ControlPointsContainer);
            point->m_OwnerObject = this;
        }
        return point;
    }

    QRectF QtVectorObject::boundingRect() const
    {
        return m_Kind->boundingRectForObject(m_SpecificData);
    }

    void QtVectorObject::initPropertyList(QtPropertyList* propertyList)
    {
        m_Kind->initPropertyListForObject(m_SpecificData, propertyList);
    }

    QVariant QtVectorObject::itemChange(GraphicsItemChange change, const QVariant& value)
    {
        switch (change)
        {
        case ItemPositionHasChanged:
        case ItemRotationHasChanged:
        case ItemScaleHasChanged:
        case ItemScenePositionHasChanged:
            m_ControlPointsContainer->setTransform(sceneTransform());
            break;

        default:
            break;
        }

        return QtVectorSceneItem::itemChange(change, value);
    }

    void QtVectorObject::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
    {
        m_ControlPointsContainer->setRect(boundingRect());
        m_Kind->paintObject(m_SpecificData, painter);
    }

    bool QtVectorObject::sceneEventFilter(QGraphicsItem* watched, QEvent* event)
    {
        //sceneEvent(event);
        //return true;
        return false;
    }
}
