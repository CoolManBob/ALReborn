/******************************************************************************
Module:  AgsmRecruitServer.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#ifndef __AGSM_RECRUIT_SERVER_H__
#define __AGSM_RECRUIT_SERVER_H__

#include "ApBase.h"
#include "LinkedList.h"
#include "AgsEngine.h"
#include "AuOLEDB.h"
#include "AgpmRecruit.h"
#include "AgpmItem.h"
#include "AgsmServerManager2.h"

#define UNICODE
#define _UNICODE

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmRecruitServerD" )
#else
#pragma comment ( lib , "AgsmRecruitServer" )
#endif
#endif

enum eAgsmRecruitOperation
{
	AGSM_RECRUIT_OP_INSERT_LFP				= 1,
	AGSM_RECRUIT_OP_UPDATE_LFP,
	AGSM_RECRUIT_OP_DELETE_LFP,
	AGSM_RECRUIT_OP_SEARCH_LFP,
	AGSM_RECRUIT_OP_INSERT_LFM,
	AGSM_RECRUIT_OP_UPDATE_LFM,
	AGSM_RECRUIT_OP_DELETE_LFM,
	AGSM_RECRUIT_OP_SEARCH_LFM
};

enum eAgsmRecruitServerCB_ID
{
	RECRUIT_CB_ID_REGISTER_LFP					= 0,	//Recruit DB에 나도 등록해줘~~ ㅠ.ㅠ
	RECRUIT_CB_ID_CANCEL_LFP,							//Recruit DB에서 빼줘~
	RECRUIT_CB_ID_SEARCH_LFP,							//Recruit DB에서 Search해줘~
	RECRUIT_CB_ID_REGISTER_LFM,							//Recruit DB에 나도 등록해줘~~ ㅠ.ㅠ
	RECRUIT_CB_ID_CANCEL_LFM,							//Recruit DB에서 빼줘~
	RECRUIT_CB_ID_SEARCH_LFM,							//Recruit DB에서 Search해줘~
	RECRUIT_CB_ID_RESULT								
};

class AgsmRecruitQuery
{
public:
	char			*m_pstrQuery;
	INT32			m_lLevel;
	INT32			m_lPage;

	AgsmRecruitQuery()
	{
		m_pstrQuery = NULL;
	}
	~AgsmRecruitQuery()
	{
		if( m_pstrQuery != NULL )
		{
			delete [] m_pstrQuery;
			m_pstrQuery = NULL;
		}
	}
};

//리크루트 큐에 넣는 정보.
class AgsmRecruitQueueInfo
{
public:
	CDList<AgsmRecruitQuery *>	*m_pcQueryList;

	INT8			m_nOperation;
	bool			m_bSearchByLevel;

	INT32			m_lCID;
	char			*m_pstrCharName;
	char			*m_pstrPurpose;
	INT32			m_lRequireMember;
	INT32			m_lLV;
	INT32			m_lMinLV;
	INT32			m_lMaxLV;
	INT32			m_lClass;
	INT32			m_lLeaderID;

	INT32			m_lErrorCode;
	INT32			m_lPage;

	AgsmRecruitQueueInfo()
	{
		m_bSearchByLevel = false;
		m_nOperation = 0;
		m_lCID = 0;
		m_lRequireMember = 0;
		m_lLV = 0;
		m_lMinLV = 0;
		m_lMaxLV = 0;
		m_lClass = 0;
		m_lLeaderID = 0;
		m_lErrorCode = 0;
		m_lPage = 0;

		m_pstrCharName = NULL;
		m_pstrPurpose = NULL;

		m_pcQueryList = NULL;
	}
	~AgsmRecruitQueueInfo()
	{
		if( m_pcQueryList != NULL )
		{
			delete m_pcQueryList;
			m_pcQueryList = NULL;
		}
		if( m_pstrCharName != NULL )
		{
			delete m_pstrCharName;
			m_pstrCharName = NULL;
		}
		if( m_pstrPurpose != NULL ) 
		{
			delete m_pstrPurpose;
			m_pstrPurpose = NULL;
		}
	}
};

class AgsmRecruitSearchInfo
{
public:
	INT32			m_lTotalCount;
	INT32			m_lIndex;
	char			*m_pstrPurpose;
	INT32			m_lRequireMember;
	INT32			m_lMinLV;
	INT32			m_lMaxLV;
	INT32			m_lLevel;
	INT32			m_lClass;
	char			*m_pstrCharName;
	INT32			m_lLeaderID;

	AgsmRecruitSearchInfo()
	{
		m_lTotalCount = 0;
		m_lIndex = 0;
		m_lRequireMember = 0;
		m_lMinLV = 0;
		m_lMaxLV = 0;
		m_lLevel = 0;
		m_lClass = 0;
		m_lLeaderID = 0;

		m_pstrPurpose = NULL;
		m_pstrCharName = NULL;
	}
	~AgsmRecruitSearchInfo()
	{
		if( m_pstrPurpose != NULL )
		{
			delete [] m_pstrPurpose;
			m_pstrPurpose = NULL;
		}

		if( m_pstrCharName != NULL )
		{
			delete [] m_pstrCharName;
			m_pstrCharName = NULL;
		}
	}
};

class AgsmRecruitServer : public AgsModule
{
	AuPacket		m_csPacket;			//Recruit 패킷.
	AuPacket		m_csLFPInfo;		//DetailInfo 패킷.
	AuPacket		m_csLFMInfo;		//DetailInfo 패킷.
	AuPacket		m_csSearchInfo;		//SearchInfo

	AuOLEDBManager		*m_paAuOLEDBManager;
	AgsmServerManager	*m_pagsmServerManager;

	ApAdmin			m_csRegisterCID; //등록된 CID

public:

	AgsmRecruitServer();
	~AgsmRecruitServer();

	BOOL PreProcessRegisterLFP( INT32 lControlServerID, INT32 lLFPID );
	BOOL PreProcessRegisterLFM( INT32 lControlServerID, INT32 lLFPID, void *pvLFMInfo );

	BOOL ProcessRegisterLFP( INT32 lLFPID, void *pvLFPInfo, BOOL bCheckQueue = TRUE );
	BOOL ProcessRegisterLFM( INT32 lLFMID, void *pvLFMInfo, BOOL bCheckQueue = TRUE );
	BOOL ProcessUpdateLFP( INT32 lLFPID, void *pvDetailInfo, BOOL bCheckQueue = TRUE );
	BOOL ProcessCancelLFP( INT32 lLFPID, void *pvLFPInfo, BOOL bCheckQueue = TRUE );
	BOOL ProcessCancelLFM( INT32 lLFMID, void *pvLFMInfo, BOOL bCheckQueue = TRUE );
	BOOL ProcessSearchFromLFP( INT32 lLFPID, void *pvSearchInfo, BOOL bCheckQueue = TRUE );
	BOOL ProcessSearchFromLFM( INT32 lLFMID, void *pvSearchInfo, BOOL bCheckQueue = TRUE );

	//GameServer에서 등록해달라고 한다.
	BOOL SendRegisterLFP( INT32 lControlServerID, INT32 lLFPID );
	BOOL SendRegisterLFM( INT32 lControlServerID, INT32 lLFPID, void *pvLFMInfo );

	//등록 콜백~
	BOOL SetCallbackRegisterLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCancelLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackRegisterLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCancelLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
};

#endif
