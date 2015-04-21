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
#include <functional>
#include <atomic>
#include <memory>

namespace Z
{
    class ResourceManager;

    class Resource : public std::enable_shared_from_this<Resource>
    {
    public:
        explicit Resource(ResourceManager* manager);
        virtual ~Resource();

        ResourceManager* manager() const { return m_Manager; }

        bool isNeededResident() const { return m_Flags.load() & NeededResident; }
        bool isResident() const { return m_Flags.load() & Resident; }
        bool isResidentOrFailedToLoad() const { return m_Flags.load() & (Resident | DidFailToLoad); }
        bool isLoadingOrUnloading() const { return m_Flags.load() & LoadingOrUnloading; }
        bool didFailToLoad() const { return m_Flags.load() & DidFailToLoad; }

        void setNeededResident(bool flag);

    protected:
        virtual void load(const std::function<void(bool)>& callback) = 0;
        virtual void unload(const std::function<void()>& callback) = 0;

    private:
        enum Flag {
            Resident            = 0b0000001,
            NeededResident      = 0b0000010,
            LoadingOrUnloading  = 0b0000100,
            DidFailToLoad        = 0b0001000,
        };

        ResourceManager* m_Manager;
        std::atomic<int> m_Flags;

        void initiateLoad();
        void initiateUnload();
    };

    using ResourcePtr = std::shared_ptr<Resource>;
}
