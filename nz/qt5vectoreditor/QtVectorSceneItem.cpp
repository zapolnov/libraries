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
#include "QtVectorSceneItem.h"
#include "QtVectorScene.h"
#include "QtVectorUndo.h"
#include "QtVectorPoint.h"
#include "qt5util/QtMergeableUndoCommand.h"
#include "qt5propertylist/QtPropertyDataType.h"
#include "qt5propertylist/QtPropertyList.h"
#include <ctime>

namespace Z
{
    QtVectorSceneItem::QtVectorSceneItem(QtVectorScene* scene)
        : QGraphicsObject(nullptr)
    {
        Q_ASSERT(scene != nullptr);
        m_Scene = scene;
        scene->addItem(this);
        init();
    }

    QtVectorSceneItem::QtVectorSceneItem(QtVectorSceneItem* parent)
        : QGraphicsObject(parent)
    {
        Q_ASSERT(parent != nullptr);
        m_Scene = parent->scene();
        init();
    }

    void QtVectorSceneItem::init()
    {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
    }

    QtVectorSceneItem::~QtVectorSceneItem()
    {
    }

    QString QtVectorSceneItem::titleName() const
    {
        QString text = name();
        if (text.length() > 0)
            text[0] = text[0].toTitleCase();
        return text;
    }

    QtPropertyList* QtVectorSceneItem::propertyList() const
    {
        if (!m_PropertyList) {
            auto self = const_cast<QtVectorSceneItem*>(this);
            m_PropertyList = new QtPropertyList(self);
            self->initPropertyList(m_PropertyList);
        }
        return m_PropertyList;
    }

    QtPropertyListItem* QtVectorSceneItem::addXProperty(QtPropertyList* propertyList, int groupIndex) const
    {
        auto property = propertyList->addProperty(groupIndex, tr("x"), QtPropertyDataType::floatType(2));
        property->setValue(x());
        connect(this, SIGNAL(setXValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        connect(property, SIGNAL(valueEdited()), SLOT(xEdited()));
        connect(property, SIGNAL(valueChanged()), SLOT(xPropertyChanged()));
        return property;
    }

    QtPropertyListItem* QtVectorSceneItem::addYProperty(QtPropertyList* propertyList, int groupIndex) const
    {
        auto property = m_PropertyList->addProperty(groupIndex, tr("y"), QtPropertyDataType::floatType(2));
        property->setValue(y());
        connect(this, SIGNAL(setYValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        connect(property, SIGNAL(valueEdited()), SLOT(yEdited()));
        connect(property, SIGNAL(valueChanged()), SLOT(yPropertyChanged()));
        return property;
    }

    void QtVectorSceneItem::setPosWithoutUndo(qreal x, qreal y)
    {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
        setPos(x, y);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
        emit positionChanged();
    }

    void QtVectorSceneItem::addUndoCommandForMove(const QString& text, qreal newX, qreal newY, bool allowMerge)
    {
        struct Data : Z::QtMergeableUndoCommand::Data {
            QtVectorSceneItem* object;
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
                m_Changing = true;
                emit setXValue(data->newX);
                emit setYValue(data->newY);
                m_Changing = false;
                setPosWithoutUndo(data->newX, data->newY);
            }, [this, data]() {
                m_Changing = true;
                emit setXValue(data->oldX);
                emit setYValue(data->oldY);
                m_Changing = false;
                setPosWithoutUndo(data->oldX, data->oldY);
            }, [this, data](const QtMergeableUndoCommand::Data* other, QString& text, const QString& otherText) {
                auto oth = static_cast<const Data*>(other);
                if (data->object != oth->object || abs(int(data->time - oth->time)) > UNDO_AUTOMERGE_SECONDS)
                    return false;
                data->newX = oth->newX;
                data->newY = oth->newY;
                data->time = std::max(data->time, oth->time);
                text = otherText;
                return true;
            }
        );
        undoCommand->setAllowMerging(allowMerge);

        Q_ASSERT(scene() != nullptr);
        scene()->undoStack()->push(undoCommand);
    }

    void QtVectorSceneItem::initPropertyList(QtPropertyList* propertyList)
    {
        int group = propertyList->addGroup(titleName());
        addXProperty(propertyList, group);
        addYProperty(propertyList, group);
    }

    QVariant QtVectorSceneItem::itemChange(GraphicsItemChange change, const QVariant& value)
    {
        switch (change)
        {
        case QGraphicsItem::ItemPositionChange:
          {
            QVariant v = QGraphicsObject::itemChange(change, value);
            QPointF newPos = v.value<QPointF>();
            if (x() != newPos.x() || y() != newPos.y()) {
                m_Changing = true;
                emit setXValue(newPos.x());
                emit setYValue(newPos.y());
                m_Changing = false;
                QString text = tr("Move %1 to (%2, %3).").arg(name()).arg(newPos.x()).arg(newPos.y());
                addUndoCommandForMove(text, newPos.x(), newPos.y(), true);
            }
            return v;
          }

        case QGraphicsItem::ItemPositionHasChanged:
            emit positionChanged();
            break;

        default:
            break;
        }

        return QGraphicsItem::itemChange(change, value);
    }

    void QtVectorSceneItem::xEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        qreal pos = property->value().toFloat();
        if (pos != x()) {
            QString text = tr("Change %1 to %2.").arg(property->name()).arg(pos);
            addUndoCommandForMove(text, pos, y(), false);
            setPosWithoutUndo(pos, y());
        }
    }

    void QtVectorSceneItem::yEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        qreal pos = property->value().toFloat();
        if (pos != y()) {
            QString text = tr("Change %1 to %2.").arg(property->name()).arg(pos);
            addUndoCommandForMove(text, x(), pos, false);
            setPosWithoutUndo(x(), pos);
        }
    }

    void QtVectorSceneItem::xPropertyChanged()
    {
        if (!m_Changing) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            setPosWithoutUndo(property->value().toFloat(), y());
        }
    }

    void QtVectorSceneItem::yPropertyChanged()
    {
        if (!m_Changing) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            setPosWithoutUndo(x(), property->value().toFloat());
        }
    }
}
