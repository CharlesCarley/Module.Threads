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
#include <pthread.h>
#include <unistd.h>
#include "Threads/Thread.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{
    class PosixStaticUtils
    {
    public:
        static void* threadExec(void* arg0)
        {
            Thread* thread = (Thread*)arg0;
            if (thread)
                thread->update();
            return nullptr;
        }
    };

    PosixThread::PosixThread() :
        _id(Npos),
        _thread(NullThread)
    {
    }

    PosixThread::~PosixThread()
    {
        if (_thread != NullThread)
        {
            int status;
            status = pthread_cancel(_thread);
            if (status != 0)
                Con::writeError("pthread_cancel returned: ", status);
        }
    }

    void PosixThread::startImpl()
    {
        if (_thread != NullThread)
            joinImpl();
        int status;

        if ((status = pthread_create((pthread_t*)&_thread,
                                     nullptr,
                                     PosixStaticUtils::threadExec,
                                     this)) != 0)
        {
            Con::writeError("pthread_cancel pthread_create: ", status);
        }
    }

    void PosixThread::waitImpl(size_t milliseconds) const
    {
        usleep(milliseconds * 1000);
    }

    void PosixThread::joinImpl()
    {
        if (_thread != NullThread)
            pthread_join(_thread, nullptr);
        _thread = NullThread;
    }
}  // namespace Rt2::Threads
