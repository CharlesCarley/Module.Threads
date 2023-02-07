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
#include "Threads/ProcessQueue.h"
#include <thread>
#include "Threads/CriticalSection.h"
#include "Threads/Semaphore.h"
#include "Threads/Thread.h"
#include "Utils/Array.h"
#include "Utils/Queue.h"

namespace Rt2::Threads
{
    class ProcessQueuePrivate final : public Thread
    {
    public:
        typedef ShiftingQueue<Packet*> PacketQueue;
        friend class ProcessQueue;

    private:
        enum RunStates
        {
            PqsStopped = 0x01,
            PqsRunning = 0x02,
            PqsFlush   = 0x04,
        };

        Semaphore     _wake;
        PacketQueue   _packets;
        uint8_t       _state{PqsStopped};
        ProcessQueue* _parent;

        uint32_t _maxPacketTemp{0};

    public:
        explicit ProcessQueuePrivate(ProcessQueue* parent) :
            _parent(parent)
        {
            _packets.reserve(32);
        }

        ~ProcessQueuePrivate() override
        {
            finalize();
        }

        void flush()
        {
            exec();
        }

        void finalize() noexcept
        {
            exec();

            _state = PqsStopped;
            _wake.signal();
            join();
        }

        void enqueue(Packet* packet)
        {
            if (packet)
                _packets.enqueue(packet);

            _state = PqsRunning;

            if (_packets.size() == std::thread::hardware_concurrency() - 1)
                _wake.signal();
        }

        void exec()
        {
            try
            {
                CriticalSectionLock lock(&_parent->_criticalSection);
                _maxPacketTemp = Max(_maxPacketTemp, _packets.size());

                while (!_packets.empty())
                {
                    Packet* pack = _packets.dequeue();
                    if (pack != nullptr)
                        pack->handle();
                    delete pack;
                }
                _packets.clear();
            }

            catch (...)
            {
            }
        }

        int update() override
        {
            try
            {
                while (_state != PqsStopped)
                {
                    exec();
                    if (_state != PqsStopped)
                        _wake.wait();
                }

                Console::writeLine("Max ", _maxPacketTemp);
                return 0;
            }
            catch (...)
            {
                return -1;
            }
        }
    };

    ProcessQueue::ProcessQueue()
    {
        _private = new ProcessQueuePrivate(this);
    }

    ProcessQueue::~ProcessQueue()
    {
        delete _private;
    }

    void ProcessQueue::enqueue(Packet* packet)
    {
        RT_ASSERT(_private)
        if (_private->_state == ProcessQueuePrivate::PqsStopped)
            _private->start();

        CriticalSectionLock lock(&_criticalSection);
        if (_private)
            _private->enqueue(packet);
    }

    void ProcessQueue::flush()
    {
        RT_ASSERT(_private)
        //CriticalSectionLock lock(&_criticalSection);
        if (_private)
            _private->flush();
    }

    bool ProcessQueue::isEmpty()
    {
        RT_ASSERT(_private)
        CriticalSectionLock lock(&_criticalSection);
        if (_private)
            return _private->_packets.empty();

        return true;
    }

}  // namespace Rt2::Threads
