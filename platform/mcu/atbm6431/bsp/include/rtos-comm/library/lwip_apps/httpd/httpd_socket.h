/**
 * @file
 * HTTP server
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * This version of the file has been modified by Texas Instruments to offer
 * simple server-side-include (SSI) and Common Gateway Interface (CGI)
 * capability.
 */

#ifndef LWIP_HDR_APPS_HTTPD_H_1
#define LWIP_HDR_APPS_HTTPD_H_1



#ifdef __cplusplus
extern "C" {
#endif
	
#include "httpd_structs_socket.h"

#if LWIP_HTTPD_SSI
#define  LWIP_HTTPD_SSI_EX  1

/*
 * Function pointer for the SSI tag handler callback.
 *
 * This function will be called each time the HTTPD server detects a tag of the
 * form <!--#name--> in a .shtml, .ssi or .shtm file where "name" appears as
 * one of the tags supplied to http_set_ssi_handler in the ppcTags array.  The
 * returned insert string, which will be appended after the the string
 * "<!--#name-->" in file sent back to the client,should be written to pointer
 * pcInsert.  iInsertLen contains the size of the buffer pointed to by
 * pcInsert.  The iIndex parameter provides the zero-based index of the tag as
 * found in the ppcTags array and identifies the tag that is to be processed.
 *
 * The handler returns the number of characters written to pcInsert excluding
 * any terminating NULL or a negative number to indicate a failure (tag not
 * recognized, for example).
 *
 * Note that the behavior of this SSI mechanism is somewhat different from the
 * "normal" SSI processing as found in, for example, the Apache web server.  In
 * this case, the inserted text is appended following the SSI tag rather than
 * replacing the tag entirely.  This allows for an implementation that does not
 * require significant additional buffering of output data yet which will still
 * offer usable SSI functionality.  One downside to this approach is when
 * attempting to use SSI within JavaScript.  The SSI tag is structured to
 * resemble an HTML comment but this syntax does not constitute a comment
 * within JavaScript and, hence, leaving the tag in place will result in
 * problems in these cases.  To work around this, any SSI tag which needs to
 * output JavaScript code must do so in an encapsulated way, sending the whole
 * HTML <script>...</script> section as a single include.
 */
typedef u16 (*tSSIHandler)(char *pcInsert, int iInsertLen);



typedef struct
{
#if LWIP_HTTPD_SSI_EX
	const char *url;
#endif//LWIP_HTTPD_SSI_E
	const char *pSSIName;
	tSSIHandler pfnSSIHandler;
} tSSI;

void httpd_set_ssi_handler(const tSSI *ssis, int num_handlers);
/* The maximum length of the string comprising the tag name */
#ifndef LWIP_HTTPD_MAX_TAG_NAME_LEN
#define LWIP_HTTPD_MAX_TAG_NAME_LEN 8
#endif

/* The maximum length of string that can be returned to replace any given tag */
#ifndef LWIP_HTTPD_MAX_TAG_INSERT_LEN
#define LWIP_HTTPD_MAX_TAG_INSERT_LEN 1024
#endif


#endif /* LWIP_HTTPD_SSI */

#if LWIP_HTTPD_CGI

/*
 * Function pointer for a CGI script handler.
 *
 * This function is called each time the HTTPD server is asked for a file
 * whose name was previously registered as a CGI function using a call to
 * http_set_cgi_handler. The iIndex parameter provides the index of the
 * CGI within the ppcURLs array passed to http_set_cgi_handler. Parameters
 * pcParam and pcValue provide access to the parameters provided along with
 * the URI. iNumParams provides a count of the entries in the pcParam and
 * pcValue arrays. Each entry in the pcParam array contains the name of a
 * parameter with the corresponding entry in the pcValue array containing the
 * value for that parameter. Note that pcParam may contain multiple elements
 * with the same name if, for example, a multi-selection list control is used
 * in the form generating the data.
 *
 * The function should return a pointer to a character string which is the
 * path and filename of the response that is to be sent to the connected
 * browser, for example "/thanks.htm" or "/response/error.ssi".
 *
 * The maximum number of parameters that will be passed to this function via
 * iNumParams is defined by LWIP_HTTPD_MAX_CGI_PARAMETERS. Any parameters in the incoming
 * HTTP request above this number will be discarded.
 *
 * Requests intended for use by this CGI mechanism must be sent using the GET
 * method (which encodes all parameters within the URI rather than in a block
 * later in the request). Attempts to use the POST method will result in the
 * request being ignored.
 * 其中的connection 为struct http_state *hs 指针
 *
 */
typedef const char *(*tCGIHandler)(void *connection,int iIndex, int iNumParams, char *pcParam[],char *pcValue[]);

typedef const char *(*RxPostDataHandler)(int offset,unsigned char *pdata, unsigned int len, unsigned int content_len_left);

/*return  the file len,if filebuf = NULL,just return file total len,if no file return -1;*/
typedef int(*GetUserFileHandler)(char * url/*IN*/,char * filebuf/*OUT*/, int fileMaxlen/*IN*/);

/*
 * Structure defining the base filename (URL) of a CGI and the associated
 * function which is to be called when that URL is requested.
 * 注册 post 的处理函数；
 * pfnCGIHandler httpd 对post命令进行解析之后调用的函数，设置的参数是post 命令里设置的参数和值，pfnCGIHandler 函数的返回值为需要发送的url 的
 * pfnPostCallBack httpd 对post命令不进行解析直接把收到的post 包传输给pfnPostCallBack 函数，pfnPostCallBack 函数的返回值为需要发送的url 的,pfnPostCallBack 一般使用在在线升级等post内容不是命令的情况
 * pfnGetFileHandler httpd 在得到需要返回的url之后，先到文件系统中查找，如果文件系统中找不到，就会将url 通过 http_set_GetFile_handlers设置的函数查找，
 * pfnCGIHandler 与pfnPostCallBack 之需要存在一个为非空即可，如果两个函数都是非空 那么pfnPostCallBack优先调用
 */
typedef struct
{
	const char *pcCGIName;
	tCGIHandler pfnCGIHandler;
	RxPostDataHandler pfnPostCallBack;
} tCGI;




void httpd_set_GetFile_handlers(const GetUserFileHandler pGF);

void httpd_set_cgi_handlers(const tCGI *pCGIs, int iNumHandlers);

/* The maximum number of parameters that the CGI handler can be sent. */
#define LWIP_HTTPD_MAX_CGI_PARAMETERS 16

#endif /* LWIP_HTTPD_CGI */



void httpd_init_socket(void);


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HTTPD_H */
