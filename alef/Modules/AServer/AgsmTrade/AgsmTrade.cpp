/******************************************************************************
Module:  AgsmTrade.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 15
******************************************************************************/

#include "ApBase.h"
#include "AgsmTrade.h"
#include "AgsdCharacter.h"
#include "AgsdTrade.h"

AgsmTrade::AgsmTrade()
{
	SetModuleName("AgsmTrade");
}

AgsmTrade::~AgsmTrade()
{
}

BOOL AgsmTrade::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pagsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");

	if (!m_pagpmCharacter || !m_pagpmItem || !m_pagsmCharacter)
		return FALSE;

	// setting module data
	// SetModuleData();

	// setting attached data(m_nKeeping) in character module
	SetAttachedModuleData(1, AGSMTRADE_AD_CHARACTER, "AgpmCharacter", 0, sizeof(AgsdCharacter));

	// setting callback functions
	SetCallback("AgsmCharacter", CHARACTER_CB_ID_MOVE, CancelTrade, this);

	// setting serialize table
	stMODULEDISP	stModuleDisp[3] = { /*{ PACKET_TYPE, PACKET_ACTION, ProcessTrade },
		{ PACKET_TYPE, PACKET_ACTION, ProcessConnectDeal },*/
		{ 0, 0, NULL } };

	SetModuleDispatch(stModuleDisp);

	// setting update functions
	// SetUpdatesFunc();

	return TRUE;
}

BOOL AgsmTrade::OnInit()
{
	return TRUE;
}

BOOL AgsmTrade::OnDestroy()
{
	return TRUE;
}

BOOL AgsmTrade::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

//		OnValid
//	Functions
//		- 넘어온 데이터가 유효한지 검사
//			1. hack check
//			2. valid action check
//	Arguments
//		- pszData : 데이타 포인터
//		- nSize : 데이타 크기
//	Return value
//		- FALSE인경우 뭔가가 잘못된것임...
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::OnValid(CHAR* pszData, INT16 nSize)
{
	/*
	if (m_csValidCheck.CheckHack())
		return FALSE;

	if (m_csValidCheck.CheckAvailAction())
		return FALSE;
	*/

	return TRUE;
}

/*
BOOL AgsmTrade::IsAvailableTrade(pstAPADMINOBJECTCXT pstObjectCxt)
{
	AgsdCharacter *pAgsdCharacter = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pstObjectCxt->pObject);

	if (pAgsdCharacter->m_fCharStatus == AGSDCHAR_FLAG_ALIVE)
		return TRUE;
	else
		return FALSE;
}
*/

//		StartTrade
//	Functions
//		- trade가 가능한지 검사한다.
//		- 가능하다면 캐릭터 상태를 거래 상태로 바꾼다.
//	Arguments
//		- pChar1 : 거래하는 캐릭터
//		- pChar2 : 거래하는 캐릭터
//	Return value
//		- TRUE : 거래 시작
//		- FALSE : 거래 시작 실패
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::StartTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2)
{
	BOOL	bRetval = FALSE;

	AgsdCharacter *pAgsdCharacter1 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar1);
	AgsdCharacter *pAgsdCharacter2 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar2);

	AgsdTrade *pAgsdTrade1 =
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar1);
	AgsdTrade *pAgsdTrade2 =
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar2);

	pChar1->m_Mutex.WLock();
	pChar2->m_Mutex.WLock();

	if (pAgsdCharacter1->m_fCharStatus == AGSDCHAR_FLAG_NORMAL &&
		pAgsdCharacter2->m_fCharStatus == AGSDCHAR_FLAG_NORMAL)
	{
		pAgsdCharacter1->m_fCharStatus = AGCDCHAR_FLAG_TRADE;
		pAgsdCharacter2->m_fCharStatus = AGCDCHAR_FLAG_TRADE;

		// 거래창 초기화
		ZeroMemory(pAgsdTrade1, sizeof(AgsdTrade));
		ZeroMemory(pAgsdTrade2, sizeof(AgsdTrade));

		bRetval = TRUE;
	}

	pChar2->m_Mutex.Release();
	pChar1->m_Mutex.Release();

	return bRetval;
}

