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
#include "Threads/Thread.h"
#include "Threads/CriticalSection.h"
#include "Utils/StreamMethods.h"

namespace Rt2::Threads
{

    class TaskManager
    {
    private:
        friend class Task;
        using TaskList = Array<Task*>;
        static TaskManager*    _inst;
        TaskList               _tasks;
        static CriticalSection _section;

        void push(Task* task)
        {
            if (task)
                _tasks.push_back(task);
        }

        void remove(Task* task)
        {
            if (const U32 loc = _tasks.find(task); loc != Npos32)
            {
                const Task* dangle = _tasks.at(loc);
                _tasks.remove(loc);
                delete dangle;
            }
        }

        void clear()
        {
            // Console::writeLine("Tasks: ", _tasks.size());
            // for (const auto task : _tasks)
            //     Console::writeLine("     : ", Hex((size_t)task));
            for (const auto task : _tasks)
                delete task;
            _tasks.clear();
        }

        TaskManager()
        {
            _inst = this;
        }

        ~TaskManager()
        {
            clear();
            _inst = nullptr;
        }

        static TaskManager& get()
        {
            static TaskManager mgr;
            ScopeLockCs(_section);
            return *_inst;
        }
    };

    CriticalSection TaskManager::_section;
    TaskManager*    TaskManager::_inst = nullptr;

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

    void Task::invoke() const
    {
        if (_private)
            _private->start();
    }

    Task& Task::whenDone(const TaskCall& call)
    {
        _done = call;
        return *this;
    }

    void Task::start(const TaskCall& main)
    {
        Task* tsk = new Task(main);
        tsk->whenDone([=] {});
        TaskManager::get().push(tsk);
        tsk->invoke();
    }

    void Task::joinAll()
    {
        TaskManager::get().clear();
    }

    void Task::start(const TaskCall& main, const TaskCall& onDone)
    {
        Task* tsk = new Task(main);
        tsk->whenDone(onDone);
        TaskManager::get().push(tsk);
        tsk->invoke();
    }

    void Task::notifyDone() const
    {
        if (_done)
            _done();
    }

}  // namespace Rt2::Threads
