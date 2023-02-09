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
#include "Threads/Windows/WindowsThread.h"
#include "Threads/Windows/WindowsUtils.h"
#include "Utils/Console.h"
#include "Utils/Exception.h"

namespace Rt2::Threads
{
    WindowsThread::~WindowsThread()
    {
        joinImpl();
    }

    void WindowsThread::startImpl()
    {
        struct Runner
        {
            static DWORD WINAPI invoke(const LPVOID user)
            {
                try
                {
                    if (WindowsThread* thread = static_cast<WindowsThread*>(user))
                        return thread->update();
                }
                catch (Exception& e)
                {
                    Console::writeLine(e.what());
                }
                catch (...)
                {
                }
                return 0;
            }
        };

        if (_thread != NullHandle)
            joinImpl();


        if (HANDLE handle = CreateThread(
                nullptr,           // No inheritance
                0,                 // Use the default stack size
                Runner::invoke,    // main routine
                this,              // parameter
                CREATE_SUSPENDED,  // dwCreationFlags
                (LPDWORD)&_id      // lpThreadId
            );
            handle == nullptr)
            LogError("failed to create thread", FALSE);
        else
        {
            _thread = (ThreadHandle)handle;

            if (ResumeThread(handle) == (DWORD)-1)
                LogError("failed to resume thread", (DWORD)-1);
        }
    }

    void WindowsThread::joinImpl()
    {
        if (const HANDLE handle = toHandle(_thread))
        {
            if (WaitForSingleObjectEx(handle, INFINITE, TRUE) == WAIT_FAILED)
                LogError("failed to wait on thread", WAIT_FAILED);

            if (CloseHandle(handle) == FALSE)
                LogError("failed to close handle", FALSE);

            _thread = NullHandle;
            _id     = Npos;
        }
    }
}  // namespace Rt2::Threads
