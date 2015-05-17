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
#include "QtUndoCommand.h"
#include <memory>

namespace Z
{
    QtUndoCommand::QtUndoCommand(const QString& text, QUndoCommand* parent)
        : QUndoCommand(text, parent)
        , m_SkipRedo(true)
    {
    }

    QtUndoCommand* QtUndoCommand::create(const QString& text, FnRef fnrepeat, FnRef fnrevert)
    {
        return create(text, nullptr, fnrepeat, fnrevert);
    }

    QtUndoCommand* QtUndoCommand::create(const QString& text, QUndoCommand* parent, FnRef fnrepeat, FnRef fnrevert)
    {
        class Command : public QtUndoCommand
        {
        public:
            Fn m_Repeat;
            Fn m_Revert;

            Command(const QString& text, QUndoCommand* command = nullptr) : QtUndoCommand(text, command) {}

            void repeat() final override { if (m_Repeat) m_Repeat(); }
            void revert() final override { if (m_Revert) m_Revert(); }
        };

        std::unique_ptr<Command> command(new Command(text, parent));
        command->m_Repeat = fnrepeat;
        command->m_Revert = fnrevert;

        return command.release();
    }

    void QtUndoCommand::redo()
    {
        if (m_SkipRedo)
            m_SkipRedo = false;
        else
            repeat();
    }

    void QtUndoCommand::undo()
    {
        Q_ASSERT(!m_SkipRedo);
        m_SkipRedo = false;
        revert();
        onCommandUndone();
    }
}
