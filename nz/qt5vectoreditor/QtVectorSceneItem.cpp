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
#include "QtVectorControlPoint.h"
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
        setParentItem(scene->canvas());
        init();
    }

    QtVectorSceneItem::QtVectorSceneItem(QGraphicsItem* parent)
        : QGraphicsObject(parent)
    {
        Q_ASSERT(parent != nullptr);
        m_Scene = qobject_cast<QtVectorScene*>(parent->scene());
        Q_ASSERT(m_Scene != nullptr);
        init();
    }

    void QtVectorSceneItem::init()
    {
        setFlag(ItemSendsGeometryChanges);
        setFlag(ItemIsMovable);
        setFlag(ItemIsSelectable);
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

    QtPropertyListItem* QtVectorSceneItem::addRotationProperty(QtPropertyList* propertyList, int groupIndex) const
    {
        auto property = propertyList->addProperty(groupIndex, tr("rotation"), QtPropertyDataType::floatType(2));
        property->setValue(rotation());
        connect(this, SIGNAL(setRotationValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        connect(property, SIGNAL(valueEdited()), SLOT(rotationEdited()));
        connect(property, SIGNAL(valueChanged()), SLOT(rotationPropertyChanged()));
        return property;
    }

    QtPropertyListItem* QtVectorSceneItem::addScaleProperty(QtPropertyList* propertyList, int groupIndex) const
    {
        auto property = propertyList->addProperty(groupIndex, tr("scale"), QtPropertyDataType::floatType(2));
        property->setValue(scale());
        connect(this, SIGNAL(setScaleValue(const QVariant&)), property, SLOT(setValue(const QVariant&)));
        connect(property, SIGNAL(valueEdited()), SLOT(scaleEdited()));
        connect(property, SIGNAL(valueChanged()), SLOT(scalePropertyChanged()));
        return property;
    }

    void QtVectorSceneItem::setPosWithoutUndo(qreal x, qreal y)
    {
        m_Flags |= PreventUndoPos;
        setPos(x, y);
        m_Flags &= ~PreventUndoPos;
        emit positionChanged();
    }

    void QtVectorSceneItem::setRotationWithoutUndo(qreal r)
    {
        m_Flags |= PreventUndoRotation;
        setRotation(r);
        m_Flags &= ~PreventUndoRotation;
        emit rotationChanged();
    }

    void QtVectorSceneItem::setScaleWithoutUndo(qreal s)
    {
        m_Flags |= PreventUndoScale;
        setScale(s);
        m_Flags &= ~PreventUndoScale;
        emit scaleChanged();
    }

    void QtVectorSceneItem::addUndoCommandForMove(const QString& text, qreal newX, qreal newY, bool allowMerge)
    {
        Z::QtMergeableUndoCommand* undoCommand;

        if (scene()->currentSingleSelection())
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

            undoCommand = Z::QtMergeableUndoCommand::create(
                text,
                int(QtVectorUndo::ItemMove),
                data,
                [this, data]() {
                    m_Flags |= ChangingX; emit setXValue(data->newX); m_Flags &= ~ChangingX;
                    m_Flags |= ChangingY; emit setYValue(data->newY); m_Flags &= ~ChangingY;
                    setPosWithoutUndo(data->newX, data->newY);
                }, [this, data]() {
                    m_Flags |= ChangingX; emit setXValue(data->oldX); m_Flags &= ~ChangingX;
                    m_Flags |= ChangingY; emit setYValue(data->oldY); m_Flags &= ~ChangingY;
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
        }
        else
        {
            struct Movement
            {
                qreal oldX, oldY, newX, newY;
            };

            struct Data : Z::QtMergeableUndoCommand::Data {
                std::unordered_map<QtVectorSceneItem*, Movement> movements;
                time_t time;
            };

            Data* data = new Data;
            data->time = time(nullptr);

            auto& movement = data->movements[this];
            movement.oldX = x();
            movement.oldY = y();
            movement.newX = newX;
            movement.newY = newY;

            QString customText = tr("Move selected items to (%1, %2).").arg(newX).arg(newY);
            undoCommand = Z::QtMergeableUndoCommand::create(
                customText,
                int(QtVectorUndo::GroupItemMove),
                data,
                [data]() {
                    for (const auto& m : data->movements) {
                        auto o = m.first;
                        o->m_Flags |= ChangingX; emit o->setXValue(m.second.newX); o->m_Flags &= ~ChangingX;
                        o->m_Flags |= ChangingY; emit o->setYValue(m.second.newY); o->m_Flags &= ~ChangingY;
                        o->setPosWithoutUndo(m.second.newX, m.second.newY);
                    }
                }, [data]() {
                    for (const auto& m : data->movements) {
                        auto o = m.first;
                        o->m_Flags |= ChangingX; emit o->setXValue(m.second.oldX); o->m_Flags &= ~ChangingX;
                        o->m_Flags |= ChangingY; emit o->setYValue(m.second.oldY); o->m_Flags &= ~ChangingY;
                        o->setPosWithoutUndo(m.second.oldX, m.second.oldY);
                    }
                }, [data](const QtMergeableUndoCommand::Data* other, QString& text, const QString& otherText) {
                    auto oth = static_cast<const Data*>(other);
                    if (abs(int(data->time - oth->time)) > UNDO_AUTOMERGE_SECONDS)
                        return false;
                    for (const auto& m : oth->movements) {
                        auto it = data->movements.find(m.first);
                        if (it == data->movements.end())
                            data->movements.insert({ m.first, m.second });
                        else {
                            it->second.newX = m.second.newX;
                            it->second.newY = m.second.newY;
                        }
                    }
                    data->time = std::max(data->time, oth->time);
                    text = otherText;
                    return true;
                }
            );
        }

        undoCommand->setAllowMerging(allowMerge);

        Q_ASSERT(scene() != nullptr);
        scene()->undoStack()->push(undoCommand);
    }

    void QtVectorSceneItem::addUndoCommandForRotation(const QString& text, qreal newRotation, bool allowMerge)
    {
        struct Data : Z::QtMergeableUndoCommand::Data {
            QtVectorSceneItem* object;
            qreal oldRotation, newRotation;
            time_t time;
        };

        Data* data = new Data;
        data->object = this;
        data->oldRotation = rotation();
        data->newRotation = newRotation;
        data->time = time(nullptr);

        auto undoCommand = Z::QtMergeableUndoCommand::create(
            text,
            int(QtVectorUndo::ItemRotate),
            data,
            [this, data]() {
                m_Flags |= ChangingRotation; emit setRotationValue(data->newRotation); m_Flags &= ~ChangingRotation;
                setRotationWithoutUndo(data->newRotation);
            }, [this, data]() {
                m_Flags |= ChangingRotation; emit setRotationValue(data->oldRotation); m_Flags &= ~ChangingRotation;
                setRotationWithoutUndo(data->oldRotation);
            }, [this, data](const QtMergeableUndoCommand::Data* other, QString& text, const QString& otherText) {
                auto oth = static_cast<const Data*>(other);
                if (data->object != oth->object || abs(int(data->time - oth->time)) > UNDO_AUTOMERGE_SECONDS)
                    return false;
                data->newRotation = oth->newRotation;
                data->time = std::max(data->time, oth->time);
                text = otherText;
                return true;
            }
        );
        undoCommand->setAllowMerging(allowMerge);

        Q_ASSERT(scene() != nullptr);
        scene()->undoStack()->push(undoCommand);
    }

    void QtVectorSceneItem::addUndoCommandForScale(const QString& text, qreal newScale, bool allowMerge)
    {
        struct Data : Z::QtMergeableUndoCommand::Data {
            QtVectorSceneItem* object;
            qreal oldScale, newScale;
            time_t time;
        };

        Data* data = new Data;
        data->object = this;
        data->oldScale = scale();
        data->newScale = newScale;
        data->time = time(nullptr);

        auto undoCommand = Z::QtMergeableUndoCommand::create(
            text,
            int(QtVectorUndo::ItemScale),
            data,
            [this, data]() {
                m_Flags |= ChangingScale; emit setScaleValue(data->newScale); m_Flags &= ~ChangingScale;
                setScaleWithoutUndo(data->newScale);
            }, [this, data]() {
                m_Flags |= ChangingScale; emit setScaleValue(data->oldScale); m_Flags &= ~ChangingScale;
                setScaleWithoutUndo(data->oldScale);
            }, [this, data](const QtMergeableUndoCommand::Data* other, QString& text, const QString& otherText) {
                auto oth = static_cast<const Data*>(other);
                if (data->object != oth->object || abs(int(data->time - oth->time)) > UNDO_AUTOMERGE_SECONDS)
                    return false;
                data->newScale = oth->newScale;
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
        addRotationProperty(propertyList, group);
        addScaleProperty(propertyList, group);
    }

    QVariant QtVectorSceneItem::itemChange(GraphicsItemChange change, const QVariant& value)
    {
        switch (change)
        {
        case ItemPositionChange:
          {
            QVariant v = QGraphicsObject::itemChange(change, value);
            if (!(m_Flags & PreventUndoPos)) {
                QPointF newPos = v.value<QPointF>();
                if (x() != newPos.x() || y() != newPos.y()) {
                    m_Flags |= ChangingX; emit setXValue(newPos.x()); m_Flags &= ~ChangingX;
                    m_Flags |= ChangingY; emit setYValue(newPos.y()); m_Flags &= ~ChangingY;
                    QString text = tr("Move %1 to (%2, %3).").arg(name()).arg(newPos.x()).arg(newPos.y());
                    addUndoCommandForMove(text, newPos.x(), newPos.y(), true);
                }
            }
            return v;
          }

        case ItemPositionHasChanged:
            emit positionChanged();
            break;

        case ItemRotationChange:
          {
            QVariant v = QGraphicsObject::itemChange(change, value);
            if (!(m_Flags & PreventUndoRotation)) {
                qreal newRotation = v.toFloat();
                if (rotation() != newRotation) {
                    m_Flags |= ChangingRotation; emit setRotationValue(newRotation); m_Flags &= ~ChangingRotation;
                    QString text = tr("Rotate %1 to %2 degrees.").arg(name()).arg(newRotation);
                    addUndoCommandForRotation(text, newRotation, true);
                }
            }
            return v;
          }

        case ItemRotationHasChanged:
            emit rotationChanged();
            break;

        case ItemScaleChange:
          {
            QVariant v = QGraphicsObject::itemChange(change, value);
            if (!(m_Flags & PreventUndoScale)) {
                qreal newScale = v.toFloat();
                if (scale() != newScale) {
                    m_Flags |= ChangingScale; emit setScaleValue(newScale); m_Flags &= ~ChangingScale;
                    QString text = tr("Scale %1 to %2.").arg(name()).arg(newScale);
                    addUndoCommandForScale(text, newScale, true);
                }
            }
            return v;
          }

        case ItemScaleHasChanged:
            emit scaleChanged();
            break;

        default:
            break;
        }

        return QGraphicsObject::itemChange(change, value);
    }

    void QtVectorSceneItem::xEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        qreal value = property->value().toFloat();
        if (value != x()) {
            QString text = tr("Change %1 to %2.").arg(property->name()).arg(value);
            addUndoCommandForMove(text, value, y(), false);
            setPosWithoutUndo(value, y());
        }
    }

    void QtVectorSceneItem::yEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        qreal value = property->value().toFloat();
        if (value != y()) {
            QString text = tr("Change %1 to %2.").arg(property->name()).arg(value);
            addUndoCommandForMove(text, x(), value, false);
            setPosWithoutUndo(x(), value);
        }
    }

    void QtVectorSceneItem::rotationEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        qreal value = property->value().toFloat();
        if (value != rotation()) {
            QString text = tr("Change %1 to %2.").arg(property->name()).arg(value);
            addUndoCommandForRotation(text, value, false);
            setRotationWithoutUndo(value);
        }
    }

    void QtVectorSceneItem::scaleEdited()
    {
        auto property = qobject_cast<QtPropertyListItem*>(sender());
        qreal value = property->value().toFloat();
        if (value != scale()) {
            QString text = tr("Change %1 to %2.").arg(property->name()).arg(value);
            addUndoCommandForScale(text, value, false);
            setScaleWithoutUndo(value);
        }
    }

    void QtVectorSceneItem::xPropertyChanged()
    {
        if (!(m_Flags & ChangingX)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            setPosWithoutUndo(property->value().toFloat(), y());
        }
    }

    void QtVectorSceneItem::yPropertyChanged()
    {
        if (!(m_Flags & ChangingY)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            setPosWithoutUndo(x(), property->value().toFloat());
        }
    }

    void QtVectorSceneItem::rotationPropertyChanged()
    {
        if (!(m_Flags & ChangingRotation)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            setRotationWithoutUndo(property->value().toFloat());
        }
    }

    void QtVectorSceneItem::scalePropertyChanged()
    {
        if (!(m_Flags & ChangingScale)) {
            auto property = qobject_cast<QtPropertyListItem*>(sender());
            setScaleWithoutUndo(property->value().toFloat());
        }
    }
}
