/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  modify it
 *
*****************************************************************************************************************/

#include "app_AT_cmd.h"
#include "sys_test.h"

FLASH_FUNC void AT_sys_test(char* param)
{
	UN_USED_PARAMS(*param);
	sys_test();
	
	UART_Send_status(0);
	return;
}

struct cli_cmd_struct ATCommands_Test[] =
{
	{.cmd ="AT+SYS_TEST",				.fn = AT_sys_test, 			.next = (void*)0},
};

FLASH_FUNC void ATCmd_init_test(void)
{
	cli_add_cmds(ATCommands_Test,sizeof(ATCommands_Test)/sizeof(ATCommands_Test[0]));
	return;
}

