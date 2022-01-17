/******************************************************************************
Module:  AgpmItem.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 2
******************************************************************************/

//99,105,109 확인버튼 RGB값.

#if !defined(__AGCMPRIVATETRADE_H__)
#define __AGCMPRIVATETRADE_H__

#include "AgcModule.h"
#include "ApBase.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgcmCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgpmGrid.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmPrivateTradeD" )
#else
#pragma comment ( lib , "AgcmPrivateTrade" )
#endif
#endif

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
	AGPMPRIVATETRADE_PACKET_TRADE_REFUSE,					//상대가 거래거부중이다.	2005.06.02. By SungHoon
} eAgcmPrivateTradeOperation;

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
} eAgcmPrivateTradeErrorCode;

// Command Message
enum 
{
	UICM_ACUITRADECONFIRM_COLSE_ME		=		0		// Window Close 를 요청한다 
};

enum eAgcmPrivateTradeCB_ID				// callback id in item module
{
	ITEM_CB_ID_UPDATE_LOCK_BUTTON			= 0,
	ITEM_CB_ID_UPDATE_TARGET_LOCK_BUTTON,
	ITEM_CB_ID_UPDATE_TRADE_MONEY,
	ITEM_CB_ID_UPDATE_TARGET_TRADE_MONEY,
	ITEM_CB_ID_TRADE_START,
	ITEM_CB_ID_TRADE_END,
	ITEM_CB_ID_TRADE_CANCEL,

//새로운 UI에 따라 새로 추가되는 CallBack
	ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE_YES_NO,
	ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE,
	ITEM_CB_ID_TRADE_CLOSE_UI_PRIVATETRADE,
	ITEM_CB_ID_TRADE_UI_UPDATE_TARGET_LOCK_BUTTON,
	ITEM_CB_ID_TRADE_UI_ACTIVE_READY_TO_EXCHANGE,
	ITEM_CB_ID_TRADE_UI_READY_TO_EXCHANGE,
	ITEM_CB_ID_TRADE_UI_CLIENT_READY_TO_EXCHANGE,

//PrivateTrade처리메세지
	ITEM_CB_ID_TRADE_UI_MSG_TRADE_COMPELTE,
	ITEM_CB_ID_TRADE_UI_MSG_TRADE_CANCEL,

	ITEM_CB_ID_TRADE_REQUESE_REFUSE_USER,			//	2005.06.02. By SungHoon
};

class AgcmPrivateTrade : public AgcModule
{
	ApmMap			*m_papmMap;
	AgpmCharacter	*m_pagpmCharacter;
	AgcmCharacter	*m_pagcmCharacter;
	AgpmFactors		*m_pagpmFactors;
	AgpmItem		*m_pagpmItem;
	AgcmItem		*m_pagcmItem;
	AgpmGrid		*m_pagpmGrid;

	//거래용 패킷
	AuPacket		m_csPacket;
	AuPacket		m_csOriginPos;
	AuPacket		m_csGridPos;

	AgpdCharacter *GetAgpdCharacter( INT32 lCID );

	//TradeGrid에 아이템이 들어간다.
	BOOL AddItemToTradeGrid( INT32 lCID, INT32 lIID, void *pGridPos );
	//TradeGrid에서 아이템이 지워진다.
	BOOL RemoveItemFromTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos );
	//TradeGrid에서 lIID가 이동한다.
	BOOL MoveItemTradeGrid( INT32 lCID, INT32 lIID, void *pOriginGridPos, void *pGridPos );
	//TargetTradeGrid에 아이템이 들어간다.
	BOOL AddItemToTargetTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos );
	//TargetTradeGrid에서 아이템이 지워진다.
	BOOL RemoveItemFromTargetTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos );

	//GridPos를 풀어낸다.
	BOOL ParseGridPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnColumn );

	BOOL ProcessLockButton( INT32 lCRequesterID, bool bPushDown );
	BOOL ProcessTargetLockButton( INT32 lCRequesterID, bool bPushDown );
	BOOL ProcessActiveReadyToExchange( INT32 lCRequesterID );
	BOOL ProcessReadyToExchange( INT32 lCRequesterID );
	BOOL ProcessClientReadyToExchange( INT32 lCRequesterID );

	BOOL ProcessCancel( INT32 lCRequesterID );

public:
	//Seong Yon-jun - Private Trade confirm Windows
	
	//AgcmPrivateTrade의 생성자
	AgcmPrivateTrade();
	//어디에나 있는 OnAddModule이다. 특별할거 없다. ㅠ.ㅠ
	BOOL OnAddModule();
	//내가(lCID) 누군가(lCTargetID)에게 거래를 요청한다.
	BOOL SendTradeRequest( INT32 lCID, INT32 lTargetCID );
	//Trade요청을 받아준다.
	BOOL SendTradeRequestConfirm( INT32 lCID );
	//Trade취소!
	BOOL SendTradeCancel( INT32 lCID );
	//돈을 올린다.
	BOOL SendTradeUpdateMoney( INT32 lCID, INT32 lMoneyCount );

	//TradeGrid로 Item을 넣는다( 수정 2009. 3. 3. )
	// 첫번째인자에 아이템 종류가 들어간다
	BOOL SendTradeAddItemToTradeGrid( eAGPM_PRIVATE_TRADE_ITEM eType , INT32 lCID, INT32 lIID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn );
	//TradeGrid에서 Item을 지운다
	BOOL SendTradeRemoveItemFromTradeGrid(INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus );
	//아이템을 다 올리고 ok!버튼을 누른상태.
	BOOL SendTradeLock();
	//아이템을 다 올리고 ok!버튼을 눌렀지만 취소하고 싶을때.... ㅠ.ㅠ
	BOOL SendTradeUnlock();
	//최종적으로 거래하자! 할때~
	BOOL SendReadyToExchange();

	//[CallBacks]
	//내 Lock Button이 토글될때~
	BOOL SetCallbackUpdateLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//상대방이 Lock Button을 눌렀을때~
	BOOL SetCallbackUpdateTargetLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//내가돈을 올릴때
	BOOL SetCallbackUpdateTradeMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//상대가 올린돈이 바뀔때
	BOOL SetCallbackUpdateTargetTradeMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//Trade가 시작되었음을 알리는 CB
	BOOL SetCallbackTradeStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//Trade가 끝났음을 알리는 CB
	BOOL SetCallbackTradeEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//Trade이 취소 되었을음 알리는 CB
	BOOL SetCallbackTradeCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//Trade UI콜백.
	BOOL SetCallbackTradeOpenUIYesNo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeOpenUI(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeCloseUI(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeUpdateTargetLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeActiveReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeClientReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//Trade MSG Callback
	BOOL SetCallbackTradeMSGComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeMSGCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackTradeRequestRefuseUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.06.02. By SungHoon
	
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	char			m_strDebugMessage[256];

	// Trade confirm UI 관련 함수 
	BOOL	OnTradeStart()			;			// Trade가 시작시..
	BOOL	OnUpdateLockButton()	;			// Confirm 버튼이 갱신되면..
	BOOL	OnUpdateTradeMoney()	;			// Money가 갱신되면
	BOOL	OnUpdateMyTradeMoney( INT32 lMoney )	;
	BOOL	OnUpdateHisTradeMoney( INT32 lMoney )	;
};

#endif
