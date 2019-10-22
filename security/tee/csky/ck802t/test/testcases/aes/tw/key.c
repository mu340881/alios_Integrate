/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "aes_test.h"

uint8_t Key128[16] = {
0x11, 0x4d, 0x54, 0x4d, 0xd0, 0xdc, 0xfd, 0xec, 0x0b, 0x1d, 0xc6, 0x99, 0x0f, 0xcb, 0x97, 0x4a };
uint8_t Key192[24] = {
0x09, 0x43, 0x25, 0xa4, 0x26, 0xe7, 0x10, 0xf4, 0xb5, 0xe8, 0xdb, 0xf3, 0x7d, 0xeb, 0xc7, 0x37,
0x63, 0x93, 0x4a, 0x08, 0x35, 0x74, 0x1b, 0x2b };
uint8_t Key256[32] = {
0x70, 0x51, 0x61, 0xb5, 0x13, 0x97, 0x9e, 0xaa, 0xe2, 0xbb, 0x7f, 0x34, 0x1f, 0x15, 0x48, 0x5a,
0xb0, 0xa5, 0x0e, 0xb8, 0xf1, 0x6f, 0x29, 0xff, 0x04, 0xe2, 0x70, 0x24, 0xc8, 0x52, 0x2a, 0x63 };

uint8_t IV[16] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
