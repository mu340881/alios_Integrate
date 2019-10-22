/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#ifndef APP_NET_AT_CMD_H_
#define APP_NET_AT_CMD_H_

#include "hmac_timer.h"

#define AT_LinkMax 4

typedef struct
{
	BOOL linkEn;
	uint8_t linkId;
	struct atbmconn *pCon;
}AT_LinkConType;

FLASH_FUNC void AT_socketmux(char* pLine);

FLASH_FUNC void AT_socketstatus(char* pLine);

FLASH_FUNC void AT_socketserver(char* pLine);

FLASH_FUNC void AT_servermaxconn(char* pLine);

FLASH_FUNC void AT_socketsend(char* pLine);

FLASH_FUNC void AT_socketconn(char* pLine);

#endif /* APP_NET_AT_CMD_H_ */
