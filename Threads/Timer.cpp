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
#include "Threads/Timer.h"
#include <cstdint>
#include <functional>
#include <utility>
#include "Thread.h"
#include "Utils/Timer.h"

namespace Rt2::Threads
{
    Timer::Timer(uint32_t milliseconds, Callback callback) :
        _callback(std::move(callback)),
        _duration(milliseconds)
    {
    }

    int Timer::update()
    {
        const uint64_t start   = Rt2::Timer::tickCount();
        uint64_t elapsed = Rt2::Timer::tickCount() - start;

        while (elapsed < _duration)
        {
            {
                CriticalSectionLock lock(&_cs);

                _callback.operator()();
            }
            elapsed = Rt2::Timer::tickCount() - start;
        }
        return 0;
    }

}  // namespace Rt2::Threads
