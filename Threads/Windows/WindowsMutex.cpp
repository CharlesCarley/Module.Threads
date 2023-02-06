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
#include "WindowsMutex.h"
#include <Windows.h>
#include "Utils/Console.h"

namespace Rt2::Threads
{
    WindowsMutex::WindowsMutex() :
        _isLocked(false)
    {
        _mutex = (size_t)CreateMutex(nullptr, false, nullptr);
        if (!_mutex)
        {
            Console::writeLine("CreateMutex returned: ", ::GetLastError());
        }
    }

    WindowsMutex::~WindowsMutex()
    {
        if (_mutex)
        {
            ::CloseHandle((HANDLE)_mutex);
            _mutex = 0;
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
            notifyImpl();
        }
    }

    void WindowsMutex::waitImpl() const
    {
        if (_mutex)
        {
            const int res = ::WaitForSingleObject((HANDLE)_mutex, INFINITE);
            if (res)
                Console::writeLine("WaitForSingleObject returned: ", res, ' ', GetLastError());
        }
    }

    void WindowsMutex::waitImpl(size_t milliseconds) const
    {
        if (_mutex)
        {
            const int res = ::WaitForSingleObject((HANDLE)_mutex, (DWORD)milliseconds);
            if (res)
                Console::writeLine("WaitForSingleObject returned: ", res, ' ', ::GetLastError());
        }
    }

    void WindowsMutex::notifyImpl() const
    {
        if (_mutex)
            ::ReleaseMutex((HANDLE)_mutex);
    }
}  // namespace Rt2::Threads
