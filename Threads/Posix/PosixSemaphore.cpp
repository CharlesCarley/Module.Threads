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
#include "Threads/Posix/PosixSemaphore.h"
#include "Threads/Posix/PosixUtils.h"
#include "Utils/Console.h"

#define SHARED_SINGLE_PROCESS 0

// defined as 1 so that decrementing to 0 will block 
#define BINARY_SEM_COUNT 1

namespace Rt2::Threads
{
    PosixSemaphore::PosixSemaphore()
    {
        if (const int st = sem_init(
                &_sem,
                SHARED_SINGLE_PROCESS,
                BINARY_SEM_COUNT);
            st == -1)
        {
            switch (errno)
            {
            case EINVAL:  // hard coded
                break;
            case ENOSPC:
                Console::writeLine("maximum number of semaphores has been reached");
                break;
            case ENOSYS:
                Console::writeLine("sem_init is not supported");
                break;
            case EPERM:
                Console::writeLine("the process lacks the privileges to use sem_init");
                break;
            default:
                Console::writeLine("sem_init - unknown error");
                break;
            }
            _sem  = NullSemaphore;
            _init = false;
        }
    }

    PosixSemaphore::~PosixSemaphore()
    {
        if (_init)
        {
            if (const int st = sem_destroy(&_sem);
                st == -1)
            {
                switch (errno)
                {
                case EINVAL:
                    Console::writeLine("Invalid handle");
                    break;
                case ENOSYS:
                    Console::writeLine("sem_destroy is not supported");
                    break;
                case EBUSY:
                    Console::writeLine("semaphore is in use");
                    break;
                default:
                    Console::writeLine("sem_destroy - unknown error");
                    break;
                }

                _sem = NullSemaphore;
                _locked = false;
            }
        }
    }

    void PosixSemaphore::waitImpl()
    {
        if (_init)
        {
            _locked = true;
            sem_wait(&_sem);
        }
    }

    void PosixSemaphore::signalImpl()
    {
        if (_init)
        {
            if (_locked)
                sem_post(&_sem);
        }
    }
}  // namespace Rt2::Threads
