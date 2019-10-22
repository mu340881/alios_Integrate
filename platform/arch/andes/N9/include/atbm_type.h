#ifndef ATBM_TYPE_H
#define ATBM_TYPE_H


#include "sys/types.h"

typedef void (*IRQ_HANDLER)(unsigned IrqPriority);

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define UN_USED_PARAMS (void)


#endif  //ATBM_TYPE_H
