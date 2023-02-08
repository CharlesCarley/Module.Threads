#pragma once

#ifdef WIN32
#include "Utils/Definitions.h"
#if RT_ARCH == RT_ARCH_64
constexpr int SemSize = 32;
#else
constexpr int SemSize = 16;
#endif

union sem_t
{
    char     _size[SemSize];
    long int _align;
};

extern int sem_init(sem_t* sem, int pshared, unsigned int value);
extern int sem_destroy(sem_t* sem);
extern int sem_wait(sem_t* sem);
extern int sem_post(sem_t* sem);
#else
#include <semaphore.h>
#endif