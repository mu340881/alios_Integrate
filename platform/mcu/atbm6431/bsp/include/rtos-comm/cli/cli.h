#ifndef __CLI_H__
#define __CLI_H__


///#include "cmd_line.h"
struct cmd_struct{

	const char *cmd;
	int (*fn)(const char *argv);
	struct cmd_struct *next;
};

/***********************************************************************/
/***                        Data Types and Constants                 ***/
/***********************************************************************/




typedef struct CLI_COMMAND_DEF_S
{
    const char *    Name;
    const char *    HelpLine;
    /* See CmdLine_ReadMemory(), CmdLine_ReadMemory() and CmdLine_Dump() for example.
    */
    void            (*Handler)(char *Args);
}CLI_COMMAND_DEF;

struct cli_cmd_struct{

	char *cmd;
	void (*fn)(char *args);
	struct cli_cmd_struct *next;
};

extern NORELOC void cli_add_cmd(struct cli_cmd_struct *cmd);
extern NORELOC void cli_add_cmds(struct cli_cmd_struct *cmds, int len);

/***********************************************************************/
/***                        Public Functions                         ***/
/***********************************************************************/

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
NORELOC char * CmdLine_GetToken(char ** pLine);


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
NORELOC int CmdLine_GetHex(char ** pLine, uint32 *pDword);

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
NORELOC int CmdLine_GetInteger(char ** pLine, uint32 *pDword);
NORELOC void CmdLine_ProcessingInput(void);



extern void cli_add_cmd(struct cli_cmd_struct *cmd);
extern void cli_add_cmds(struct cli_cmd_struct *cmds, int len);

extern void cli_main(void);

#endif /* __CLI_H__ */
