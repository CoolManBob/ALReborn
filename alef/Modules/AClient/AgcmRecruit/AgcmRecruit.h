#ifndef __AGCM_RECRUIT_H__
#define __AGCM_RECRUIT_H__

#include "AgcModule.h"
#include "AgpmRecruit.h"
#include "ApBase.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgcmCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgcmConnectManager.h"
#include "AcUIRecruit.h"
#include "AcUIRecruitWrite.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmRecruitD" )
#else
#pragma comment ( lib , "AgcmRecruit" )
#endif
#endif

class AgcmRecruit : public AgcModule
{
	INT32				m_lControlServerID;
	INT16				m_nControlServerNID;
	INT16				m_nRecruitServerNID;

	ApmMap				*m_papmMap;
	AgpmCharacter		*m_pagpmCharacter;
	AgcmCharacter		*m_pagcmCharacter;
	AgpmFactors			*m_pagpmFactors;
	AgpmItem			*m_pagpmItem;
	AgcmConnectManager	*m_pagcmConnectManager;

	AuPacket			m_csPacket;   //Recruit 패킷.
	AuPacket			m_csLFPInfo;  //Looking for Party
	AuPacket			m_csLFMInfo;  //Looking for Member
	AuPacket			m_csSearchInfo; //찾기를 할때 사용되는 패킷.

	//AcUIBbs				m_clBbs;
	BOOL				m_bBBSWindowShow;
	bool				m_bSearchByMyLevel;			// 찾기를 누를때 마다 갱신된다 - 이전 검색 조건 저장
	INT32				m_lSelectedClass;			// 찾기를 누를때 마다 갱신된다 - 이전 검색 조건 저장
	
public:

	AgcmRecruit();
	~AgcmRecruit();

	AcUIRecruit			m_clRecruitWindow			;
	AcUIRecruitWrite	m_clRecruitWriteWindow		;
	
	INT32 GetControlServerID();
	INT16 GetControlServerNID();
	INT16 GetRecruitServerNID();

	void SetControlServerID( INT32 nControlServerID );
	void SetControlServerNID( INT16 nControlServerNID );
	void SetRecruitServerNID( INT16 nRecruitServerNID );

	BOOL SendAddToRecruitServer( INT32 lCID );
	BOOL SendRegisterToLFP( INT32 lCID, char *pstrCharName );
	BOOL SendRegisterToLFM( INT32 lCID, char *pstrCharName, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lClass );
	BOOL SendCancelFromLFP( INT32 lCID, char *pstrCharName );
	BOOL SendCancelFromLFM( INT32 lCID, char *pstrCharName );
	BOOL SendSearchLFPByPage( INT32 lCID, bool bSearchByMyLevel, INT32 lPage, INT32 lClass );
	BOOL SendSearchLFMByPage( INT32 lCID, bool bSearchByMyLevel, INT32 lPage, INT32 lClass );

	BOOL ProcessSearchLFPResult( void *pvPlayerInfo );
	BOOL ProcessSearchLFMResult( void *pvPlayerInfo );

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	//2002_03_14 For BBS
	BOOL	OnInit();
	void	ShowBbsWindow( BOOL bShow );

	static
	BOOL	CBBbsClickContent( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );
	static
	BOOL	CBBbsChangePage( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );
	static
	BOOL	CBBbsWindowClose( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );

	
public:		// BBS Manage function
	void	ClickMercenaryBbsContent( UINT32 lIndex, UINT8 cRow, UINT8 cColumn );
	void	ClickPartyBbsContent( UINT32 lIndex, UINT8 cRow, UINT8 cColumn );

	// UI관련 Function
	void	UIClickRegisterLFP()	;		// LFP 등록 요청
	void	UIClickDeleteLFP()		;		// LFP 삭제 요청
	void	UIClickRegisterLFM( char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lClass );	// LFM 등록 요청 
	void	UIClickDeleteLFM()		;
	void	UILFPSearch( bool bSearchByMyLevel, INT32 lPage, INT32 lClass); 
	void	UILFMSearch( bool bSearchByMyLevel, INT32 lPage, INT32 lClass); 
	void	UILFPPageChange( INT32 lPage );
	void	UILFMPageChange( INT32 lPage );
};

#endif