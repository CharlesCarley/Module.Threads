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
#include <Windows.h>
#include "Threads/Windows/WindowsMutex.h"
#include "Utils/Console.h"
#include "Utils/Exception.h"

namespace Rt2::Threads
{
    WindowsThread::~WindowsThread()
    {
        joinImpl();
    }

    static DWORD WINAPI ThreadCallback(const LPVOID user)
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

    void WindowsThread::startImpl()
    {
        if (_thread != NullThread)
            joinImpl();

        _thread = (ThreadHandle)CreateThread(
            nullptr,         // No inheritance
            0,               // Use the default stack size
            ThreadCallback,  // main routine
            this,            // parameter
            0,               // dwCreationFlags
            (LPDWORD)&_id    // lpThreadId
        );
    }

    void WindowsThread::waitImpl() const
    {
        if (_thread != NullThread)
            ::WaitForSingleObject((HANDLE)_thread, INFINITE);
    }

    void WindowsThread::waitImpl(const size_t milliseconds) const
    {
        if (_thread != NullThread)
            ::WaitForSingleObjectEx(
                (HANDLE)_thread,
                (DWORD)milliseconds,
                TRUE);
    }

    void WindowsThread::joinImpl()
    {
        try
        {
            if (_thread != NullThread)
            {
                waitImpl(INFINITE);
                if (CloseHandle((HANDLE)_thread) == FALSE)
                    Console::writeLine("Failed to close thread handle : ", GetLastError());
                _thread = NullThread;
                _id     = Npos;
            }
        }
        catch (Exception& ex)
        {
            Console::writeLine(ex.what());
            _thread = NullThread;
            _id     = Npos;
        }
        catch (...)
        {
            _thread = NullThread;
            _id     = Npos;
        }
    }
}  // namespace Rt2::Threads
