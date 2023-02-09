/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#pragma once

#include "Threads/ThreadUtils.h"

#if RT_PLATFORM == RT_PLATFORM_WINDOWS
#include "Threads/Windows/WindowsMutex.h"
#else
#include "Threads/Posix/PosixMutex.h"
#endif

namespace Rt2::Threads
{
#if RT_PLATFORM == RT_PLATFORM_WINDOWS
    using PlatformMutex = WindowsMutex;
#else
    using PlatformMutex = PosixMutex;
#endif

    class Mutex final : public PlatformMutex
    {
    public:
        Mutex() = default;

        void lock()
        {
            lockImpl();
        }

        void unlock()
        {
            unlockImpl();
        }

        void wait()
        {
            waitImpl();
        }

        void wait(const size_t ms)
        {
            waitImpl(ms);
        }
    };

    class ScopeLock
    {
    private:
        Mutex* _mutex;
    public:
        explicit ScopeLock(Mutex* mtx) :
            _mutex(mtx)
        {
            if (_mutex)
                _mutex->lock();
        }

        ~ScopeLock()
        {
            if (_mutex)
                _mutex->unlock();
        }
    };

#define ScopeLockMtx(mtx) \
    Rt2::Threads::ScopeLock ____scope__lock__(&mtx)

}  // namespace Rt2::Threads
