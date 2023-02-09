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
#include "Threads/Windows/WindowsMutex.h"
#include "Threads/Windows/WindowsUtils.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{
    WindowsMutex::WindowsMutex()
    {
        if (HANDLE handle = CreateMutex(
                nullptr,  // lpMutexAttributes
                FALSE,    // bInitialOwner
                nullptr   // lpName -  No name
            );
            handle == nullptr)
            LogError("failed to create mutex", FALSE);
        else
            _mutex = (MutexHandle)handle;
    }

    WindowsMutex::~WindowsMutex()
    {
        if (const HANDLE hand = toHandle(_mutex))
        {
            if (CloseHandle(hand) == FALSE)
                LogError("failed to close handle", FALSE);
            _mutex = NullHandle;
        }
    }

    void WindowsMutex::lockImpl()
    {
        if (!_isLocked)
        {
            _isLocked = true;
            waitImpl();
        }
    }

    void WindowsMutex::unlockImpl()
    {
        if (_isLocked)
        {
            _isLocked = false;
            waitImpl();
        }
    }

    void WindowsMutex::waitImpl()
    {
        waitImpl(INFINITE);
    }

    void WindowsMutex::waitImpl(const size_t ms)
    {
        if (_mutex != NullHandle)
        {
            if (const HANDLE handle = toHandle(_mutex))
            {
                if (WaitForSingleObject(handle, (DWORD)ms) == WAIT_FAILED)
                    LogError("failed to wait", WAIT_FAILED);

                if (ReleaseMutex(handle) == FALSE)
                    LogError("failed to release mutex", FALSE);

                _isLocked = false;
            }
        }
    }
}  // namespace Rt2::Threads
