/******************************************************************************
Module:  AsDebugOutput.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 25
******************************************************************************/

#if !defined(__ASDEBUGOUTPUT_H__)
#define __ASDEBUGOUTPUT_H__

#include "ApBase.h"

typedef enum _eAsDebugOut {
	AS_LOG_FILE		= 0x01,
	AS_LOG_CONSOLE	= 0x02
} eAsDebugOut;

typedef enum _eAsDebugLevel {
	AS_LOG_DEBUG,
	AS_LOG_RELEASE
} eAsDebugLevel;

BOOL OpenLog(int nOutputType);
BOOL CloseLog();

//BOOL WriteLog(int nLevel, char *szText);


#endif //__ASDEBUGOUTPUT_H__