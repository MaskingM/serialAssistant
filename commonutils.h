#ifndef COMMONUTILS_H
#define COMMONUTILS_H
#include <iostream>
#ifdef WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#endif
using namespace std;
/*
 * 休眠毫秒值
 * @param   secs        毫秒值
 * @return  void
*/
void sleep_ms(unsigned int secs /*in*/);

#endif // COMMONUTILS_H
