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
#include <QGraphicsObject>

namespace Z
{
    class QtPropertyList;
    class QtPropertyListItem;
    class QtVectorScene;

    class QtVectorSceneItem : public QGraphicsObject
    {
        Q_OBJECT

    public:
        enum {
            Type = UserType,
            ControlPointType = UserType + 1,
            ObjectType = UserType + 2,
        };

        explicit QtVectorSceneItem(QtVectorScene* scene);
        explicit QtVectorSceneItem(QtVectorSceneItem* parent);
        ~QtVectorSceneItem();

        int type() const override { return Type; }
        QtVectorScene* scene() const { return m_Scene; }

        virtual QString name() const = 0;
        QString titleName() const;

        QtPropertyList* propertyList() const;
        QtPropertyListItem* addXProperty(QtPropertyList* propertyList, int groupIndex) const;
        QtPropertyListItem* addYProperty(QtPropertyList* propertyList, int groupIndex) const;
        QtPropertyListItem* addRotationProperty(QtPropertyList* propertyList, int groupIndex) const;
        QtPropertyListItem* addScaleProperty(QtPropertyList* propertyList, int groupIndex) const;

        Q_SLOT void setPosWithoutUndo(qreal x, qreal y);
        Q_SLOT void setRotationWithoutUndo(qreal r);
        Q_SLOT void setScaleWithoutUndo(qreal s);

        void addUndoCommandForMove(const QString& text, qreal newX, qreal newY, bool allowMerge);
        void addUndoCommandForRotation(const QString& text, qreal newRotation, bool allowMerge);
        void addUndoCommandForScale(const QString& text, qreal newScale, bool allowMerge);

        void prepareGeometryChange() { QGraphicsObject::prepareGeometryChange(); }

    signals:
        void positionChanged();
        void rotationChanged();
        void scaleChanged();

        void setXValue(const QVariant& value);
        void setYValue(const QVariant& value);
        void setRotationValue(const QVariant& value);
        void setScaleValue(const QVariant& value);

    protected:
        virtual void initPropertyList(QtPropertyList* propertyList);
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    private:
        enum {
            ChangingX           = 0b0000000001,
            ChangingY           = 0b0000000010,
            ChangingRotation    = 0b0000000100,
            ChangingScale       = 0b0000001000,
        };

        QtVectorScene* m_Scene;
        mutable QtPropertyList* m_PropertyList = nullptr;
        int m_Flags = 0;

        void init();

        Q_SLOT void xEdited();
        Q_SLOT void yEdited();
        Q_SLOT void rotationEdited();
        Q_SLOT void scaleEdited();

        Q_SLOT void xPropertyChanged();
        Q_SLOT void yPropertyChanged();
        Q_SLOT void rotationPropertyChanged();
        Q_SLOT void scalePropertyChanged();

        Q_DISABLE_COPY(QtVectorSceneItem);
    };
}
