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
#include "QtMergeableUndoCommand.h"
#include <memory>

namespace Z
{
    QtMergeableUndoCommand::QtMergeableUndoCommand(const QString& text, int id, Data* data, QUndoCommand* parent)
        : QtUndoCommand(text, parent)
        , m_ID(id)
        , m_AllowMerging(id >= 0)
        , m_Data(data)
    {
    }

    QtMergeableUndoCommand::~QtMergeableUndoCommand()
    {
        delete m_Data;
    }

    QtMergeableUndoCommand* QtMergeableUndoCommand::create(const QString& text, int id, Data* data,
        FnRef fnrepeat, FnRef fnrevert, MergeFnRef fnmerge)
    {
        return create(text, id, data, nullptr, fnrepeat, fnrevert, fnmerge);
    }

    QtMergeableUndoCommand* QtMergeableUndoCommand::create(const QString& text, int id, Data* data,
        QUndoCommand* parent, FnRef fnrepeat, FnRef fnrevert, MergeFnRef fnmerge)
    {
        class Command : public QtMergeableUndoCommand
        {
        public:
            Fn m_Repeat;
            Fn m_Revert;
            MergeFn m_Merge;

            Command(const QString& text, int id, Data* data = nullptr, QUndoCommand* parent = nullptr)
                : QtMergeableUndoCommand(text, id, data, parent) {}

            void repeat() final override { if (m_Repeat) m_Repeat(); }
            void revert() final override { if (m_Revert) m_Revert(); }

            bool merge(const QtMergeableUndoCommand* other) final override {
                QString thisText = text();
                if (m_Merge && m_Merge(other->data(), thisText, other->text())) {
                    setText(thisText);
                    return true;
                }
                return false;
            }
        };

        std::unique_ptr<Command> command(new Command(text, id, data, parent));
        command->m_Repeat = fnrepeat;
        command->m_Revert = fnrevert;
        command->m_Merge = fnmerge;

        return command.release();
    }

    bool QtMergeableUndoCommand::mergeWith(const QUndoCommand* command)
    {
        if (!m_AllowMerging || !command || command->id() != m_ID || m_ID < 0)
            return false;

        auto other = static_cast<const QtMergeableUndoCommand*>(command);
        if (!other->m_AllowMerging)
            return false;

        return merge(other);
    }
}
