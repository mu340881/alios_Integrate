/**************************************************************************************************************
 * altobeam iot at cmd file 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 * Permission is hereby granted for use on Altobeam IOT 6421 only,
 * in which case ,it is free to  redistribute it and/or modify
 *
*****************************************************************************************************************/


#include "app_AT_cmd.h"
#include "app_flash_param.h"
#include "gpio.h"
#include "uart.h"
#include "pwm/pwm.h"

struct gpio_params{
		uint8 gpio_dir;
		uint8 gpio_pin;
		uint8 gpio_level;
		uint8 pad;
	};
struct gpio_params gpio_user_set;

	
	
	
/*
struct iot_uart_param {
	u32 uart_Baudrate;
	u8	uart_DataBits;
	u8	uart_Parity;
	u8	uart_StopBits;
	u8	reserve;
};*/

struct iot_uart_param uart_user_params;
//AT+UART_CFG_TMP RATE 115200 LEN 8 STOP 1 PARITY 0
FLASH_FUNC int parse_uart_cmd(char *pLine, uint8 saveflag)
{
	uint32 tmpdata = 0;
	char *str = NULL;
	if (!memcmp("RATE",pLine,strlen("RATE")))
	{	
		str = CmdLine_GetToken(&pLine);
		if(str !=NULL)
		{
			CmdLine_GetInteger(&pLine, &tmpdata);
		}
		if((tmpdata > 921600)||(tmpdata < 9600))
		{
			return -1;
		}
		uart_user_params.uart_Baudrate	= tmpdata;		
	}else
	{
		return -1;
	}
	if (!memcmp("LEN",pLine,strlen("LEN")))
	{
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&pLine, &tmpdata);
		uart_user_params.uart_DataBits	= (uint8)tmpdata;
	
	}else
	{
		return -1;
	}
	if (!memcmp("STOP",pLine,strlen("STOP")))
	{
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&pLine, &tmpdata);
		uart_user_params.uart_StopBits = (uint8)tmpdata;		
	}else
	{
		return -1;
	}
	if (!memcmp("PARITY",pLine,strlen("PARITY")))
	{
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&pLine, &tmpdata);
		uart_user_params.uart_Parity = (uint8)tmpdata;
	}
	/*
	if (!memcmp("RTS_CTS",pLine,strlen("RTS_CTS")))  //no RTS CTS control line, here reserved.
	{
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&pLine, &tmpdata);
		uart_user_params.reserve = tmpdata;
	}*/
	if(saveflag)
	{

			iot_boot_volatile_sect.uart_param.uart_Baudrate = uart_user_params.uart_Baudrate;
			iot_boot_volatile_sect.uart_param.uart_DataBits = uart_user_params.uart_DataBits;
			iot_boot_volatile_sect.uart_param.uart_StopBits = uart_user_params.uart_StopBits;
			iot_boot_volatile_sect.uart_param.uart_Parity   = uart_user_params.uart_Parity;
			atbmwifi_flash_param_uartconfig_change();
			wifi_printk(WIFI_ATCMD,"AT+UART_CFG\n");
	}else
		{
			wifi_printk(WIFI_ATCMD,"AT+UART_CFG_TMP\n");
		}	
	return 0;
}

FLASH_FUNC int parse_gpio_value_cmd(char *pLine)
{
	uint32 tmpdata = 0;
	char *str = NULL;
	if (!memcmp("PIN",pLine,strlen("PIN")))
	{	
		str = CmdLine_GetToken(&pLine);
		if(str !=NULL)
		{
			CmdLine_GetInteger(&pLine, &tmpdata);
		}
		gpio_user_set.gpio_pin	= (uint8)tmpdata;			
	}else
	{
		return -1;
	}
	if (!memcmp("VALUE",pLine,strlen("VALUE")))
	{
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&pLine, &tmpdata);
		gpio_user_set.gpio_level  = (uint8)tmpdata;

	}else
	{
		return -1;
	}	
	return 0;
}
FLASH_FUNC int parse_gpio_dir_cmd(char *pLine)
{
	uint32 tmpdata = 0;
	char *str = NULL;
	if (!memcmp("PIN",pLine,strlen("PIN")))
	{	
		str = CmdLine_GetToken(&pLine);
		if(str !=NULL)
		{
			CmdLine_GetInteger(&pLine, &tmpdata);
		}
		gpio_user_set.gpio_pin	= (uint8)tmpdata;			
	}else
	{
		return -1;
	}
	if (!memcmp("DIR",pLine,strlen("DIR")))
	{
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&pLine, &tmpdata);
		gpio_user_set.gpio_dir  = (uint8)tmpdata;

	}else
	{
		return -1;
	}

	return 0;
}

