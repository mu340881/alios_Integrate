#include <stdio.h>
#include "k_api.h"
#include "k_dbg_api.h"
#include "backtrace.h"


#define PANIC_PRINT     printf

extern int printf(const char *fmt, ...);
