/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __BOARD__H_
#define __BOARD__H_
#define HARDWARE_REVISION "V1.0"
#define MODEL             "ATBM6431"

#ifdef BOOTLOADER
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART_BUADRATE 115200
#endif

#endif /*__BOARD__H_*/
