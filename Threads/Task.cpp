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
#include "Threads/Task.h"
#include <cstdint>
#include "Threads/Thread.h"

namespace Rt2::Threads
{
    class TaskPrivate final : public Thread
    {
    private:
        const Task* _parent;
        TaskCall    _call;

    public:
        explicit TaskPrivate(const Task* parent, TaskCall call) :
            _parent(parent),
            _call(std::move(call))
        {
        }

        int update() override
        {
            if (_call)
                _call();
            if (_parent)
                _parent->notifyDone();
            return 0;
        }
    };

    Task::Task(const TaskCall& call)
    {
        _private = new TaskPrivate(this, call);
    }

    Task::~Task()
    {
        if (_private)
            _private->join();
        delete _private;
    }

    const Task& Task::invoke() const
    {
        if (_private)
            _private->start();
        return *this;
    }

    const Task& Task::wait(uint32_t ms) const
    {
        if (_private)
            _private->wait(ms);
        return *this;
    }

    const Task& Task::whenDone(const TaskCall& call)
    {
        _done = call;
        return *this;
    }

    void Task::notifyDone() const
    {
        if (_done)
            _done();
    }

}  // namespace Rt2::Threads
