/******************************************************************************
Module:  AgsmIPBlockingManager.h
Notices: Copyright (c) NHN Studio 2004 Ashulam
Purpose: 
Last Update: 2004. 1. 18
******************************************************************************/

#ifndef __AGSM_IPBLOCKINGMANAGER_H__
#define __AGSM_IPBLOCKINGMANAGER_H__

#include "ApAdmin.h"
#include "AuExcelTxtLib.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmIPBlockingManagerD" )
#else
#pragma comment ( lib , "AgsmIPBlockingManager" )
#endif
#endif

#define AGSMIPBLOCKING_MAX_IP_COUNT			10000

class AgsmIPBlockingData
{
public:
	unsigned long			m_lMinIP;
	unsigned long			m_lMaxIP;

	AgsmIPBlockingData()
	{
		m_lMinIP = 0;
		m_lMaxIP = 0;		
	}
};

class AgsmIPBlockingManager
{
	ApAdmin				m_csBlockIP;
	ApAdmin				m_csPCRoomIP;

	AuExcelTxtLib		m_csExcelTxtLib;
public:

	AgsmIPBlockingManager();
	~AgsmIPBlockingManager();

	BOOL AddIP( ApAdmin *pcsApAdmin, char *pstrIP, unsigned long lMin, unsigned long lMax );
	BOOL SearchIPFromBlock( char *pstrIP );
	BOOL SearchIPFromPCRoom( char *pstrIP );
	BOOL LoadBlockIPFromFile( char *pstrFileName );
	BOOL LoadPCRoomIPFromFile( char *pstrFileName );
};

#endif