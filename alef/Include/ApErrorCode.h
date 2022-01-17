/******************************************************************************
Module:  ApErrorCode.h
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2002. 04. 11
******************************************************************************/

#if !defined(__APERRORCODE_H__)
#define __APERRORCODE_H__

#define AP_NO_ERROR			1

static BOOL ApMemAllocTest(PVOID pAddr)
{
	if (pAddr == NULL)
	{
		// memory alloction error. log error...
		return FALSE;
	}
	else
		return TRUE;
}

#endif // __APERRORCODE_H__