FLASH_FUNC int parse_gpio_pin_cmd(char *pLine)
{
	uint32 tmpdata = 0;
	 char *str = NULL;
	if (!memcmp("PIN",pLine,strlen("PIN")))
	{	
		str = CmdLine_GetToken(&pLine);
		if(str !=NULL)
		{
			CmdLine_GetInteger(&pLine, &tmpdata);
		}
		gpio_user_set.gpio_pin	= (uint8)tmpdata;			
	}else
	{
		return -1;
	}
	return 0;
}



//extern void UART_change_params(u32 rate, unsigned char data_len,  unsigned char stopbits, unsigned char parity);
FLASH_FUNC void AT_UartConfig(char *pLine)
{	
	uint8 saveflag = 1;
	int ret = 0;
	ret = parse_uart_cmd(pLine, saveflag);	
	UART_change_params(uart_user_params.uart_Baudrate,uart_user_params.uart_DataBits, uart_user_params.uart_StopBits, uart_user_params.uart_Parity);
	UART_Send_status(ret);			
}




FLASH_FUNC void AT_UartConfigTmp(char *pLine)
{
	uint8 saveflag = 0;
	int ret = 0;
	ret = parse_uart_cmd(pLine, saveflag);	
	UART_change_params(uart_user_params.uart_Baudrate,uart_user_params.uart_DataBits, uart_user_params.uart_StopBits, uart_user_params.uart_Parity);
	UART_Send_status(ret);
}

FLASH_FUNC void AT_UartStatus(char *pLine)
{	
	
	UN_USED_PARAMS(*pLine);
	UART_SendString(" Baudrate:%d\n DataBits:%d\n StopBits:%d\n Parity:%d\r\n", 
		uart_user_params.uart_Baudrate, uart_user_params.uart_DataBits, uart_user_params.uart_StopBits, uart_user_params.uart_Parity);				 
}


FLASH_FUNC u8 PinNum2GpioChannelNo(u8 PinNum)
{
#if (CHIP_TYPE==CHIP_ATHENAB)
	u8 channel_no = INVALID_CHANNEL_NUM;
	if(PinNum == 19)
	{
		channel_no = GPIO4_CHANNEL_2;					 
	}
	else if(PinNum== 7){
		channel_no = GPIO3_CHANNEL_0;					 
	}
	else if(PinNum == 6){
		channel_no = GPIO2_CHANNEL_1;					
	}
	else if(PinNum == 5){
		channel_no = GPIO1_CHANNEL_3;					
	}else{

	}		
#else /*CHIP_ARES*/
	u8 channel_no = PinNum;
#endif  /*#if (CHIP_TYPE==CHIP_ATHENAB)*/
	return channel_no;

}

/*
AT+GPIO_WRITE PIN 22 VALUE 1
*/
FLASH_FUNC void AT_GPIOWrite(char *pLine)
{		
	unsigned char channel_no = 0;
	unsigned char u8level = 0;
	int ret = 0;
	ret = parse_gpio_value_cmd(pLine);
	if(!ret)
	{
		u8level = gpio_user_set.gpio_level;
		//Mapping Pin pad number to GPO channel number.
		channel_no = PinNum2GpioChannelNo(gpio_user_set.gpio_pin);
		if(channel_no  <= MAX_GPIO_PIN_NUM) 
		{
			if(u8level){
				gpio_output_value(channel_no,1);
			}
			else{
				gpio_output_value(channel_no,0);
			}
		}
		else{
			ret = -1;
		}
	}		
	UART_Send_status(ret);
}

FLASH_FUNC void AT_GPIORead(char *pLine)
{	
	unsigned char channel_no = 0;
	unsigned char u8level = 0, u8dir = 0;
	int ret = 0;		
	char strTmp[32] = {0};

	ret = parse_gpio_pin_cmd(pLine);
	channel_no = PinNum2GpioChannelNo(gpio_user_set.gpio_pin);

	if((!ret)&&(channel_no <= MAX_GPIO_PIN_NUM))
	{

	   u8dir = get_gpio_dir_value(channel_no);
	   if(u8dir){
			u8level = get_gpio_output_value(channel_no);  //output pin
	   }
	   else{
			u8level = gpio_input_value(channel_no); 	  //input pin
	   }			
		sprintf(strTmp, "pin:%d level:%d\r\n", gpio_user_set.gpio_pin, u8level);
	}
	else
	{
		sprintf(strTmp, "params error!\r\n");
		ret = -1;
	}	
	UART_SendString("%s", strTmp);
}

FLASH_FUNC void AT_GPIOToggle(char *pLine)
{
	unsigned char channel_no = 0;
	int ret = 0;
	char strTmp[32] = {0};
	ret = parse_gpio_pin_cmd(pLine);
	channel_no = PinNum2GpioChannelNo(gpio_user_set.gpio_pin);

	if((!ret)&&(channel_no <= MAX_GPIO_PIN_NUM))
	{

		gpio_toggle_value(channel_no);
	}
	else
	{
		sprintf(strTmp, "params error!\r\n");
		ret = -1;
	}

	UART_SendString("%s", strTmp);
}

