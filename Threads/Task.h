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

#include <functional>
#include "Utils/Array.h"

namespace Rt2::Threads
{

    class TaskPrivate;
    using TaskCall = std::function<void()>;

    class Task
    {
    private:
        friend TaskPrivate;
        TaskPrivate* _private{nullptr};
        TaskCall     _done{nullptr};

        void notifyDone() const;

        void invoke() const;

        Task& whenDone(const TaskCall& call);

    public:
        explicit Task(const TaskCall& call);
        ~Task();

        static void start(const TaskCall& main, const TaskCall& onDone);

        static void start(const TaskCall& main);

        static void joinAll();
    };
}  // namespace Rt2::Threads
