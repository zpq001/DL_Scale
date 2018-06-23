#ifndef __CMDPROCESS_H
#define __CMDPROCESS_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
     
typedef void(*PtrCmdFunProc)(char *str,unsigned char strlenth);

typedef struct _CmdStcu
{
	char *cmdstr;
    unsigned char cmdstrlenth;
	PtrCmdFunProc cmdproc;

}CmdStruct;
//
// 
typedef struct _ServerFram
{
    char headstr[2];
    char datalen;
    char addr;
    char cmdchar;
    char paramters[4];  // max numer
    char checksum;
}CmdFramStruct;

extern CmdStruct SetCmdArry[];
void SetCmdProcess(char *recstr,CmdStruct *Ptrcmdstruct);

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */
