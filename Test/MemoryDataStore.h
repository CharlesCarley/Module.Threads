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
#include "Threads/CriticalSection.h"
#include "Utils/FixedArray.h"

namespace Rt2
{
    template <typename T, uint16_t Capacity>
    class MemoryDataStore
    {
    public:
        using Database = FixedArray<T, Capacity + 1>;

    private:
        static MemoryDataStore* db;

        Database                 _data;
        Threads::CriticalSection _cs;

    public:
        MemoryDataStore()
        {
            db = this;
        }

        ~MemoryDataStore()
        {
            db = nullptr;
        }

        static typename Database::SizeType size()
        {
            RT_ASSERT(db);
            Threads::CriticalSectionLock lock(&db->_cs);
            return db->_data.size();
        }

        static void clear()
        {
            RT_ASSERT(db);
            Threads::CriticalSectionLock lock(&db->_cs);
            db->_data.clear();
        }
        

        template <typename... Args>
        static void insert(Args&&... argumentList)
        {
            RT_ASSERT(db);
            Threads::CriticalSectionLock lock(&db->_cs);

            if (db->_data.size() + 1 < Database::limit)
            {
                OutputStringStream oss;
                ((oss << std::forward<Args>(argumentList)), ...);

                db->_data.push_back(oss.str());
            }
            else
            {
                Console::writeLine("Database Limit ",
                                   Database::limit,
                                   " exceeded");
            }
        }

        static Database& get()
        {
            RT_ASSERT(db != nullptr);
            return db->_data;
        }
    };

    template <typename T, uint16_t Capacity>
    MemoryDataStore<T, Capacity>* MemoryDataStore<T, Capacity>::db = nullptr;

}  // namespace Rt2
