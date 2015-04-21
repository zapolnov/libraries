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
#include "QtVectorScene.h"
#include <QGraphicsView>

namespace Z
{
    class QtPropertyList;
    class QtVectorSceneItem;

    class QtVectorEditor : public QGraphicsView
    {
        Q_OBJECT

    public:
        enum Mode
        {
            ClickMode,
            SelectMode,
        };

        explicit QtVectorEditor(QWidget* parent = nullptr);
        ~QtVectorEditor();

        QtVectorScene* scene() const { return qobject_cast<QtVectorScene*>(QGraphicsView::scene()); }
        void setScene(QtVectorScene* scene);

        Mode currentMode() const { return m_CurrentMode; }
        void setCurrentMode(Mode mode);

        QtPropertyList* currentPropertyList() const { return m_CurrentPropertyList; }

    signals:
        void currentModeChanged(Mode mode);
        void currentPropertyListChanged(Z::QtPropertyList* propertyList);

    private:
        Mode m_CurrentMode;
        QtPropertyList* m_CurrentPropertyList = nullptr;
        QGraphicsScene* m_DummyScene;

        Q_SLOT void onSceneSelectionChanged();
    };
}
