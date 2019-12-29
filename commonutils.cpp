#include "commonutils.h"



/*
 * 休眠毫秒值
 * @param   secs        毫秒值
 * @return  void
*/
void sleep_ms(unsigned int secs /*in*/) {
#ifdef WIN32
    Sleep(secs);
#else
    struct timeval tval;
    tval.tv_sec=secs/1000;
    tval.tv_usec=(secs*1000)%1000000;
    select(0,NULL,NULL,NULL,&tval);
#endif
}