//		UpdateTrade
//	Functions
//		- 거래창을 업데이트 한다. 
//			(여기에선 실제 캐릭터들간의 아이템 정보는 변경하지 않는다. 실제 변경은 완료 루틴에서 처리한다.)
//	Arguments
//		- pCharacter : 거래창에 뭔가를 새로 올려놓은 캐릭터
//		- ulIID : 새로 올려놓은 아이템 (돈인경우 이값은 무시한다)
//		- nPosInventory : 올려놓은 아이템의 인벤토리 위치 (거래창 인벤토리 위치)
//		- ulMoney : 돈을 올려놓은 경우 돈 액수
//	Return value
//		- TRUE : 거래창 업데이트 성공
//		- FALSE : 거래창 업데이트 실패
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::UpdateTrade(AgpdCharacter *pCharacter, UINT32 ulIID, INT16 nPosInventory, UINT32 ulMoney)
{
	AgsdCharacter	*pAgsdCharacter = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pCharacter);

	// 현재 거래 상태인지 검사
	pCharacter->m_Mutex.RLock();
	if (pAgsdCharacter->m_fCharStatus != AGCDCHAR_FLAG_TRADE)
	{
		pCharacter->m_Mutex.Release();
		return FALSE;
	}
	pCharacter->m_Mutex.Release();

	// 거래 인벤토리 정보를 가져온다.
	AgsdTrade		*pAgsdTrade = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pCharacter);

	if (ulIID != AGPMITEM_INVALID_IID)	// 아이템인지 아님 던인지 구분
	{
		// 아이템 템플릿 데이타를 가져온다. (아이템 크기를 구하기 위해)
		AgpdItemTemplate	*pItemTemplate;

		pItemTemplate = m_pagpmItem->GetItemTemplateUseIID(ulIID);

		if (CheckInventoryPos(pAgsdTrade, nPosInventory, pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH], pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT]))
		{
			// 그 위치에 아이템이 들어갈 수 있다.
			AddInventory(pCharacter, pAgsdTrade, ulIID, nPosInventory, pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH], pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT]);
		}
		else
		{
			// 아이템이 들어갈 수 없는 위치다.
			return FALSE;
		}
	}
	else	// 변경된게 돈이라면
	{
		pCharacter->m_Mutex.WLock();

		pAgsdTrade->ulMoney = ulMoney;

		pCharacter->m_Mutex.Release();
	}

	return TRUE;
}

//		CheckInventoryPos
//	Functions
//		- 거래창에서 특정 위치에 특정 크기의 아이템이 올라갈 수 있는지 검사
//	Arguments
//		- pAgsdTrade : 거래창 데이타
//		- nPosInventory : 올려놓은 아이템의 인벤토리 위치 (거래창 인벤토리 위치)
//		- pSize : 아이템 크기 (2차원 배열 포인터)
//	Return value
//		- TRUE : 올라갈 수 있다.
//		- FALSE : 없당 배째랑
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::CheckInventoryPos(AgsdTrade *pAgsdTrade, INT16 nPosInventory, INT16 nWidth, INT16 nHeight)
{
	INT16	nRow;
	INT16	nColumn;

	nColumn = nPosInventory / AGSMTRADE_MAX_INVENTORY_ROW;
	nRow = nPosInventory - (nColumn * AGSMTRADE_MAX_INVENTORY_ROW);

	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			if (pAgsdTrade->ulTradeInventory[nColumn + i][nRow + j] != AGPMITEM_INVALID_IID)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//		AddInventory
//	Functions
//		- 거래창에 아이템을 올려 놓는다. (거래창 배열에 아이템 아뒤를 아이템 크기만큼 그자리에 채워넣는다)
//	Arguments
//		- pCharacter : 아템 객체
//		- pAgsdTrade : 거래창 데이타
//		- ulIID : 아이템 아뒤
//		- nPosInventory : 올려놓은 아이템의 인벤토리 위치 (거래창 인벤토리 위치)
//		- pSize : 아이템 크기 (2차원 배열 포인터)
//	Return value
//		- TRUE : 잘 올렸다.
//		- FALSE : 어째 잘 안올라간다. 배째~
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::AddInventory(AgpdCharacter *pCharacter, AgsdTrade *pAgsdTrade, 
							 UINT32 ulIID, INT16 nPosInventory, INT16 nWidth, INT16 nHeight)
{
	INT16	nRow;
	INT16	nColumn;

	nColumn = nPosInventory / AGSMTRADE_MAX_INVENTORY_ROW;
	nRow = nPosInventory - (nColumn * AGSMTRADE_MAX_INVENTORY_ROW);

	pCharacter->m_Mutex.WLock();

	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			pAgsdTrade->ulTradeInventory[nColumn + i][nRow + j] = ulIID;
		}
	}

	pAgsdTrade->nNumItem++;

	pCharacter->m_Mutex.Release();

	return TRUE;
}

