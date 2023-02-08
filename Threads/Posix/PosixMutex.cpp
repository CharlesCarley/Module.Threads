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
        if (int status = pthread_mutex_init(&_mutex, nullptr);
            status != NoError)
        {
            Console::writeLine("failed to initialize mutex ", status);
            _isInit = false;
        }
    }

    PosixMutex::~PosixMutex()
    {
        if (_isInit)
        {
            if (int status = pthread_mutex_destroy(&_mutex);
                status != NoError)
                Console::writeLine("failed to finalize mutex: ", status);
            _isInit = false;
        }
    }

    void PosixMutex::lockImpl()
    {
        notifyImpl();
    }

    void PosixMutex::unlockImpl()
    {
        if (_isInit)
        {
            if (int status = pthread_mutex_unlock(&_mutex);
                status != NoError)
                Console::writeLine("failed to unlock mutex: ", status);
        }
    }

    void PosixMutex::waitImpl() const
    {
        if (_isInit)
        {
            if (int status = pthread_mutex_lock(
                    const_cast<pthread_mutex_t*>(&_mutex));
                status != NoError)
                Console::writeLine("failed to lock mutex: ", status);
        }
    }

    void PosixMutex::waitImpl(const size_t milliseconds) const
    {
        if (_isInit)
        {
            struct timespec timed = {0, (long)(milliseconds * 1000)};
            if (int status = pthread_mutex_timedlock(
                    const_cast<pthread_mutex_t*>(&_mutex), &timed);
                status != NoError)
                Console::writeLine("failed to lock mutex: ", status);
        }
    }

    void PosixMutex::notifyImpl() const
    {
        if (_isInit)
        {
            if (int status = pthread_mutex_lock(const_cast<pthread_mutex_t*>(&_mutex));
                status != NoError)
                Console::writeLine("failed to lock mutex: ", status);
        }
    }
}  // namespace Rt2::Threads
