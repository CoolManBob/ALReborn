/******************************************************************************
Module:  AgsmRecruitClient.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#ifndef __AGSM_RECRUIT_CLIENT_H__
#define __AGSM_RECRUIT_CLIENT_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgsEngine.h"
#include "AgpmRecruit.h"
#include "AgsmRecruitServer.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmRecruitClientD" )
#else
#pragma comment ( lib , "AgsmRecruitClient" )
#endif
#endif

class CLoginCharInfo
{
public:
	UINT32			m_ulNID;
	char			*m_pstrCharName;

	CLoginCharInfo()
	{
		m_pstrCharName = NULL;
	}
	~CLoginCharInfo()
	{
		if( m_pstrCharName != NULL )
		{
			delete [] m_pstrCharName;
			m_pstrCharName = NULL;
		}
	}	
};

class AgsmRecruitClient : public AgsModule
{
	AuPacket			m_csPacket;   //Recruit 패킷.
	AuPacket			m_csLFPInfo;  //Looking for Party
	AuPacket			m_csLFMInfo;  //Looking for Member
	AuPacket			m_csSearchInfo; //찾기를 할때 사용되는 패킷.

	AuOLEDBManager		*m_paAuOLEDBManager;
	AgsmRecruitServer	*m_pagsmRecruitServer;

	ApAdmin				m_csCIDManager;

public:

	AgsmRecruitClient();
	~AgsmRecruitClient();

	static BOOL	CBRegisterLFP(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBCancelLFP(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSearchLFP(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRegisterLFM(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBCancelLFM(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSearchLFM(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResult(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL ProcessAddToRecruitServer( INT32 lPlayerID, UINT32 ulNID );
	BOOL PreProcessRegisterLFP( INT32 lControlServerID, INT32 lPlayerID, void *pvLFPInfo );
	BOOL PreProcessRegisterLFM( INT32 lControlServerID, INT32 lPlayerID, void *pvLFMInfo );

	UINT32 GetulNID( INT32 lCID );

	BOOL SendRegisterLFP( INT32 lPlayerID, INT32 lResult );
	BOOL SendCancelLFP( INT32 lPlayerID, INT32 lResult );
	BOOL SendLFPInfo( INT32 lPlayerID, INT32 lTotalCount, INT32 lIndex, INT32 lLevel, INT32 lClass, char *pstrLFPID, INT32 lLFPID );
	BOOL SendLFMInfo( INT32 lPlayerID, INT32 lTotalCount, INT32 lIndex, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lLevel, INT32 lClass, char *pstrCharName, INT32 lLeaderID );
	BOOL SendResult( INT32 lPlayerID, INT32 lResult );
	BOOL SendResult( UINT32 ulNID, INT32 lResult );

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnDisconnect(INT32 lAccountID);
};

#endif