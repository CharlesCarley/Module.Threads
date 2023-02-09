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
#include "Threads/Posix/PosixThread.h"
#include "PosixUtils.h"
#include "Threads/Thread.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{

    PosixThread::PosixThread() = default;

    PosixThread::~PosixThread()
    {
        joinImpl();
    }

    void PosixThread::startImpl()
    {
        if (_thread != NullPThread)
            joinImpl();

        struct Runner {
            static void* hook(void* arg0)
            {
                if (Thread* thread = (Thread*)arg0)
                    thread->update();
                return nullptr;
            }
        };

        pthread_attr_init(&_attr);
        pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_JOINABLE);

        if (int st = pthread_create(&_thread, &_attr, Runner::hook, this);
            st != 0)
        {
            Con::writeError("failed to create thread: ", st);
        }
    }

    void PosixThread::joinImpl()
    {
        if (_thread != NullPThread)
        {
            if (int st = pthread_join(_thread, nullptr); st != 0)
                Con::writeError("failed to destroy thread: ", st);
            if (int st = pthread_attr_destroy(&_attr); st != 0)
                Con::writeError("failed to destroy thread: ", st);

            _attr   = {};
            _thread = NullPThread;
        }
    }


    int PosixThread::update()
    {
        joinImpl();
        return 0;
    }
}  // namespace Rt2::Threads
