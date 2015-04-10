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
#include "QtPropertyList.h"
#include "QtPropertyListItem.h"
#include "QtPropertyDataType.h"
#include <QStandardItem>
#include <QTreeView>
#include <vector>
#include <utility>
#include <cassert>

namespace Z
{
    const std::vector<std::pair<QColor, QColor>> g_Colors = {
        { QColor("#fffddf"), QColor("#fffbc2") },       // yellow
        { QColor("#e4fbde"), QColor("#cdf8c0") },       // green
        { QColor("#eafefe"), QColor("#d1fcfe") },       // cyan
        { QColor("#f0e2ff"), QColor("#e2c7fe") },       // magenta
        { QColor("#fae2f7"), QColor("#f6c7ef") },       // pink
        { QColor("#fef2df"), QColor("#fce5c2") },       // orange
        { QColor("#fae0df"), QColor("#f7c4c1") },       // red
    };

    QtPropertyList::QtPropertyList(QObject* parent)
        : QStandardItemModel(parent)
        , m_View(nullptr)
        , m_NextColor(0)
    {
        setHorizontalHeaderLabels({ tr("Name"), tr("Value") });
    }

    QtPropertyList::~QtPropertyList()
    {
    }

    int QtPropertyList::addGroup(const QString& groupName, bool sorted)
    {
        QStandardItem* item = nullptr;
        int index = -1;

        if (!sorted)
        {
            index = m_GroupNames.count();

            Group group;
            group.item = item = new QStandardItem(groupName);
            group.color = (m_NextColor++ % g_Colors.size());

            m_GroupNames.insert(index, groupName);
            m_Groups.insert(index, group);
            invisibleRootItem()->insertRow(index, group.item);
        }
        else
        {
            auto it = qLowerBound(m_GroupNames.begin(), m_GroupNames.end(), groupName);
            index = it - m_GroupNames.begin();
            if (it == m_GroupNames.end() || *it != groupName)
            {
                Group group;
                group.item = item = new QStandardItem(groupName);
                group.color = (m_NextColor++ % g_Colors.size());

                m_GroupNames.insert(it, groupName);
                m_Groups.insert(index, group);
                invisibleRootItem()->insertRow(index, group.item);
            }
        }

        if (item)
        {
            item->setSelectable(false);
            item->setEditable(false);
            item->setBackground(QColor("#bfbfbf"));
            item->setForeground(Qt::white);

            QFont font;
            font.setBold(true);
            item->setFont(font);
        }

        return index;
    }

    QtPropertyListItem* QtPropertyList::addProperty(int groupIndex, const QString& name,
        const QtPropertyDataType* type, bool sorted)
    {
        Group& group = m_Groups[groupIndex];
        QtPropertyListItem* property = new QtPropertyListItem(this);

        assert(type != nullptr);
        property->m_DataType = type;

        property->m_NameItem = new QStandardItem(name);
        property->m_NameItem->setEditable(false);
        property->m_NameItem->setForeground(Qt::black);

        property->m_ValueItem = new QStandardItem;
        property->m_ValueItem->setEditable(true);
        property->m_ValueItem->setForeground(Qt::black);

        connect(property, SIGNAL(valueChanged()), SLOT(onPropertyValueChanged()));

        m_Buddies.insert(std::make_pair(property->m_NameItem, property->m_ValueItem));
        m_PropertyItems.insert(std::make_pair(property->m_ValueItem, property));

        int index;
        if (!sorted)
            index = group.propertyNames.size();
        else {
            auto it = qUpperBound(group.propertyNames.begin(), group.propertyNames.end(), name);
            index = it - group.propertyNames.begin();
        }

        group.propertyNames.insert(index, name);
        group.properties.insert(index, property);
        group.item->insertRow(index, { property->m_NameItem, property->m_ValueItem });

        updateColors(group.item, group.color);

        return property;
    }

    QModelIndex QtPropertyList::buddy(const QModelIndex& index) const
    {
        QStandardItem* item = itemFromIndex(index);

        auto it = m_Buddies.find(item);
        if (it != m_Buddies.end())
            return it->second->index();

        return index;
    }

    QVariant QtPropertyList::data(const QModelIndex& index, int role) const
    {
        bool indexValid = index.isValid();
        bool indexParentValid = indexValid && index.parent().isValid();

        if (m_View && indexValid && !indexParentValid)
            m_View->setFirstColumnSpanned(index.row(), index.parent(), true);

        if (role == Qt::DisplayRole && indexParentValid) {
            QtPropertyListItem* item = itemForIndex(index);
            if (item)
                return item->m_DataType->formatForDisplay(item->m_Value);
        }

        return QStandardItemModel::data(index, role);
    }

    QtPropertyListItem* QtPropertyList::itemForIndex(const QModelIndex& index) const
    {
        QStandardItem* item = itemFromIndex(index);

        auto it = m_PropertyItems.find(item);
        if (it != m_PropertyItems.end())
            return it->second;

        return nullptr;
    }

    void QtPropertyList::onPropertyValueChanged()
    {
        QtPropertyListItem* item = qobject_cast<QtPropertyListItem*>(sender());
        if (item) {
            QModelIndex index = item->m_ValueItem->index();
            emit dataChanged(index, index);
        }
    }

    void QtPropertyList::updateColors(QStandardItem* item, size_t color)
    {
        int numRows = item->rowCount();
        int numCols = item->columnCount();

        const auto& it = g_Colors[color];
        for (int row = 0; row < numRows; row++) {
            for (int column = 0; column < numCols; column++) {
                QStandardItem* child = item->child(row, column);
                child->setBackground(row & 1 ? it.second : it.first);
            }
        }
    }
}
