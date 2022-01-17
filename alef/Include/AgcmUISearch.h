// -----------------------------------------------------------------------------
//                                _    _ _____  _____                      _         _     
//     /\                        | |  | |_   _|/ ____|                    | |       | |    
//    /  \    __ _  ___ _ __ ___ | |  | | | | | (___   ___  __ _ _ __  ___| |__     | |__  
//   / /\ \  / _` |/ __| '_ ` _ \| |  | | | |  \___ \ / _ \/ _` | '__|/ __| '_ \    | '_ \ 
//  / ____ \| (_| | (__| | | | | | |__| |_| |_ ____) |  __/ (_| | |  | (__| | | | _ | | | |
// /_/    \_\\__, |\___|_| |_| |_|\____/|_____|_____/ \___|\__,_|_|   \___|_| |_|(_)|_| |_|
//            __/ |                                                                        
//           |___/                                                                         
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 06/28/2006 by Nonstopdj
//
// Copyright 2006, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#pragma once

#include "AgcModule.h"
#include "AgcmUIMain.h"
#include "AgcmUIManager2.h"
#include "AgpmChannel.h"
#include "AgpmChatting.h"
#include "AgcmChatting2.h"
#include "AgcmUIChatting2.h"
#include "AgpmSearch.h"

// -----------------------------------------------------------------------------

//***************************************************************************************************
//
// Class Name 		: AgcmUIChannel : public AgcModule
//
// Last Modified 	: Nonstopdj		2006. 06. 28
// First Modified 	: Nonstopdj		2006. 06. 28
//
// Function			: 캐릭터 조건 검색 관련.
//
//***************************************************************************************************
class	AgcmUISearch : public AgcModule
{
private:
	// Data members
	AgcmUISearch*		m_pcsAgcmUISearch;
	AgcmUIManager2*		m_pcsAgcmUIManager2;
	ApmMap*				m_pcsApmMap;
	AgpmSearch*			m_pcsAgpmSearch;

	AgcdUIUserData*		m_pstUDSearchResultList;		//. Search Result List User Data.
	INT32				m_lEventSearchResultUIOpen;		//. Event ID
	INT32				m_lEventSearchResultUIClose;
	INT32				m_ICurrentResultSelectIndex;	//. 결과창에 현재 선택된 리스트의 Index
	INT32				m_lMsgBoxSearchFailed;			//. 검색실패 메세지 박스.

	vector<AgpdSearch>	m_SearchResultList;			//. 단순 Display용이므로 그냥 cm에 넣음.
	map<string, INT32>	m_RaceClassNameMap;
	map<INT32,	string>	m_ClassNameMap;

	ApSafeArray<INT32, AGPMSEARCH_MAX_COUNT>	m_arSearchResult;	//. List User Data Array;
	
public:
	// Construction/Destruction
	AgcmUISearch();
	virtual				~AgcmUISearch();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL AddEvent();
	//. BOOL AddBoolean();
	BOOL AddFunction();
	BOOL AddUserData();
	BOOL AddDisplay();

	void	ClearTransferData(AgpdSearch&	psdSearch)
	{
		memset(psdSearch.m_szName, 0, sizeof(CHAR) *AGPDCHARACTER_NAME_LENGTH);
		psdSearch.m_lTID			= 0;
		psdSearch.m_lMinLevel	= 0;
		psdSearch.m_lMaxLevel	= 0;
		psdSearch.m_stPos.x = 0;
		psdSearch.m_stPos.y = 0;
		psdSearch.m_stPos.z = 0;
	}

	void	SearchListClear()
	{
		m_SearchResultList.clear();
	}


	BOOL	RefreshSearchResultList();
	BOOL	SendSearchCondition(eAgpmSearchPacketType etype, AgpdSearch& pstSearch);	//. send packet.
	INT32	FindClassName(TCHAR* szClassName);											//. return TID

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. callback functions
	static BOOL CBSearchResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchFail(PVOID pData, PVOID pClass, PVOID pCustData);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. display
	static BOOL CBSearchResultList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. functions
	static BOOL	CBSearchReultListSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//. clear function
	//. static BOOL CBCreateRoomClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	//. normal function
	//. static BOOL	CBCreateRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//. script
	void	AddSearchResultList();
	void	OpenSearchResultUI();
	void	CloseSearchResultUI();
};


