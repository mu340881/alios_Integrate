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
#include "wifi_api.h"



FLASH_FUNC char ConvertHexChar(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return -1;
}

FLASH_FUNC int hex2byte(const char *hex)
{
	int a, b;
	a = ConvertHexChar(*hex++);
	if (a < 0)
		return -1;
	b = ConvertHexChar(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}


/**
 * hexstr2bin - Convert ASCII hex string into binary data
 * @hex: ASCII hex string (e.g., "01ab")
 * @buf: Buffer for the binary data
 * @len: Length of the text to convert in bytes (of buf); hex will be double
 * this size
 * Returns: 0 on success, -1 on failure (invalid hex string)
 */
FLASH_FUNC int hexstr2bin(u8 *buf, const char *hex, size_t len)
{
	size_t i;
	int a;
	const char *ipos = hex;
	u8 *opos = (u8 *)buf;

	for (i = 0; i < len; i++) {
		a = hex2byte(ipos);
		if (a < 0)
			return -1;
		*opos++ = a;
		ipos += 2;
	}
	return 0;
}

FLASH_FUNC int str2mac(char *macstr, unsigned char *buf)
{
	unsigned char  i = 0;
	char tmp = 0;
	for(i =0; i< 5; i++)
	{
		tmp = ConvertHexChar(*macstr++);
		if(tmp < 0)
		{
			return -1;
		}
		buf[i] = tmp*16;
		tmp = ConvertHexChar(*macstr++);
		if(tmp < 0)
		{
			return -1;
		}
		buf[i] = buf[i]+tmp;
		macstr++;
	}
	tmp= ConvertHexChar(*macstr++);
	if(tmp < 0)
	{
		return -1;
	}
	buf[5] = tmp*16;
	tmp = ConvertHexChar(*macstr);
	if(tmp < 0)
	{
		return -1;
	}
	buf[5] = buf[5]+tmp;
	return 0;
}



NORELOC FLASH_FUNC  char * CmdLine_SkipSpace(char * line)
{
    char ch;
	int loop =0;
	

    /* escape white space */
    ch = line[0];
    while(ch != 0)
    {
        /* CmdLine_GetLine() has already replaced '\n' '\r' with 0 */
        if ( (ch == ' ') || (ch == ',') || (ch == '\t') || (ch == '='))
        {
            line++;
            ch = line[0];
			//¨¦¡ª???¡§???¨¦?¡¯?¡À?T cmd ¨¦????¡®?1|¨¦?¡­???	    	loop++;
			if(loop > 1600)
				break;
			
            continue;
        }
        break;
    }
    return line;
}

/**************************************************************************
**
** NAME        CmdLine_GetToken
**
** PARAMETERS:    *pLine -    current line location to parse.
**
** RETURNS:        the token located. It never be NULL, but can be "\0"
**              *pLine - next line location to parse.
**
** DESCRIPTION    Locate the next token from a cli.
**
**************************************************************************/
NORELOC FLASH_FUNC  char * CmdLine_GetToken(char ** pLine)
{
    char *    str;
    char *    line;
    char ch;
	int loop =0;
	
    line = *pLine;

    /* escape white space */
    ch = line[0];
    while(ch != 0)
    {
        /* CmdLine_GetLine() has already replaced '\n' '\r' with 0 */
        if ( (ch == ' ') || (ch == ',') || (ch == '\t') || (ch == '='))
        {
            line++;
            ch = line[0];
			//¨¦¡ª???¡§???¨¦?¡¯?¡À?T cmd ¨¦????¡®?1|¨¦?¡­???	    	loop++;
			if(loop > 1600)
				break;
			
            continue;
        }
        break;
    }

    str = line;
    while(ch != 0)
    {
        if ( (ch == ' ') || (ch == ',') || (ch == '\t') || (ch == '='))
        {
            line[0] = 0;
            /* CmdLine_GetLine() has replaced '\n' '\r' with 0, so we can do line++ */
            line++;
            break;
        }
        line++;
        ch = line[0];
		//¨¦¡ª???¡§???¨¦?¡¯?¡À?T cmd ¨¦????¡®?1|¨¦?¡­???    	loop++;
		if(loop > 1600)
			break;
    }

    *pLine = line;

    return str;
}
/*get string between " " */
char * CmdLine_GetToken_String(char ** ppLine)
{
	
	char *	  str;
	char *	  pLine;
	int index = 0;
	int first_flag =0;
	
	pLine = *ppLine;
	
	
	pLine = CmdLine_SkipSpace(pLine);
	if(pLine[0] == '"'){
		first_flag=1;
		index=1;
		str = &pLine[1];
		while(pLine[index] != 0)
		{
			if (pLine[index] == '"')
			{
				first_flag++;
				//if (first_flag == 1)
				//{
				//	str = &pLine[index + 1];
				//}else
				//{
					pLine[index] = 0;
					index++;
					break;
				//}
			}
			index++;
		}
	}
		
	if (first_flag == 2)
		pLine += index;
	else{
		str = CmdLine_GetToken(&pLine);
	}
	*ppLine=pLine;
	
	return str;
}


/**************************************************************************
**
** NAME        CmdLine_GetHex
**
** PARAMETERS:  *pLine - the current line location to parse.
**
** RETURNS:        TRUE if the next token is a hexdecimal integer.
**              *pDword - the integer returned. Unchanged if return FALSE.
**              *pLine - next line location to parse.
**
** DESCRIPTION    Read a hexdecimal integer from a cli.
**
**************************************************************************/
NORELOC FLASH_FUNC  int CmdLine_GetHex(char **pLine, uint32 *pDword)
{
    char *  str;
    char *  str0;
    int     got_hex;
    uint32  d = 0;

    str = CmdLine_GetToken(pLine);
    if (str[0] == 0)
    {
        return FALSE;
    }

    str0 = str;
    got_hex = FALSE;
    for (;;)
    {
        char    ch;

        ch = str[0];
        if (ch == 0)
        {
            break;
        }
        if (ch >= '0' && ch <= '9')
        {
            d = (d<<4) | (ch - '0');
        }
        else if (ch >= 'a' && ch <= 'f')
        {
            d = (d<<4) | (ch - 'a' + 10);
        }
        else if (ch >= 'A' && ch <= 'F')
        {
            d = (d<<4) | (ch - 'A' + 10);
        }
        else
        {
            got_hex = FALSE;
            break;
        }
        got_hex = TRUE;
        str++;
    }
    if (got_hex)
    {
        *pDword = d;
    }
    else
    {
        DBG_Printf("Invalid hexdecimal: %s\n", str0);
    }

    return got_hex;
}


/**************************************************************************
**
** NAME        CmdLine_GetInteger
**
** PARAMETERS:  *pLine - the current line location to parse.
**
** RETURNS:        TRUE if the next token is an unsigned decimal integer.
**              *pDword - the integer returned. Unchanged if return FALSE.
**              *pLine - next line location to parse.
**
** DESCRIPTION    Read an unsigned decimal integer from a cli.
**
**************************************************************************/
NORELOC  FLASH_FUNC  int CmdLine_GetInteger(char **pLine, uint32 *pDword)
{
    char *  str;
    char *  str0;
    int     got_int;
    uint32  d = 0;

    str = CmdLine_GetToken(pLine);
    if (str[0] == 0)
    {
        return FALSE;
    }

    str0 = str;
    got_int = FALSE;
    for (;;)
    {
        char    ch;

        ch = str[0];
        if (ch == 0)
        {
            break;
        }
        if (ch >= '0' && ch <= '9')
        {
            d = d*10 + (ch - '0');
            got_int = TRUE;
            str++;
        }
        else
        {
            got_int = FALSE;
            break;
        }
    }
    if (got_int)
    {
        *pDword = d;
    }
    else
    {
        DBG_Printf("Invalid unsigned decimal: %s\n", str0);
    }

    return got_int;
}


/**************************************************************************
**
** NAME        CmdLine_GetInteger
**
** PARAMETERS:  *pLine - the current line location to parse.
**
** RETURNS:        TRUE if the next token is an unsigned decimal integer.
**              *pDword - the integer returned. Unchanged if return FALSE.
**              *pLine - next line location to parse.
**
** DESCRIPTION    Read an signed decimal integer from a cli.
**
**************************************************************************/
NORELOC  FLASH_FUNC  int CmdLine_GetSignInteger(char **pLine, int *pDword)
{
    char *  str;
    char *  str0;
    int     got_int;
	int negativeFlag = 0;
    int  d = 0;

    str = CmdLine_GetToken(pLine);
    if (str[0] == 0)
    {
        return FALSE;
    }

    str0 = str;
    got_int = FALSE;
    for (;;)
    {
        char    ch;

        ch = str[0];
        if (ch == 0)
        {
            break;
        }
		if((ch == '-') && (str0 == str))
		{
			negativeFlag = -1;
            str++;
		}else if (ch >= '0' && ch <= '9')
        {
            d = d*10 + (ch - '0');
            got_int = TRUE;
            str++;
        }
        else
        {
            got_int = FALSE;
            break;
        }
    }
    if (got_int)
    {
    	if (negativeFlag < 0)
        	*pDword = d * negativeFlag;
    	else
    		*pDword = d;	
    }
    else
    {
        DBG_Printf("Invalid unsigned decimal: %s\n", str0);
    }

    return got_int;
}



/**************************************************************************
**
** NAME        CmdLine_GetDecimalFraction_x10
**
** PARAMETERS:  *pLine - the current line location to parse.
**
** RETURNS:        TRUE if the next token is an unsigned decimal integer.
**           
**              *pDword - the decimal fraction returned. Unchanged if return FALSE.
**              *pLine - next line location to parse.
**
** DESCRIPTION    Read an unsigned decimal fraction from a cli.
**
**************************************************************************/
NORELOC  FLASH_FUNC  int CmdLine_GetDecimalFraction_x10(char **pLine, uint32 *pDword)
{
    char *  str;
    char *  str0;
    int     got_int;
    uint32  d = 0;
	uint32  hasDot = 0;

    str = CmdLine_GetToken(pLine);
    if (str[0] == 0)
    {
        return FALSE;
    }

    str0 = str;
    got_int = FALSE;
    for (;;)
    {
        char    ch;

        ch = str[0];
        if (ch == 0)
        {
            break;
        }
        if (ch >= '0' && ch <= '9')
        {
            d = d*10 + (ch - '0');
            got_int = TRUE;
            str++;
        }
		else if(ch == '.')
		{
            str++;
			hasDot = 1;
			continue;
		}
        else
        {
            got_int = FALSE;
            break;
        }
    }
    if (got_int)
    {
    	if(hasDot == 0)
        	*pDword = d*10;
		else
			*pDword = d;
    }
    else
    {
        DBG_Printf("Invalid unsigned decimal Fraction: %s\n", str0);
    }

    return got_int;
}

FLASH_FUNC int parse_mac_cmd(char *pLine, unsigned char *buf)
{
	char * str;
	unsigned char str_len = 0;
	int ret = 0;
	if (!memcmp("ADDR",pLine,strlen("ADDR")))
	{	
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		str_len = (unsigned char)strlen(str);
		if(str_len < 17)
		{
			return -1;
		}
		ret = str2mac(str,buf); 

	}
	else
	{
		return -1;
	}
	return ret;
}

FLASH_FUNC int parse_country_code_cmd(char *pLine, int *countryid)
{
	char * str;

	int ret = 0;

	int country_id = country_MAX;

	if (!memcmp("CODE",pLine,strlen("CODE")))
	{	
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		country_id =  get_country_id(str);
	}
	else{
		return -1;
	}
	if(country_id < country_MAX)
	{
		wifi_printk(WIFI_ATCMD, "country_id:%d\n", country_id);
		atbm_wifi_set_country_code(country_id);	
		*countryid = country_id;
	}
	else{
		ret = -1;
	}
	return ret;
}

FLASH_FUNC int parse_channel_no_cmd(char *pLine, int *channel_no)
{

	int ret = 0;
    uint32 tmpdata = 0;
	int  channel_idx = 0xff;
    char *str = NULL;
	if (!memcmp("NUM",pLine,strlen("NUM")))
	{	
		str = CmdLine_GetToken(&pLine);
		if(str !=NULL)
		{
			CmdLine_GetInteger(&pLine, &tmpdata);
			channel_idx = (int)tmpdata;
		}
		
	}
	else{
		return -1;
	}
	if(channel_idx <= 14)
	{
		wifi_printk(WIFI_ATCMD, "channel_no:%d\n", channel_idx);
		//atbmwifi_ieee80211_channel_country(channel_idx);
		//fixed me
		*channel_no = channel_idx;
	}
	else{
		ret = -1;
	}
	return ret;
}



#define AT_CMD_SET_RETURN_STATUS 1
	


FLASH_FUNC void UART_Send_status(int ret)
{
   
#ifdef AT_CMD_SET_RETURN_STATUS
	char *strTmp0  = "+OK\n";
	char *strTmp1  = "+ERR\n";	
	char *strTmp2  = "+UNSUPPORT\n";	
	Console_SetPolling(1);
	if(ret == 0)
	{
		UART_SendString(strTmp0);
	}
	else if(ret == HAL_ERR_NOT_PRESENT)
	{
		UART_SendString(strTmp2);
	}else
	{
		UART_SendString(strTmp1);
	}
	Console_SetPolling(0);
#endif
}


//AT+WIFI_ETF_START_TX  CH 7  RATE 65   40M 0 GREENFIELD 0
FLASH_FUNC int parse_EtfStartTxCmd(char *pLine,	u8 *Txchannel, int *Txrate,	u8 *Txis40M,	u8 *TxisGreenfield)
{
	uint32 tmpdata = 0;
	char * str;
	
	u8 channel =0;
	int rate = 0;
	u8 is40M = 0;
	u8 isGreenfield = 0;

	
	pLine = CmdLine_SkipSpace(pLine);

	wifi_printk(WIFI_ATCMD,"\nAT_WifiETF_StartTx %s\n",pLine);
	if (!memcmp("CH",pLine,strlen("CH")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		
		CmdLine_GetInteger(&str,&tmpdata);
		channel = tmpdata;
		
		wifi_printk(WIFI_ATCMD,"channel  %d,%s\n",channel,str);
	}else
	{
		DBG_Printf("channel  %d,%s Error!\n",channel,str);
		return -1;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("RATE",pLine,strlen("RATE")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetDecimalFraction_x10(&str, &tmpdata);
		rate = tmpdata;
		wifi_printk(WIFI_ATCMD,"rate %d %s\n",rate,str);
		
	}else
	{
		DBG_Printf("rate  %d,%s Error!\n",rate,str);
		return -2;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("40M",pLine,strlen("40M")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		is40M = tmpdata;
		wifi_printk(WIFI_ATCMD,"is40M %d %s\n",is40M,str);
	}else
	{
		DBG_Printf("is40M  %d,%s Error!\n",is40M,str);
		return -3;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("GREENFIELD",pLine,strlen("GREENFIELD")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		isGreenfield = tmpdata;
		wifi_printk(WIFI_ATCMD,"isGreenfield %d %s\n",isGreenfield,str);
	}else
	{
		DBG_Printf("isGreenfield  %d,%s Error!\n",isGreenfield,str);
		return -4;
	}

	*Txchannel	= channel;
	*Txrate		= rate;
	*Txis40M	= is40M;
	*TxisGreenfield= isGreenfield;

	return 0;
}

//AT+WIFI_ETF_CFG_FILTER  ID 1024  RSSI -100   TXEVM 200 RXEVM 200 FREQ 7
FLASH_FUNC int parse_EtfFilterParam(char *pLine,	uint32 *featureid, int *rssi, int *txevm, int *rxevm, int *freqoffset)
{
	uint32 tmpdata = 0;
	char * str;
	
	uint32 id =0;
	int rssi_filter = 0;
	int txevm_filter = 0;
	int rxevm_filter = 0;
	int targetfreqoffset = 0;

	
	pLine = CmdLine_SkipSpace(pLine);

	wifi_printk(WIFI_ATCMD,"\nAT+WIFI_ETF_CFG_FILTER %s\n",pLine);
	if (!memcmp("ID",pLine,strlen("ID")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		
		CmdLine_GetInteger(&str,&tmpdata);
		id = tmpdata;
		
		wifi_printk(WIFI_ATCMD,"feature id %d,%s\n",id,str);
	}else
	{
		DBG_Printf("channel  %d,%s Error!\n",id,str);
		return -1;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("RSSI",pLine,strlen("RSSI")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		CmdLine_GetInteger(&str, &tmpdata);
		rssi_filter = tmpdata;
		wifi_printk(WIFI_ATCMD,"rssi_filter %d %s\n",rssi_filter,str);
		
	}else
	{
		DBG_Printf("rssi_filter  %d,%s Error!\n",rssi_filter,str);
		return -2;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("TXEVM",pLine,strlen("TXEVM")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		txevm_filter = tmpdata;
		wifi_printk(WIFI_ATCMD,"txevm_filter %d %s\n",txevm_filter,str);
	}else
	{
		DBG_Printf("txevm_filter  %d,%s Error!\n",txevm_filter,str);
		return -3;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("RXEVM",pLine,strlen("RXEVM")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		rxevm_filter = tmpdata;
		wifi_printk(WIFI_ATCMD,"rxevm_filter %d %s\n",rxevm_filter,str);
	}else
	{
		DBG_Printf("rxevm_filter  %d,%s Error!\n",rxevm_filter,str);
		return -4;
	}

	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("FREQ",pLine,strlen("FREQ")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		targetfreqoffset = tmpdata;
		wifi_printk(WIFI_ATCMD,"freqoffset %d %s\n",targetfreqoffset,str);
	}else
	{
		DBG_Printf("freqoffset  %d,%s Error!\n",targetfreqoffset,str);
		return -4;
	}

	*featureid	= id;
	*rssi		= rssi_filter;
	*txevm	= txevm_filter;
	*rxevm= rxevm_filter;
	*freqoffset = targetfreqoffset;
	return 0;
}



FLASH_FUNC int parse_etf_rx_cmd(char *pLine,	u8 *Txchannel, u8 *Txis40M)
{

	uint32 tmpdata = 0;
	char * str;
	
	u8 channel =0;
	u8 is40M = 0;	

	pLine = CmdLine_SkipSpace(pLine);

	wifi_printk(WIFI_ATCMD,"parse_etf_rx_cmd %s\n",pLine);
	if (!memcmp("CH",pLine,strlen("CH")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		
		CmdLine_GetInteger(&str,&tmpdata);
		channel = tmpdata;
		wifi_printk(WIFI_ATCMD,"channel  %d,%s\n",channel,str);
	}else
	{
		DBG_Printf("channel  %d,%s Error!\n",channel,str);
		return -1;
	}
	
	pLine = CmdLine_SkipSpace(pLine);
	if (!memcmp("40M",pLine,strlen("40M")))
	{
		str = CmdLine_GetToken(&pLine);
		str = CmdLine_GetToken(&pLine);
		//
		CmdLine_GetInteger(&str, &tmpdata);
		is40M = tmpdata;
		wifi_printk(WIFI_ATCMD,"is40M %d %s\n",is40M,str);
	}else
	{
		DBG_Printf("is40M  %d,%s Error!\n",is40M,str);
		return -3;
	}


	*Txchannel	= channel;
	*Txis40M	= is40M;

	return 0;
}


