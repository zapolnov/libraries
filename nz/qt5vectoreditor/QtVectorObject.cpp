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
#include "QtVectorUndo.h"
#include "qt5util/QtMergeableUndoCommand.h"
#include "qt5propertylist/QtPropertyDataType.h"
#include "qt5propertylist/QtPropertyList.h"

namespace Z
{
    QtVectorObject::QtVectorObject(QtVectorObjectKind* kind, QtVectorScene* scene)
        : QObject(scene)
        , m_Kind(kind)
        , m_SpecificData(nullptr)
    {
        Q_ASSERT(kind != nullptr);
        Q_ASSERT(scene != nullptr);
        scene->addItem(this);
        init();
    }

    QtVectorObject::QtVectorObject(QtVectorObjectKind* kind, QtVectorObject* parent)
        : QObject((Q_ASSERT(parent != nullptr), parent->scene()))
        , QGraphicsItem(parent)
        , m_Kind(kind)
        , m_SpecificData(nullptr)
    {
        Q_ASSERT(kind != nullptr);
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
        return qobject_cast<QtVectorScene*>(QObject::parent());
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
        case QGraphicsItem::ItemPositionChange:
          {
            QVariant v = QGraphicsItem::itemChange(change, value);
            QPointF newPos = v.value<QPointF>();
            if (x() != newPos.x() || y() != newPos.y()) {
                m_XProperty->setValue(newPos.x());
                m_YProperty->setValue(newPos.y());
                QString text = tr("Move item to (%1, %2).").arg(newPos.x()).arg(newPos.y());
                addUndoCommandForMove(text, newPos.x(), newPos.y(), true);
            }
            return v;
          }

        default:
            break;
        }

        return QGraphicsItem::itemChange(change, value);
    }

    void QtVectorObject::setPosWithoutNotification(qreal x, qreal y)
    {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
        setPos(x, y);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    }

    void QtVectorObject::addUndoCommandForMove(const QString& text, qreal newX, qreal newY, bool allowMerge)
    {
        struct Data : Z::QtMergeableUndoCommand::Data {
            QtVectorObject* object;
            qreal oldX, oldY, newX, newY;
            time_t time;
        };

        Data* data = new Data;
        data->object = this;
        data->oldX = x();
        data->oldY = y();
        data->newX = newX;
        data->newY = newY;
        data->time = time(nullptr);

        auto undoCommand = Z::QtMergeableUndoCommand::create(
            text,
            int(QtVectorUndo::ItemMove),
            data,
            [this, data]() {
                m_XProperty->setValue(data->newX);
                m_YProperty->setValue(data->newY);
                setPosWithoutNotification(data->newX, data->newY);
            }, [this, data]() {
                m_XProperty->setValue(data->oldX);
                m_YProperty->setValue(data->oldY);
                setPosWithoutNotification(data->oldX, data->oldY);
            }, [this, data](const QtMergeableUndoCommand::Data* other, QString& text, const QString& otherText) {
                auto oth = static_cast<const Data*>(other);
                if (data->object != oth->object || abs(int(data->time - oth->time)) > 3)
                    return false;
                data->newX = oth->newX;
                data->newY = oth->newY;
                data->time = std::max(data->time, oth->time);
                text = otherText;
                return true;
            }
        );

        undoCommand->setAllowMerging(allowMerge);
        scene()->undoStack()->push(undoCommand);
    }

    void QtVectorObject::xEdited()
    {
        qreal pos = m_XProperty->value().toFloat();
        if (pos != x()) {
            QString text = tr("Change %1 to %2.").arg("x").arg(pos);
            addUndoCommandForMove(text, pos, y(), false);
            setPosWithoutNotification(pos, y());
        }
    }

    void QtVectorObject::yEdited()
    {
        qreal pos = m_YProperty->value().toFloat();
        if (pos != y()) {
            QString text = tr("Change %1 to %2.").arg("y").arg(pos);
            addUndoCommandForMove(text, x(), pos, false);
            setPosWithoutNotification(x(), pos);
        }
    }
}
