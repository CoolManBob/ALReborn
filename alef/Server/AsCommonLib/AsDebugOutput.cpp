/******************************************************************************
Module:  AsDebugOutput.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 25
******************************************************************************/

#include "AsDebugOutput.h"
#include <stdio.h>

int		g_nOutputType = AS_LOG_CONSOLE;
FILE	*g_pFile = NULL;

BOOL OpenLog(int nOutputType)
{
	g_nOutputType = nOutputType;

	if (g_nOutputType & AS_LOG_FILE)
	{
		g_pFile = fopen("LOG_SERVER.TXT", "w");

		if (g_pFile == NULL)
		{
			g_nOutputType = AS_LOG_CONSOLE;
		}
	}

	return TRUE;
}

BOOL CloseLog()
{
	if (g_nOutputType & AS_LOG_FILE)
		fclose(g_pFile);

	return TRUE;
}

BOOL WriteLog(int nLevel, char *szText)
{
	if (nLevel == AS_LOG_DEBUG)
	{
		BOOL bIsDebug = FALSE;

#ifdef	_DEBUG
		bIsDebug = TRUE;
#endif	_DEBUG

		if (!bIsDebug)
			return FALSE;
	}

	if (g_nOutputType & AS_LOG_FILE)
	{
		fprintf(g_pFile, "%s\n", szText);
		fflush(g_pFile);
	}

	if (g_nOutputType & AS_LOG_CONSOLE)
	{
		/*
		fprintf(stdout, "%s\n", szText);
		fflush(stdout);
		*/

		printf("%s\n", szText);
	}

	return TRUE;
}
