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
#include "Threads/CriticalSection.h"
#include "Threads/ProcessQueue.h"
#include "Threads/Task.h"
#include "Threads/MemoryDataStore.h"
#include "Utils/Console.h"
#include "Utils/FixedArray.h"
#include "Utils/String.h"
#include "gtest/gtest.h"

using namespace Rt2;
using namespace Threads;
using OutputDatabase = MemoryDataStore<String, 32>;

GTEST_TEST(Threads, Thread_001)
{
    int x = 0;
    Task task(
        [&x]
        {
            while (x < 10000)
                x += 1;
        });
    (void)task.whenDone(
                  [&x]
                  {
                      Console::writeLine("The Value of x = ", x);
                      EXPECT_EQ(x, 10000);
                  })
        .invoke()
        .wait();
}

GTEST_TEST(Threads, Thread_002)
{
    OutputDatabase localMemory;
    OutputDatabase::clear();

    for (int i = 0; i < 16; ++i)
    {
        Task([]
             { OutputDatabase::insert("Hello"); })
            .invoke();
        Task([]
             { OutputDatabase::insert("World"); })
            .invoke();
    }

    const OutputDatabase::Database& db = OutputDatabase::get();

    for (const String& str : db)
    {
        Console::writeLine(str);
    }
}

class TestThreadPacket final : public Packet
{
private:
    String _name;

public:
    explicit TestThreadPacket(const char* name)
    {
        _name = name;
    }

    int handle() override
    {
        Console::writeLine("Inserting ", _name);
        Console::writeLine("Done");
        std::flush(std::cout);

        OutputDatabase::insert(_name);
        return 0;
    }
};

class TestPacket final : public Packet
{
public:
    explicit TestPacket(const int data) :
        Packet(data)
    {
    }

    int handle() override
    {
        OutputStringStream oss;
        oss << "Background packet " << getId() << std::endl;

        OutputDatabase::insert(oss.str());
        return 0;
    }
};

GTEST_TEST(Threads, Thread_003)
{
    OutputDatabase output;
    ProcessQueue   queue;
    queue.enqueue(new TestPacket(1));
    queue.enqueue(new TestPacket(2));
    queue.enqueue(new TestPacket(3));
    queue.enqueue(new TestPacket(4));
    queue.flush();

    OutputDatabase::Database& data = OutputDatabase::get();

    OutputStringStream oss;
    for (String& el : data)
        oss << el;

    const String expected =
        "Background packet 1\n"
        "Background packet 2\n"
        "Background packet 3\n"
        "Background packet 4\n";

    EXPECT_EQ(expected, oss.str());
}
