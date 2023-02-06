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
#pragma once

#include <pthread.h>

namespace Rt2::Threads
{
    class PosixMutex
    {
    protected:
        pthread_mutex_t _mutex;
        bool            _isInit;

    protected:
        PosixMutex();
        virtual ~PosixMutex();

        void lockImpl();

        void unlockImpl();

        void waitImpl();

        void waitImpl(size_t milliseconds);

        void notifyImpl();
    };
}  // namespace Rt2::Threads