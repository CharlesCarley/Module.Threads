#pragma once
#ifdef WIN32

#include "errno.h"
struct pthread_mutex_t
{
    int __size;
    int __align[32];
};

typedef int pthread_t;
typedef int pthread_attr_t;

#define PTHREAD_MUTEX_INITIALIZER \
    {                             \
        0, 0                      \
    }
extern int pthread_mutex_init(pthread_mutex_t*, pthread_mutex_t*);
extern int pthread_mutex_destroy(pthread_mutex_t*);
extern int pthread_mutex_lock(pthread_mutex_t*);
extern int pthread_mutex_timedlock(pthread_mutex_t*, timespec*);
extern int pthread_mutex_unlock(pthread_mutex_t*);

extern int pthread_attr_destroy(pthread_attr_t *attr);
extern int pthread_attr_init(pthread_attr_t *attr);

#define PTHREAD_CREATE_JOINABLE 0

extern int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);


extern int pthread_cancel(pthread_t thread);
extern int pthread_join(pthread_t thread, void** retval);


/// https://man7.org/linux/man-pages/man3/pthread_create.3.html
extern int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);

#else

#include <errno.h>
#include <pthread.h>
#endif

#define NULL_PTHREAD {}