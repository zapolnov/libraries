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
#include <QList>
#include <QStandardItemModel>
#include <unordered_map>

class QTreeView;

namespace Z
{
    class QtPropertyListItem;
    class QtPropertyDataType;

    class QtPropertyList : public QStandardItemModel
    {
        Q_OBJECT

    public:
        QtPropertyList(QObject* parent = nullptr);
        ~QtPropertyList();

        QTreeView* view() const { return m_View; }
        void setView(QTreeView* view) { m_View = view; }

        int addGroup(const QString& groupName, bool sorted);
        QtPropertyListItem* addProperty(int groupIndex, const QString& name,
            const QtPropertyDataType* type, bool sorted);

        QModelIndex buddy(const QModelIndex& index) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QtPropertyListItem* itemForIndex(const QModelIndex& index) const;

    private:
        struct Group
        {
            QStandardItem* item = nullptr;
            QList<QString> propertyNames;
            QList<QtPropertyListItem*> properties;
            size_t color;
        };

        QTreeView* m_View;
        QList<QString> m_GroupNames;
        QList<Group> m_Groups;
        std::unordered_map<QStandardItem*, QtPropertyListItem*> m_PropertyItems;
        std::unordered_map<QStandardItem*, QStandardItem*> m_Buddies;
        size_t m_NextColor;

        Q_SLOT void onPropertyValueChanged();
        void updateColors(QStandardItem* item, size_t color);

        Q_DISABLE_COPY(QtPropertyList)
    };
}
