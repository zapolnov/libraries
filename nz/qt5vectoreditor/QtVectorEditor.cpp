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
#include "QtVectorEditor.h"
#include <QGLWidget>

namespace Z
{
    QtVectorEditor::QtVectorEditor(QWidget* parent)
        : QGraphicsView(parent)
        , m_CurrentMode(ClickMode)
    {
        setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
        setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    }

    QtVectorEditor::~QtVectorEditor()
    {
    }

    void QtVectorEditor::setScene(QtVectorScene* newScene)
    {
        auto oldScene = scene();
        if (oldScene != newScene) {
            if (oldScene) {
                disconnect(oldScene, SIGNAL(selectionChanged()), this, SLOT(onSceneSelectionChanged()));
            }

            QGraphicsView::setScene(newScene);

            if (newScene) {
                connect(newScene, SIGNAL(selectionChanged()), SLOT(onSceneSelectionChanged()));
            }

            setCurrentMode(ClickMode);
        }
    }

    void QtVectorEditor::setCurrentMode(Mode mode)
    {
        if (m_CurrentMode != mode) {
            m_CurrentMode = mode;
            setDragMode(mode == SelectMode ? RubberBandDrag : NoDrag);
            emit currentModeChanged(m_CurrentMode);
        }
    }

    void QtVectorEditor::onSceneSelectionChanged()
    {
    }
}
