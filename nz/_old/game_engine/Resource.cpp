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
#include "Resource.h"
#include "ResourceManager.h"
#include "utility/debug.h"

namespace Z
{
    Resource::Resource(ResourceManager* manager)
        : m_Manager(manager)
        , m_Flags(0)
    {
        Z_ASSERT(m_Manager);
        m_Manager->registerResource(this);
    }

    Resource::~Resource()
    {
        Z_ASSERT(!isLoadingOrUnloading());
        m_Manager->unregisterResource(this);
    }

    void Resource::setNeededResident(bool flag)
    {
        int desired, expected = m_Flags.load();
        do {
            desired = expected | LoadingOrUnloading;
            if (flag) {
                if (expected & NeededResident)
                    return;
                desired |= NeededResident;
            } else {
                if (!(expected & NeededResident))
                    return;
                desired &= ~NeededResident;
            }
            desired &= ~DidFailToLoad;
        } while (!m_Flags.compare_exchange_weak(expected, desired));

        bool wasLoading = (expected & LoadingOrUnloading);
        if (!wasLoading) {
            if (flag)
                initiateLoad();
            else
                initiateUnload();
        }
    }

    void Resource::initiateLoad()
    {
        Z_ASSERT(isLoadingOrUnloading());

        auto selfp = shared_from_this();
        load([selfp](bool success) {
            if (success)
                selfp->m_Manager->handleResourceLoad(selfp);
            int desired, expected = selfp->m_Flags.load();
            do {
                Z_ASSERT(expected & LoadingOrUnloading);
                if (!(expected & NeededResident)) {
                    selfp->initiateUnload();
                    return;
                }
                Z_ASSERT(expected & NeededResident);
                desired = expected & ~LoadingOrUnloading;
                if (success)
                    desired &= ~DidFailToLoad;
                else
                    desired |= DidFailToLoad;
            } while (!selfp->m_Flags.compare_exchange_weak(expected, desired));
        });
    }

    void Resource::initiateUnload()
    {
        Z_ASSERT(isLoadingOrUnloading());

        auto selfp = shared_from_this();
        unload([selfp]() {
            selfp->m_Manager->handleResourceUnload(selfp);
            int desired, expected = selfp->m_Flags.load();
            do {
                Z_ASSERT(expected & LoadingOrUnloading);
                if (expected & NeededResident) {
                    selfp->initiateLoad();
                    return;
                }
                Z_ASSERT(!(expected & NeededResident));
                desired = expected & ~(LoadingOrUnloading | DidFailToLoad);
            } while (!selfp->m_Flags.compare_exchange_weak(expected, desired));
        });
    }
}
