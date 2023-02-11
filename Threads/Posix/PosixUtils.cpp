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
#include "Threads/Posix/PosixUtils.h"
#include "Utils/Console.h"

namespace Rt2::Threads
{
    void LogError(const char* message, const int err)
    {
        switch (err)
        {
        case EAGAIN:
            Console::writeError("system error\n", message);
            break;
        case ENOMEM:
            Console::writeError("out of memory\n", message);
            break;
        case EPERM:
            Console::writeError("insufficient privileges\n", message);
            break;
        case EBUSY:
            Console::writeError("reentry error\n", message);
            break;
        case EINVAL:
            Console::writeError("invalid parameter\n", message);
            break;
        default:
            Console::writeError("undefined error ", err, "\n", message);
            break;
        }
    }

}  // namespace Rt2::Threads