//		CompleteTrade
//	Functions
//		- 거래를 완료한다.
//			(실제 아이템 이동이 이때 이뤄진다.)
//	Arguments
//		- pChar1 : 거래하는 캐릭터
//		- pChar2 : 거래하는 캐릭터
//	Return value
//		- TRUE : 오케이 성공.
//		- FALSE : 음냐 실패당
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::CompleteTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2)
{
	AgsdCharacter *pAgsdChar1 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar1);
	AgsdCharacter *pAgsdChar2 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar2);

	AgsdTrade *pAgsdTrade1 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar1);
	AgsdTrade *pAgsdTrade2 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar2);


	// 두 캐릭터 모두 잠근다. 쓰기 모드로 꽉 잠근다.
	pChar1->m_Mutex.WLock();
	pChar2->m_Mutex.WLock();

	// 거래창에 있는 아템들을 찾는다. 찾아서 주인을 바꾼다.
	// 캐릭터에 붙어 있는 소유 아이템 리스트를 변경한다. (아이템 모듈에서 관리한당)

	for (int i = 0; i < pAgsdTrade1->nNumItem; i++)
	{
		m_pagpmItem->ChangeItemOwner(pChar1, pChar2, pChar1->m_stCharacter.m_lID, pAgsdTrade1->ulItem[i]);
	}

	for (i = 0; i < pAgsdTrade2->nNumItem; i++)
	{
		m_pagpmItem->ChangeItemOwner(pChar2, pChar1, pChar1->m_stCharacter.m_lID, pAgsdTrade2->ulItem[i]);
	}

	// 돈 거래도 있다면 처리한다.


	// 상태를 바꾼다. 보통 상태로 바꾼당
	pAgsdChar1->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;
	pAgsdChar2->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;

	// 캐릭터 잠근거 푼다.
	pChar2->m_Mutex.Release();
	pChar1->m_Mutex.Release();

	return TRUE;
}

//		CancelTrade
//	Functions
//		- 거래를 취소한다.
//			(실제 아이템 이동은 이루어 진적이 없기땜시 걍 거래 정보들만 초기화시킨다.)
//	Arguments
//		- pChar1 : 거래하는 캐릭터
//		- pChar2 : 거래하는 캐릭터
//	Return value
//		- TRUE : 오케이 성공.
//		- FALSE : 음냐 실패당
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::CancelTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2)
{
	// 두 캐릭터 모두 잠근다. 쓰기 모드로 잠그고
	// 상태, 거래창 데이타 등을 초기화한다.

	AgsdCharacter *pAgsdChar1 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar1);
	AgsdCharacter *pAgsdChar2 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar2);

	AgsdTrade *pAgsdTrade1 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar1);
	AgsdTrade *pAgsdTrade2 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar2);

	pChar1->m_Mutex.WLock();
	pChar2->m_Mutex.WLock();

	// 상태 초기화
	pAgsdChar1->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;

	// 거래 데이타 초기화
	pAgsdTrade1->nNumItem = 0;
	pAgsdTrade1->ulMoney = 0;
	//ZeroMemory(pAgsdTrade1->ulItem, sizeof(UINT32) * AGSMTRADE_MAX_ITEM);
	pAgsdTrade1->ulItem.MemSetAll();
	ZeroMemory(pAgsdTrade1->ulTradeInventory, sizeof(UINT32) * AGSMTRADE_MAX_INVENTORY_COLUMN * AGSMTRADE_MAX_INVENTORY_ROW);

	// 상태 초기화
	pAgsdChar2->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;

	// 거래 초기화
	pAgsdTrade2->nNumItem = 0;
	pAgsdTrade2->ulMoney = 0;
	//ZeroMemory(pAgsdTrade2->ulItem, sizeof(UINT32) * AGSMTRADE_MAX_ITEM);
	pAgsdTrade2->ulItem.MemSetAll();
	ZeroMemory(pAgsdTrade2->ulTradeInventory, sizeof(UINT32) * AGSMTRADE_MAX_INVENTORY_COLUMN * AGSMTRADE_MAX_INVENTORY_ROW);

	pChar2->m_Mutex.Release();
	pChar1->m_Mutex.Release();

	// 아무일 없다는 듯이 리턴한다.

	return TRUE;
}