/*
AT+GPIO_SET_DIR PIN 22 DIR 1
*/
FLASH_FUNC void AT_GPIOSetDir(char *pLine)
{	
	unsigned char channel_no = 0;
	unsigned char dir_output = 0;
	int ret = 0;
	ret =parse_gpio_dir_cmd(pLine);
	if(!ret)
	{
		dir_output = gpio_user_set.gpio_dir;

		//Mapping Pin pad number to GPO channel number. 		
		channel_no = PinNum2GpioChannelNo(gpio_user_set.gpio_pin);
		if(channel_no  <= MAX_GPIO_PIN_NUM) 
		{
			gpio_set_pin_func(channel_no,GPIO_FUNC);
			if(dir_output)
			{
				gpio_output_config(channel_no);
			}else
			{	
				gpio_input_config(channel_no);
			}
		}else
		{
			ret = -1;
		}
	}
	UART_Send_status(ret);
}


FLASH_FUNC void AT_GPIOGetDir(char *pLine)
{	
	unsigned char channel_no = 0;
	u32 u8dir = 0;
	int ret = 0;		
	char strTmp[32] = {0};

	ret = parse_gpio_pin_cmd(pLine);	
	channel_no = gpio_user_set.gpio_pin;

	//Mapping Pin pad number to GPO channel number. 	   
	channel_no = PinNum2GpioChannelNo(gpio_user_set.gpio_pin);
	if((!ret)&&(channel_no <= MAX_GPIO_PIN_NUM))
	{

        iot_printf("channel_no = %d\n",channel_no);
		u8dir = get_gpio_dir_value(channel_no);
		sprintf(strTmp, "pin:%d dir:%d\r\n", gpio_user_set.gpio_pin, u8dir);

	}
	else
	{
		sprintf(strTmp, "params error!\r\n");
	}
	UART_SendString("%s", strTmp);
}

#if((CHIP_TYPE == CHIP_ARES) || ((CHIP_TYPE == CHIP_ARES_B)))
//AT+SET_PMW 2 100 100

FLASH_FUNC void AT_SetPWM(char *pLine)
{	
	unsigned int pin = 0;
	unsigned int pwm_h = 0;
	unsigned int pwm_l = 0;
	int ret = -1;

	
    if (CmdLine_GetInteger(&pLine, &pin))
    {
        if (CmdLine_GetInteger(&pLine, &pwm_h))
        {
	        if (CmdLine_GetInteger(&pLine, &pwm_l))
	        {
	        	if(pin  <= MAX_GPIO_PIN_NUM) 
				{
					iot_printf("PWM_FUNC pin %d,high %d,low %d\n",pin,pwm_h,pwm_l);
					gpio_set_pin_func(pin,PWM_FUNC);	
					pwm_start(pin,pwm_h,pwm_l);
					ret =0;
				}
				
	        }
        }
    }

	UART_Send_status(ret);
}

FLASH_FUNC void AT_StopPWM(char *pLine)
{
	unsigned int pin = 0;
	int ret = -1;
	if (CmdLine_GetInteger(&pLine, &pin))
	{
		pwm_stop(pin);
		ret = 0;
	}

	UART_Send_status(ret);
}

#endif  //(CHIP_TYPE==CHIP_ARES)

#if ((CHIP_TYPE == CHIP_ARES_B))
//AT+SET_PMW 2 100 100
FLASH_FUNC void AT_SetRTCPWM(char *pLine)
{	
	unsigned int pin = 0;
	unsigned int pwm_h = 0;
	unsigned int pwm_l = 0;
	int ret = -1;

	
    if (CmdLine_GetInteger(&pLine, &pin))
    {
        if (CmdLine_GetInteger(&pLine, &pwm_h))
        {
	        if (CmdLine_GetInteger(&pLine, &pwm_l))
	        {
	        	if((pin  <= MAX_GPIO_PIN_NUM) &&(BIT(pin) & PWM_PIN_MASK))
				{
					if((pwm_h==0)&&(pwm_l==0)){
						iot_printf("rtc_pwm_stop pin=%d\n",pin);
						rtc_pwm_stop(pin);
						ret =0;
					}
					else {
						u8 id=rtc_pwm_alloc();
						if(id ==RTC_PWM_NUM_INVALD){
							iot_printf("All RTC PWM has used,please stop one,try again\n");
						}
						else {
							iot_printf("rtc_pwm_start pwmid=%d pin %d\n",id,pin);
							if(rtc_pwm_start(pin,id,pwm_h,pwm_l)){
								ret =0;
							}
							else {
								iot_printf("rtc_pwm_start fail,please check\n");
							}
						}
					}
				}
				else {					
					iot_printf("rtc_pwm not support pin %d\n",pin);
				}
	        }
        }
    }

	UART_Send_status(ret);
}


