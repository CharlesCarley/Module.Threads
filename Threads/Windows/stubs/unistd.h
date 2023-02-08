#pragma once
#ifdef WIN32
void usleep(unsigned long long microsec);
#else
#include <unistd.h>
#endif
