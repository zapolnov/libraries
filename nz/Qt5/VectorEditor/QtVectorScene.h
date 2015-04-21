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
#include <functional>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QUndoStack>

namespace Z
{
    class QtVectorEditor;
    class QtVectorSceneItem;

    class QtVectorScene : public QGraphicsScene
    {
        Q_OBJECT

    public:
        explicit QtVectorScene(QtVectorEditor* parent);
        ~QtVectorScene();

        QtVectorEditor* editor() const;

        QGraphicsItem* canvas() const { return m_Canvas; }
        QUndoStack* undoStack() const { return m_UndoStack; }

        QGraphicsItem* currentControlPoints() const { return m_CurrentControlPoints; }
        void setCurrentControlPoints(QGraphicsItem* cp);

        Q_SLOT void disallowMergeForLastUndoCommand();

        QtVectorSceneItem* currentSingleSelection() const;

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

    private:
        QGraphicsRectItem* m_Canvas;
        QGraphicsRectItem* m_DeletedItems;
        QGraphicsItem* m_CurrentControlPoints = nullptr;
        QUndoStack* m_UndoStack;

        Q_SLOT void onSelectionChanged();

        friend class QtVectorObject;
    };
}