#endif  //(CHIP_TYPE==CHIP_ARESB)

FLASH_FUNC void AT_GetADCValue(char *pLine)
{	
#if((CHIP_TYPE == CHIP_ARES) || ((CHIP_TYPE == CHIP_ARES_B)))
	AT_adc();
#else
	UART_Send_status(HAL_ERR_NOT_PRESENT);
#endif  //(CHIP_TYPE==CHIP_ARES)
}

FLASH_FUNC void at_init_get_boot_params(void)
{
	//uart_user_params.uart_Baudrate = 9600;
	uart_user_params.uart_Baudrate = iot_boot_volatile_sect.uart_param.uart_Baudrate;
	uart_user_params.uart_DataBits = iot_boot_volatile_sect.uart_param.uart_DataBits;
	uart_user_params.uart_StopBits = iot_boot_volatile_sect.uart_param.uart_StopBits;
	uart_user_params.uart_Parity   = iot_boot_volatile_sect.uart_param.uart_Parity;
}
#if (CHIP_TYPE==CHIP_ARES_B)
FLASH_FUNC void AT_FlashDeepSleep(char *pLine)
{
	UN_USED_PARAMS(*pLine);

	atbmwifi_flash_chip_deepSleep();

	
}
FLASH_FUNC void AT_FlashEncKey(char *pLine)
{
	unsigned int flashEncKey[4];
	int ret = -1;

	memcpy(&flashEncKey[0], 0, sizeof(flashEncKey));
    if (CmdLine_GetHex(&pLine, &flashEncKey[0]) &&
		CmdLine_GetHex(&pLine, &flashEncKey[1]) &&
		CmdLine_GetHex(&pLine, &flashEncKey[2]) &&
		CmdLine_GetHex(&pLine, &flashEncKey[3]))
    {
		if ((ret = atbmwifi_efuse_flash_enc_key(&flashEncKey[0], sizeof(flashEncKey))) >= 0)
		{
			
			UART_SendString("+OK=%08x:%08x:%08x:%08x\n", flashEncKey[0], flashEncKey[1], flashEncKey[2], flashEncKey[3]);
			return;
		}
    }
	
	UART_Send_status(ret);	
}

#endif

struct cli_cmd_struct ATCommands_dev[] =
{
	{.cmd ="AT+UART_CFG_TMP",	.fn = AT_UartConfigTmp,  .next = (void*)0},
	{.cmd ="AT+UART_CFG",		.fn = AT_UartConfig,	 .next = (void*)0},
	{.cmd ="AT+UART_GET_CFG",	.fn = AT_UartStatus,	 .next = (void*)0},

	{.cmd ="AT+GET_ADC",		.fn = AT_GetADCValue,	 .next = (void*)0},

#if((CHIP_TYPE == CHIP_ARES) || ((CHIP_TYPE == CHIP_ARES_B)))
	{.cmd ="AT+SET_PMW",		.fn = AT_SetPWM,	 .next = (void*)0},
	{.cmd ="AT+STOP_PMW",		.fn = AT_StopPWM,	 .next = (void*)0},
#endif //#if (CHIP_TYPE==CHIP_ARES)

#if((CHIP_TYPE == CHIP_ARES_B))
	{.cmd ="AT+SET_RTCPMW",		.fn = AT_SetRTCPWM,	 .next = (void*)0},
#endif //#if (CHIP_TYPE==CHIP_ARES)

	{.cmd ="AT+GPIO_WRITE", 	.fn = AT_GPIOWrite, 	 .next = (void*)0},
	{.cmd ="AT+GPIO_READ",		.fn = AT_GPIORead,		 .next = (void*)0},
	{.cmd ="AT+GPIO_TOGGLE",	.fn = AT_GPIOToggle,	 .next = (void*)0},
	{.cmd ="AT+GPIO_SET_DIR",	.fn = AT_GPIOSetDir,	 .next = (void*)0},
	{.cmd ="AT+GPIO_GET_DIR",	.fn = AT_GPIOGetDir,	 .next = (void*)0},

#if (CHIP_TYPE==CHIP_ARES_B)
	{.cmd ="AT+FLASH_DEEP_SLEEP",.fn = AT_FlashDeepSleep,   .next = (void*)0},
	{.cmd ="AT+FLASH_ENC_KEY",	 .fn = AT_FlashEncKey,   	.next = (void*)0},
#endif
};

FLASH_FUNC void ATCmd_init_dev(void)
{
	 cli_add_cmds(ATCommands_dev,sizeof(ATCommands_dev)/sizeof(ATCommands_dev[0]));
 
}

