/******************************************************************************
Module:  AuIPBlockingManager.h
Notices: Copyright (c) NHN Studio 2004 Ashulam
Purpose: 
Last Update: 2004. 1. 18
******************************************************************************/

#pragma once

#include "ApAdmin.h"
#include "AuExcelTxtLib.h"

#define AUIPBLOCKING_MAX_IP_COUNT			10000

class AuIPBlockingData
{
public:
	unsigned long			m_lMinIP;
	unsigned long			m_lMaxIP;

	AuIPBlockingData()
	{
		m_lMinIP = 0;
		m_lMaxIP = 0;		
	}
};

class AuIPBlockingManager
{
	ApAdmin				m_csBlockIP;
	ApAdmin				m_csPCRoomIP;

	AuExcelTxtLib		m_csExcelTxtLib;
public:

	AuIPBlockingManager();
	~AuIPBlockingManager();

	BOOL AddIP( ApAdmin *pcsApAdmin, char *pstrIP, unsigned long lMin, unsigned long lMax );
	BOOL SearchIPFromBlock( char *pstrIP );
	BOOL SearchIPFromPCRoom( char *pstrIP );
	BOOL LoadBlockIPFromFile( char *pstrFileName );
	BOOL LoadPCRoomIPFromFile( char *pstrFileName );
};
