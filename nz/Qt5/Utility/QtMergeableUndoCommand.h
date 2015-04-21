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
#include "QtUndoCommand.h"

namespace Z
{
    class QtMergeableUndoCommand : public QtUndoCommand
    {
    public:
        struct Data {
            Data() = default;
            Data(const Data&) {}
            Data(Data&&) {}
            virtual ~Data() = default;
            Data& operator=(const Data&) { return *this; }
            Data& operator=(Data&&) { return *this; }
        };

        using MergeFn = std::function<bool(const Data*, QString& text, const QString& otherText)>;
        using MergeFnRef = const MergeFn&;

        QtMergeableUndoCommand(const QString& text, int id, Data* data = nullptr, QUndoCommand* parent = nullptr);
        ~QtMergeableUndoCommand();

        static QtMergeableUndoCommand* create(const QString& text, int id, Data* data,
            FnRef fnrepeat, FnRef fnrevert, MergeFnRef fnmerge);
        static QtMergeableUndoCommand* create(const QString& text, int id, Data* data, QUndoCommand* parent,
            FnRef fnrepeat, FnRef fnrevert, MergeFnRef fnmerge);

        int id() const final override { return m_ID; }
        Data* data() const { return m_Data; }

        void setAllowMerging(bool allow) const { m_AllowMerging = allow; }
        bool mergeWith(const QUndoCommand* command) final override;

    protected:
        void onCommandUndone() override { m_AllowMerging = false; }
        virtual bool merge(const QtMergeableUndoCommand* other) = 0;

    private:
        int m_ID;
        mutable bool m_AllowMerging;
        Data* m_Data;

        using QtUndoCommand::create;
    };
}
