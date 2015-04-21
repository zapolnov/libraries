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
#include "QtVectorUndo.h"
#include "QtVectorScene.h"
#include "QtPropertyListItem.h"
#include "QtMergeableUndoCommand.h"
#include <QVariant>
#include <QUndoCommand>
#include <ctime>

namespace Z
{
    void addUndoCommandForProperty(QtVectorScene* scene, QtPropertyListItem* property,
        const QVariant& oldValue, const QVariant& newValue, bool allowMerge)
    {
        struct Data : Z::QtMergeableUndoCommand::Data {
            QtPropertyListItem* property;
            QVariant oldValue, newValue;
            time_t time;
        };

        Data* data = new Data;
        data->property = property;
        data->oldValue = oldValue;
        data->newValue = newValue;
        data->time = time(nullptr);

        auto undoCommand = Z::QtMergeableUndoCommand::create(
            QObject::tr("Change %1 to %2.").arg(property->name()).arg(newValue.toString()),
            int(QtVectorUndo::PropertyChange),
            data,
            [data]() {
                data->property->setValue(data->newValue);
            }, [data]() {
                data->property->setValue(data->oldValue);
            }, [data](const QtMergeableUndoCommand::Data* other, QString& text, const QString& otherText) {
                auto oth = static_cast<const Data*>(other);
                if (data->property != oth->property || abs(int(data->time - oth->time)) > UNDO_AUTOMERGE_SECONDS)
                    return false;
                data->newValue = oth->newValue;
                data->time = std::max(data->time, oth->time);
                text = otherText;
                return true;
            }
        );

        undoCommand->setAllowMerging(allowMerge);
        scene->undoStack()->push(undoCommand);
    }

    bool isMergeableUndoCommand(const QUndoCommand* command)
    {
        int id = command->id();
        if (id < 0)
            return false;

        switch (static_cast<QtVectorUndo>(id))
        {
        case QtVectorUndo::ItemMove:
        case QtVectorUndo::GroupItemMove:
        case QtVectorUndo::ItemRotate:
        case QtVectorUndo::ItemScale:
            return true;

        case QtVectorUndo::PropertyChange:
            return false;
        }

        Q_ASSERT(false);
        return false;
    }
}
