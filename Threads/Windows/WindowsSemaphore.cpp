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
#include "Threads/Windows/WindowsSemaphore.h"
#include <Windows.h>
#include "Utils/Console.h"

namespace Rt2::Threads
{
    WindowsSemaphore::WindowsSemaphore() :
        _handle(0)
    {
        initialize();
    }

    WindowsSemaphore::~WindowsSemaphore()
    {
        finalize();
    }

    void WindowsSemaphore::initialize()
    {
        _handle = (size_t)CreateSemaphore(
            nullptr,  // Cannot be inherited by child processes.
            0,        // lInitialCount
            1,        // lMaximumCount
            nullptr   // lpName -  No name
        );
        if (!_handle)
            Console::writeLine("Failed to create semaphore : ",
                               GetLastError());
    }

    void WindowsSemaphore::finalize() const
    {
        try
        {
            if (!_handle)
                return;

            if (CloseHandle((HANDLE)_handle) == FALSE)
            {
                Console::writeLine("Failed to close the semaphore handle : ",
                                   GetLastError());
            }
        }
        catch (...)
        {
        }
    }

    void WindowsSemaphore::waitImpl() const
    {
        if (_handle)
        {
            const int res = WaitForSingleObject((HANDLE)_handle, INFINITE);
            if (res)
            {
                Console::writeLine(
                    "WaitForSingleObject returned(",
                    res,
                    "): ",
                    GetLastError());
            }
        }
    }

    void WindowsSemaphore::signalImpl() const
    {
        if (_handle)
        {
            if (ReleaseSemaphore(
                    (HANDLE)_handle,  // hSemaphore
                    1,                // lReleaseCount,
                    nullptr           // lpPreviousCount
                    ) == FALSE)
            {
                // Console::writeLine(
                //    "Failed to release the semaphore handle : ",
                //    GetLastError());
            }
        }
    }
}  // namespace Rt2::Threads
