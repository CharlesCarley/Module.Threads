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
#include "Threads/Windows/stubs/unistd.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{

    PosixThread::PosixThread() = default;

    PosixThread::~PosixThread()
    {
        if (_thread != NullPThread)
        {
            if (int status = pthread_cancel(_thread);
                status != 0)
                Con::writeError("failed to destroy thread: ", status);
        }
    }

    void PosixThread::waitImpl() const
    {
    }

    void PosixThread::startImpl()
    {
        if (_thread != NullPThread)
            joinImpl();

        struct Runner
        {
            static void* hook(void* arg0)
            {
                if (Thread* thread = (Thread*)arg0)
                    thread->update();
                return nullptr;
            }
        };

        if (int status = pthread_create(
                &_thread,
                nullptr,
                Runner::hook,
                this);
            status != 0)
        {
            Con::writeError("failed to create thread: ", status);
        }
    }

    void PosixThread::waitImpl(size_t milliseconds) const
    {
        usleep(milliseconds * 1000);
    }

    void PosixThread::joinImpl()
    {
        if (_thread != NullPThread)
            pthread_join(_thread, nullptr);
        _thread = NullPThread;
    }


    int PosixThread::update()
    {
        joinImpl();
        return 0;
    }
}  // namespace Rt2::Threads
