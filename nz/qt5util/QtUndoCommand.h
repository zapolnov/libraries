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
#include <QUndoCommand>
#include <functional>

namespace Z
{
    class QtUndoCommand : public QUndoCommand
    {
    public:
        using Fn = std::function<void()>;
        using FnRef = const Fn&;

        explicit QtUndoCommand(const QString& text, QUndoCommand* parent = nullptr);

        static QtUndoCommand* create(const QString& text, FnRef fnrepeat, FnRef fnrevert);
        static QtUndoCommand* create(const QString& text, QUndoCommand* parent, FnRef fnrepeat, FnRef fnrevert);

    protected:
        virtual void onCommandUndone() {}
        virtual void repeat() = 0;
        virtual void revert() = 0;

    private:
        bool m_SkipRedo;

        void undo() final override;
        void redo() final override;

        Q_DISABLE_COPY(QtUndoCommand)
    };
}
