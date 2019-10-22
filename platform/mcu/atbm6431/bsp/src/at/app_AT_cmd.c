/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/

#include "app_AT_cmd.h"


	
extern FLASH_FUNC void at_init_get_boot_params(void);
extern FLASH_FUNC void ATCmd_init_wifi(void);
extern FLASH_FUNC void ATCmd_init_dev(void);
extern FLASH_FUNC void ATCmd_init_sys(void);
extern FLASH_FUNC void ATCmd_init_net(void);
extern FLASH_FUNC void ATCmd_init_test(void);
extern FLASH_FUNC void ATCmd_init_ssfs(void);
extern FLASH_FUNC void ATCmd_init_mem(void);

FLASH_FUNC void ATCmd_init(void)
{
#ifdef CONFIG_AT_CMD_SUPPORT
	at_init_get_boot_params();
	// cli_add_cmds(ATCommands,sizeof(ATCommands)/sizeof(ATCommands[0]));
	ATCmd_init_net();

#ifdef CONFIG_MQTT_AT_CMD
	ATCmd_init_mqtt();
#endif //#ifndef ARDUINO
	//ATCmd_init_test();
	ATCmd_init_dev();
    ATCmd_init_wifi();
	ATCmd_init_sys();
#ifdef CONFIG_SSFS
	ATCmd_init_ssfs();
#endif //CONFIG_SSFS
	//	ATCmd_init_mem();
#endif //CONFIG_AT_CMD_SUPPORT
}

FLASH_FUNC void ATCmd_loop(void)
{	
#ifdef CONFIG_AT_CMD_SUPPORT
	cli_main();
#endif //CONFIG_AT_CMD_SUPPORT
}
