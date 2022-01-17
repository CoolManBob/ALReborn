/******************************************************************************
Module:  AgpmItem.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 3
******************************************************************************/

#if !defined(__AGSMPRIVATETRADE_H__)
#define __AGSMPRIVATETRADE_H__

#include "ApModule.h"
#include "ApBase.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgsmItem.h"
#include "AgpmLog.h"
#include "AgsmCharacter.h"
#include "AgpmGrid.h"

//PrivateTrade는 4미터 안에서만 거래가 가능하다.
#define	AGPMPRIVATETRADE_MAX_USE_RANGE		400

typedef enum
{
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT = 0x00,			 //거래 결과를 받는경우(성공,실패,Error등...)
	AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_TRADE,			 //거래 요청.
	AGPMPRIVATETRADE_PACKET_TRADE_WAIT_CONFIRM,				 //거래자의 confirm을 기다림.		
	AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_CONFIRM,			 //confirm요청.
	AGPMPRIVATETRADE_PACKET_TRADE_CONFIRM,					 //confirm했음.
	AGPMPRIVATETRADE_PACKET_TRADE_CANCEL,					 //취소.
	AGPMPRIVATETRADE_PACKET_TRADE_START,					 //거래 시작.

	AGPMPRIVATETRADE_PACKET_TRADE_EXECUTE,					 //거래 실행
	AGPMPRIVATETRADE_PACKET_TRADE_END,						 //거래 종료

	AGPMPRIVATETRADE_PACKET_ADD_TO_TRADE_GRID,				
	AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TRADE_GRID,			
	AGPMPRIVATETRADE_PACKET_MOVE_TRADE_GRID,				
	AGPMPRIVATETRADE_PACKET_MOVE_INVEN_GRID,				
	AGPMPRIVATETRADE_PACKET_ADD_TO_TARGET_TRADE_GRID,		
	AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TARGET_TRADE_GRID,	
	AGPMPRIVATETRADE_PACKET_MOVE_TARGET_TRADE_GRID,			
	AGPMPRIVATETRADE_PACKET_LOCK,							//내가 거래를 아이템을 다 올린뒤 ok!버튼 누른상태.
	AGPMPRIVATETRADE_PACKET_TARGET_LOCKED,					//거래자가 거래를 아이템을 다 올린뒤 ok!버튼 누른상태.
	AGPMPRIVATETRADE_PACKET_UNLOCK,							//내가 ok! 버튼 취소. -_-;(아이템이 Grid에서 빠지거나 올라온 경우도...)
	AGPMPRIVATETRADE_PACKET_TARGET_UNLOCKED,				//거래자가 ok! 버튼 취소. -_-;(아이템이 Grid에서 빠지거나 올라온 경우도...)
	AGPMPRIVATETRADE_PACKET_ACTIVE_READY_TO_EXCHANGE,		//최종 거래 승인버튼 활성화.
	AGPMPRIVATETRADE_PACKET_READY_TO_EXCHANGE,				//거래하자고 최종적으로 승인.
	AGPMPRIVATETRADE_PACKET_TARGET_READY_TO_EXCHANGE,		//상대방이 거래하자고 최종적으로 승인했다.
	AGPMPRIVATETRADE_PACKET_UPDATE_MONEY,					//내 거래창에 돈을 변경한다.
	AGPMPRIVATETRADE_PACKET_TARGET_UPDATE_MONEY,			//거래상대의 거래창의 돈을 변경한다.
	AGPMPRIVATETRADE_PACKET_TRADE_REFUSE,					//거래상대가 거래거부 상태이다.
} eAgsmPrivateTradeOperation;

typedef enum
{
	AGPMPRIVATETRADE_PACKET_TRADE_REUSLT_00	= 0x00,			 //거래성공!
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_01,				 //타켓이 없는경우
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_02,				 //거래할 수 없는 타켓. 
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_03,				 //거리가 멀어서 안되는경우.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_05,				 //내가 거래를 취소.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_06,				 //상대방이 거래를 취소.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_07,				 //선택버튼을 비활성화시킨다.

	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_08,				 //내가 이미 거래중인데 또 거래요청
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_09,				 //상대방이 이미 거래중인데 거래 요청.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_0A,				 //내가 현재 거래할수 없는 상태일경우.(죽었거나? 이미 거래중등...)
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_0B,				 //상대가 현재 거래할수 없는 상태일경우.(죽었거나? 이미 거래중등...)

	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_FF = 0xFF			 //알수 없는 오류
} eAgsmPrivateTradeErrorCode;

