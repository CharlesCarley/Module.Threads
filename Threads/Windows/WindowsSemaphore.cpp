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
#include "Threads/Windows/WindowsUtils.h"
#include "Utils/Console.h"
#include "Utils/StreamMethods.h"

namespace Rt2::Threads
{
    WindowsSemaphore::WindowsSemaphore()
    {
        if (HANDLE handle = CreateSemaphore(
                nullptr,  // Cannot be inherited by child processes.
                0,        // lInitialCount
                1,        // lMaximumCount
                nullptr   // lpName -  No name
            );
            handle == nullptr)
            LogError("failed to create semaphore", FALSE);
        else
            _sem = (SemaphoreHandle)handle;
    }

    WindowsSemaphore::~WindowsSemaphore()
    {
        if (const HANDLE hand = toHandle(_sem))
        {
            if (CloseHandle(hand) == FALSE)
                LogError("failed to close handle", FALSE);
            _sem = NullHandle;
        }
    }

    void WindowsSemaphore::waitImpl()
    {
        if (const HANDLE hand = toHandle(_sem))
        {
            if (WaitForSingleObject(hand, INFINITE) == WAIT_FAILED)
                LogError("failed to wait on handle", WAIT_FAILED);
        }
    }

    void WindowsSemaphore::signalImpl()
    {
        if (const HANDLE hand = toHandle(_sem))
        {
            if (ReleaseSemaphore(hand, 1, nullptr) == FALSE)
                LogError("failed to release handle: ", FALSE);
        }
    }
}  // namespace Rt2::Threads
