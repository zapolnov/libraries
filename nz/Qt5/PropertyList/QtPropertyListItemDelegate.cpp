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
#include "QtPropertyListItemDelegate.h"
#include "QtPropertyList.h"
#include "QtPropertyListItem.h"
#include "QtPropertyDataType.h"
#include <QAbstractItemView>

namespace Z
{
    QtPropertyListItemDelegate::QtPropertyListItemDelegate(QAbstractItemView* view)
        : QStyledItemDelegate(view)
        , m_View(view)
    {
        if (m_View)
            m_View->setItemDelegate(this);
    }

    QtPropertyListItemDelegate::~QtPropertyListItemDelegate()
    {
    }

    QSize QtPropertyListItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(20);
        return size;
    }

    void QtPropertyListItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
    {
        QStyledItemDelegate::paint(painter, option, index);

        auto property = propertyForIndex(index);
        if (property)
            property->dataType()->paint(painter, property->value(), m_View->style(), option);
    }

    QWidget* QtPropertyListItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const
    {
        auto property = propertyForIndex(index);
        if (property)
            return property->dataType()->createEditor(parent);
        else
            return QStyledItemDelegate::createEditor(parent, option, index);
    }

    void QtPropertyListItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
    {
        auto property = propertyForIndex(index);
        if (property)
            property->dataType()->setEditorData(editor, property->value());
        else
            QStyledItemDelegate::setEditorData(editor, index);
    }

    void QtPropertyListItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* inModel,
        const QModelIndex& index) const
    {
        auto property = propertyForIndex(index);
        if (!property)
            QStyledItemDelegate::setModelData(editor, inModel, index);
        else {
            property->m_Value = property->dataType()->getEditorData(editor);
            emit property->valueEdited();
        }
    }

    QtPropertyListItem* QtPropertyListItemDelegate::propertyForIndex(const QModelIndex& index) const
    {
        if (!m_View)
            return nullptr;

        QtPropertyList* list = qobject_cast<QtPropertyList*>(m_View->model());
        if (!list)
            return nullptr;

        return list->itemForIndex(index);
    }
}