typedef enum
{
	AGPMPRIVATETRADE_CALLBACK_NONE = 0x00,
	AGPMPRIVATETRADE_CALLBACK_TRADE_BUDDY_CHECK,
} eAgsmPrivateTradeCallback;

class AgsmPrivateTrade : public AgsModule
{
	AgpmGrid		*m_pagpmGrid;

	ApmMap			*m_papmMap;
	AgpmCharacter	*m_pagpmCharacter;
	AgpmFactors		*m_pagpmFactors;
	AgpmItem		*m_pagpmItem;
	AgsmItem		*m_pagsmItem;
	AgpmItemConvert *m_pagpmItemConvert;
	//AgpmItemLog*	m_pagpmItemLog;
	AgpmLog*		m_pagpmLog;

	AgsmCharacter	*m_pagsmCharacter;

	//거래용 패킷
	AuPacket		m_csPacket;
	AuPacket		m_csOriginPos;
	AuPacket		m_csGridPos;

	BOOL AddItemFromTradeBoxToInventory( AgpdCharacter  *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nTradeLayer, INT16 nTradenRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus );
	BOOL AddItemFromInventoryToTradeBox( AgpdCharacter  *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn );

	BOOL ParseGridPos( PVOID pGridPos, INT16 *pnTradeLayer, INT16 *pnTradeRow, INT16 *pnTradeCol);

public:
	AgsmPrivateTrade();
	//어디에나 있는 OnAddModule이다.
	BOOL OnAddModule();
	//lCID에게 거래결과(성공,실패,오류등)를 알려준다.
	BOOL SendTradeResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult);
	//lCID에게 거래요청을 했고 기다리는 중이라고 알린다.
	BOOL SendWaitConfirm(AgpdCharacter *pcsAgpdCharacter);
	//TargetID에게 lCID가 거래를 요청했다고 알린다.
	BOOL SendRequestConfirm(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter);
	//TargetID가 거래거부 중이라고 알린다.
	BOOL SendRequestRefuse(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter);
	//Trade가 시작되었다고 알린다.
	BOOL SendTradeStart(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter);
	//거래 취소 패킷을 lCID에게날린다.
	BOOL SendTradeCancel( INT32 lCID );

	//내 거래창에 금액을 업뎃한다.
	BOOL SendTradeUpdateMoney( AgpdCharacter *pcsAgpdCharacter, INT32 lMoney );
	//상대 거래창의 금액을 업뎃한다.
	BOOL SendTradeUpdateTargetMoney( AgpdCharacter *pcsAgpdTargetCharacter, INT32 lMoney );

	//내 인벤에서 Trade Grid로 들어갈때
	BOOL SendTradeMoveInventoryToTradeGrid( INT32 lCID, INT32 lIID, AgpdItem *pcsAgpdItem );
	//거래자의 Trade Grid에서 인벤으로 들어갈때
	BOOL SendTradeMoveTargetInventoryToTargetTradeGrid( INT32 lCID, INT32 lIID, AgpdItem *pcsAgpdItem );
	//내 TradeGrid에서 인벤으로 이동할때
	BOOL SendTradeMoveTradeGridToInventory( INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn );
	//거래자의 TradeGrid에서 인벤으로 이동할때
	BOOL SendTradeMoveTargetTradeGridToTargetInventory( INT32 lCID, INT32 lIID, INT16 nLayer, INT16 nRow, INT16 nColumnm, INT16 nIvenLayer, INT16 nIvenRow, INT16 nIvenColumn  );

	//내가 아이템을 다 올리고 ok버튼을 누른상태.
	BOOL SendTradeLock(AgpdCharacter *pcsAgpdCharacter);
	//상대가 아이템을 다 올리고 ok버튼을 누른상태.
	BOOL SendTradeTargetLocked(AgpdCharacter *pcsAgpdCharacter);
	//내가 아이템을 다 올리고 ok버튼을 누른뒤~ 취소하고 싶을때...
	BOOL SendTradeUnlock(AgpdCharacter *pcsCharacter);
	//상대가 아이템을 다 올리고 ok버튼을 누른뒤~ 취소할때...
	BOOL SendTradeTargetUnlocked(AgpdCharacter *pcsCharacter);
	//최종거래 승인버튼을 활성화 시키라는 패킷을 날린다.
	BOOL SendActiveReadyToExchangeButton(AgpdCharacter *pcsAgpdCharacter);
	//최종거래 승인패킷을 날린다.
	BOOL SendReadyToExchange(AgpdCharacter *pcsAgpdCharacter);
	//상대가 최종거래 승인패킷을 날렸다고 알려준다.
	BOOL SendTargetReadyToExchange(AgpdCharacter *pcsAgpdCharacter);

	//거래요청을 처리해준다.
	BOOL ProcessTradeRequest( INT32 lCID, INT32 lTargetCID );
	//거래자로부터 Trade를 하겠다는 최종신호가 왔다.
	BOOL ProcessTradeConfirm( INT32 lCID );
	//(lCID의)거래상태를 초기화한다.
	BOOL ProcessTradeCancel( INT32 lCID );
	//돈을 올린다.
	BOOL ProcessTradeUpdateMoney( INT32 lCID, INT32 lMoneyCount );
	//최소시 돈과 아이템을 원상복구시킴.
	BOOL RestoreTradeItems( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar );

	BOOL ProcessAddToTradeGrid( INT32 lCID, INT32 lIID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn );
	BOOL ProcessRemoveFromTradeGrid( INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus );

	BOOL TestGridCopyInven(AgpdItemADChar *pcsAgpdItemADChar, AgpdGrid *pcTradeGrid);
	BOOL TestGridCopyTradeGrid(AgpdItemADChar *pcsAgpdItemADChar, AgpdGrid *pcTradeGrid);
	BOOL MoveTradeGridItemToInventory( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, INT32 lTimeStamp = 0);

	//JK_거래중금지
	BOOL RestoreTradeItemsMyself( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar);
	BOOL ProcessTradeCancelUnExpected( INT32 lCID );


	//아이템을 교환해준다.
	BOOL ProcessExchangeItem(AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar );
	//거래준비(모든 아이템을 거래창에 올린상태)가 다 되었다고 알려준다.
	BOOL ProcessLock( INT32 lCID );
	//거래준비(모든 아이템을 거래창에 올린상태)가 덜 되었다고 알려준다.
	BOOL ProcessUnlock( INT32 lCID );
	//최종거래 승인!! 취소 없기~
	BOOL ProcessReadyToExchange( INT32 lCID );

	BOOL CheckTradeStatus( INT32 lCID );
	void ResetTradeStatus( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar );

	BOOL ResetPrivateTradeOptionGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdADChar);

	//패킷을 받고 파싱하자~
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	
	//////////////////////////////////////////////////////////////////////////
	// Log 관련 - 2004.05.04. steeple
	INT32	GetCurrentTimeStamp();
	BOOL	WriteTradeItemLog(AgpdCharacter* pcsSeller, AgpdCharacter* pcsBuyer, AgpdItem* pcsItem, INT32 lTimeStamp = 0);
	BOOL	WriteTradeMoneyLog(AgpdCharacter* pcsSeller, AgpdCharacter* pcsBuyer, INT32 lMoney, INT32 lTimeStamp = 0);

	static BOOL	CBRemoveTradeGrid(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	// callback function
	BOOL SetCallbackBuddyCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);

private:
	BOOL CheckInventorySpace(AgpdCharacter* pcsCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter* pcsTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar);
	void GetItemCountByCoinType(AgpdGrid* pGrid, short* normalItemCount, short* cashItemCount);	// 일반 아이템과 캐쉬아이템의 개수를 구한다.
	INT32 NumberOfBoundOnOwnerItemInTradeGrid(AgpdItemADChar *pcsAgpdItemADChar);
};

#endif
