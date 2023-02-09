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
#include "Threads/Posix/PosixMutex.h"
#include "Threads/Posix/PosixUtils.h"
#include "Threads/ThreadUtils.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{
    PosixMutex::PosixMutex()
    {
        if (const int st = pthread_mutex_init(&_mutex, nullptr); st != NoError)
        {
            LogError("failed to initialize mutex", st);
            _isInit = false;
        }
    }

    PosixMutex::~PosixMutex()
    {
        if (_isInit)
        {
            if (_isLocked)
                unlockImpl();

            if (const int err = pthread_mutex_destroy(&_mutex); err != NoError)
                LogError("failed to finalize mutex", err);
            _isInit = false;
        }
    }

    void PosixMutex::lockImpl()
    {
        if (_isInit && !_isLocked)
        {
            if (const int err = pthread_mutex_lock(&_mutex); err != NoError)
                LogError("failed to lock mutex", err);
            else
                _isLocked = true;
        }
    }

    void PosixMutex::unlockImpl()
    {
        if (_isInit && _isLocked)
        {
            if (const int err = pthread_mutex_unlock(&_mutex); err != NoError)
                LogError("failed to unlock mutex", err);
            else
                _isLocked = false;
        }
    }

    void PosixMutex::waitImpl()
    {
        lockImpl();
    }

    void PosixMutex::waitImpl(const size_t milliseconds)
    {
        if (_isInit && !_isLocked)
        {
            struct timespec timed = {0, (long)(milliseconds * 1000)};

            if (int err = pthread_mutex_timedlock(&_mutex, &timed); err != NoError)
                LogError("failed to lock mutex", err);
            else
                _isLocked = true;
        }
    }

}  // namespace Rt2::Threads