//		ProcessTrade
//	Functions
//		- item(money) trade를 처리한다.
//	Arguments
//		- ulType : packet type
//		- ulAction : action number
//		- pszData : data pointer
//		- nSize : data size
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmTrade::ProcessTrade(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize)
{
	INT16				nProcessStep;	// 거래 진행 단계 저장
										// 1. 거래 시작 (AGSMTRADE_TRADE_START)
										// 2. 아템 변경 (AGSMTRADE_INVENTORY_UPDATE)
										// 3. 거래 완료 (AGSMTRADE_TRADE_COMPLETE)
										// 4. 거래 취소 (AGSMTRADE_TRADE_CANCEL)

	UINT32				ulCID1, ulCID2;

	AgpdCharacter		*pTrader1, *pTrader2;

	// 패킷을 분석한다.
	// ulCID1 = ;
	// ulCID2 = ;
	// nProcessStep = ;

	// 거래자 정보를 가져온다.
	pTrader1 = m_pagpmCharacter->GetCharacter(ulCID1);
	pTrader2 = m_pagpmCharacter->GetCharacter(ulCID2);

	BOOL bRetval;

	switch (nProcessStep) {
	case AGSMTRADE_TRADE_START:
		bRetval = StartTrade(pTrader1, pTrader2);

		// 결과 전송
		// 거래창을 연다.

		break;

	case AGSMTRADE_INVENTORY_UPDATE:
		UINT32	ulIID;
		INT16	nPosInventory;
		UINT32	ulMoney;

		// 실제 아이템 데이타는 변한게 없고 단지 클라이언트에서 보이는것만 거래창에 올려놓은건 아이템 인벤에서 안보이게 한다.
		bRetval = UpdateTrade(pTrader1, ulIID, nPosInventory, ulMoney);

		// 결과 전송

		break;

	case AGSMTRADE_TRADE_COMPLETE:
		bRetval = CompleteTrade(pTrader1, pTrader2);

		// 필요한 모듈들(아템 서버등)에 결과를 전송한다.

		// 결과 전송

		break;

	case AGSMTRADE_TRADE_CANCEL:
		bRetval = CancelTrade(pTrader1, pTrader2);
		
		// 결과 전송

		break;

	default:
		break;
	}

	return 0;
}

//		ProcessConnectDeal
//	Functions
//		- deal 서버로 연결시켜 준다.
//	Arguments
//		- ulType : packet type
//		- ulAction : action number
//		- pszData : data pointer
//		- nSize : data size
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmTrade::ProcessConnectDeal(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize)
{
	return 0;
}

//		CancelTrade
//	Functions
//		- 진행중인 거래를 중지시킨다. (캐릭터가 움직였다던가 하는 경우)
//	Arguments
//		- pData : character data pointer (pstAPADMINOBJECTCXT)
//		- pClass : this class pointer
//	Return value
//		- void
///////////////////////////////////////////////////////////////////////////////
VOID AgsmTrade::CancelTrade(PVOID pData, PVOID pClass)
{
	AgpdCharacter *pCharacter = (AgpdCharacter *) pData;



	return;
}