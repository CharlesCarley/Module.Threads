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
#include "Threads/Posix/PosixCriticalSection.h"
#include "Threads/Posix/PosixUtils.h"
#include "Threads/ThreadUtils.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{
    PosixCriticalSection::PosixCriticalSection() :
        _mutex({}),
        _isInit(false)
    {
    }

    PosixCriticalSection::~PosixCriticalSection()
    {
        if (_isInit)
        {
            int status = pthread_mutex_destroy((pthread_mutex_t*)&_mutex);
            if (status != NoError)
                Console::writeLine("pthread_mutex_destroy returned ", status);
        }
    }

    void PosixCriticalSection::lockImpl() const
    {
        int status;
        if (!_isInit)
        {
            status = pthread_mutex_init((pthread_mutex_t*)&_mutex, nullptr);
            if (status != NoError)
            {
                Console::writeLine("pthread_mutex_init returned ", status);
                return;
            }
            _isInit = true;
        }
        status = pthread_mutex_lock((pthread_mutex_t*)&_mutex);
        if (status != NoError)
            Console::writeLine("pthread_mutex_lock returned ", status);
    }

    void PosixCriticalSection::unlockImpl() const
    {
        int status = pthread_mutex_unlock((pthread_mutex_t*)&_mutex);
        if (status != NoError)
            Console::writeLine("pthread_mutex_unlock returned ", status);
    }
}  // namespace Rt2::Threads
