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
#include "QtVectorScene.h"
#include "QtVectorControlPoint.h"
#include "QtVectorEditor.h"
#include "QtVectorObject.h"
#include "QtVectorUndo.h"
#include "QtMergeableUndoCommand.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

namespace Z
{
    QtVectorScene::QtVectorScene(QtVectorEditor* parent)
        : QGraphicsScene(parent)
    {
        Q_ASSERT(parent != nullptr);

        setBackgroundBrush(Qt::black);
        connect(this, SIGNAL(selectionChanged()), SLOT(onSelectionChanged()));

        m_UndoStack = new QUndoStack(this);

        m_Canvas = addRect(QRectF(0, 0, 0, 0), QPen(Qt::black), Qt::black);
        m_Canvas->setFlags(0);
        m_Canvas->setVisible(true);

        m_DeletedItems = addRect(QRectF(0, 0, 0, 0), QPen(Qt::black), Qt::black);
        m_DeletedItems->setFlags(0);
        m_DeletedItems->setEnabled(false);
        m_DeletedItems->setVisible(false);
    }

    QtVectorScene::~QtVectorScene()
    {
    }

    QtVectorEditor* QtVectorScene::editor() const
    {
        return qobject_cast<QtVectorEditor*>(parent());
    }

    void QtVectorScene::setCurrentControlPoints(QGraphicsItem* cp)
    {
        if (m_CurrentControlPoints == cp) {
            if (m_CurrentControlPoints)
                m_CurrentControlPoints->setVisible(editor()->currentMode() == QtVectorEditor::ClickMode);
        } else {
            if (m_CurrentControlPoints)
                m_CurrentControlPoints->setVisible(false);

            m_CurrentControlPoints = cp;

            if (m_CurrentControlPoints)
                m_CurrentControlPoints->setVisible(editor()->currentMode() == QtVectorEditor::ClickMode);
        };
    }

    void QtVectorScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
    {
        /*
        QtVectorEditor* editor = qobject_cast<QtVectorEditor*>(parent());

        switch (editor->currentMode())
        {
        case QtVectorEditor::ClickMode:
          {
            QList<QGraphicsItem*> itemsUnderMouse = items(mouseEvent->scenePos());
            for (QGraphicsItem* item : itemsUnderMouse) {
                QtVectorControlPoint* controlPoint = qgraphicsitem_cast<QtVectorControlPoint*>(item);
                if (controlPoint) {
                    controlPoint->setSelected(true);
                    if (sendEvent(controlPoint, mouseEvent))
                        return;
                }
            }
            break;
          }

        case QtVectorEditor::SelectMode:
            break;
        }
        */

        QGraphicsScene::mousePressEvent(mouseEvent);
    }

    void QtVectorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }

    void QtVectorScene::disallowMergeForLastUndoCommand()
    {
        int count = m_UndoStack->count();
        if (count > 0) {
            const QUndoCommand* command = m_UndoStack->command(count - 1);
            if (isMergeableUndoCommand(command))
                static_cast<const QtMergeableUndoCommand*>(command)->setAllowMerging(false);
        }
    }

    QtVectorSceneItem* QtVectorScene::currentSingleSelection() const
    {
        QtVectorSceneItem* selectedItem = nullptr;

        QList<QGraphicsItem*> items = selectedItems();
        for (QGraphicsItem* item : items) {
            QtVectorObject* object = qgraphicsitem_cast<QtVectorObject*>(item);
            if (!object) {
                QtVectorControlPoint* point = qgraphicsitem_cast<QtVectorControlPoint*>(item);
                if (point) {
                    if (point->ownerObject())
                        object = point->ownerObject();
                    else {
                        if (!selectedItem)
                            selectedItem = point;
                        else if (selectedItem != point)
                            return nullptr;
                    }
                }
            }

            if (object) {
                if (!selectedItem)
                    selectedItem = object;
                else if (selectedItem != object)
                    return nullptr;
            }
        }

        return selectedItem;
    }

    void QtVectorScene::onSelectionChanged()
    {
        disallowMergeForLastUndoCommand();

        auto selection = currentSingleSelection();
        auto object = qobject_cast<QtVectorObject*>(selection);
        setCurrentControlPoints(object ? object->m_ControlPointsContainer : nullptr);

        /*
        if (editor()->currentMode() == QtVectorEditor::SelectMode) {
            QList<QGraphicsItem*> items = selectedItems();
            for (auto item : items) {
                QtVectorObject* object = qgraphicsitem_cast<QtVectorObject*>(item);
                if (object)
                    continue;

                QtVectorControlPoint* point = qgraphicsitem_cast<QtVectorControlPoint*>(item);
                if (!point || !point->ownerObject())
                    continue;

                auto ownerObject = point->ownerObject();
                if (!ownerObject->isSelected())
                    ownerObject->setSelected(true);
                item->setSelected(false);
            }
        }
        */
    }
}
