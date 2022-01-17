#pragma once

#include "ApDefine.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpdEventGacha.h"
#include "AgpmFactors.h"
#include "AuPacket.h"

#include <map>
using namespace std;

const	UINT32	cGacha_Rolling_Time	= 5000;

class AgpmEventGacha : public ApModule
{
protected:
	// Data Members.
	enum	Operation
	{
		OP_REQUEST	,	// 가챠 요청 Client->Server
		OP_REQUESTGRANTED	,	// 가차 요청 결과 Server->Client
		OP_GACHA	,	// 굴리기 Client->Server
		OP_RESULT		// 결과물 Server->Client
	};

	// 가챠 정보를 저장하는 맵
	map< INT32	, AgpdGachaType			>	m_mapGachaType	;

	enum	eCallbackPoint
	{
		CB_EV_REQUEST	,
		CB_EV_REQUESTGRANTED	,
		CB_EV_GACHA		,
		CB_EV_RESULT
	};

	//AgpdGachaItemTable	*	GetGachItemTable( AuRace stRace		);
public:
	struct	GachaInfo
	{
		vector< INT32 > vecTID;
		ApdEvent		*pcsEvent	;
		INT32		lResult			;
		INT32		lCID			;
		INT32		lTID			;
		INT32		lItemCount		;
		INT32		lMoney			;
		INT32		lCharisma		;

		GachaInfo():
			pcsEvent	( NULL ),
			lResult		( 0 ),
			lCID		( 0 ),
			lTID		( 0 ),
			lItemCount	( 0 ),
			lMoney		( 0 ),
			lCharisma	( 0 )
		{}
	};

public:
	AuPacket			m_csPacket				;
	AgpmCharacter*		m_pcsAgpmCharacter		;
	ApmEventManager*	m_pcsApmEventManager	;
	AgpmFactors*		m_pcsAgpmFactor			;

	AgpmEventGacha();
	virtual ~AgpmEventGacha();

	// virtual functions
	BOOL	OnAddModule();
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	
	// Streaming functinos
	BOOL	StreamReadGachaTypeTable( const CHAR *szFile, BOOL bDecryption = FALSE );
	BOOL	BuildGachaItemTable();
	void	ReportGachaTable();

	enum ERROR_CODE
	{
		EC_NOERROR					,
		EC_NOT_ENOUGH_ITEM			,
		EC_NOT_ENOUGH_MONEY			,
		EC_NOT_ENOUGH_CHARISMA,
		EC_NOT_ENOUGH_INVENTORY		,
		EC_GACHA_ERROR				,
		EC_LEVELLIMIT				,
	};

	ERROR_CODE	CheckProperGacha( AgpdCharacter * pcsCharacter , INT32 nGachaType , vector< INT32 > * pVectorItem );
	// 가차가 가능한지 확인함.
	
	INT32	GetGachaItem( AgpdCharacter * pcsCharacter , INT32 nGachaType );

	// Data manage
	AgpdGachaType		*	GetGachaTypeInfo( INT32 nGachaIndex	);
	map< INT32	, AgpdGachaType	> *	GetGachaTypeMap() { return &m_mapGachaType; }

	PVOID	MakePacketEventRequest			(ApdEvent *pApdEvent, INT32 lCID, INT32 nGachaType , INT16 *pnPacketLength);
	PVOID	MakePacketEventRequestGranted	(ApdEvent *pApdEvent, INT32 lCID, INT32 nErrorCode , vector< INT32 > * pVector , INT16 *pnPacketLength);
	PVOID	MakePacketEventGacha			(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID	MakePacketEventResult			(ApdEvent *pApdEvent, INT32 lCID, INT32 eErrorCode , INT32 nTID , INT16 *pnPacketLength);

	// Callback Interface
	BOOL	SetCallbackRequest			(ApModuleDefaultCallBack pfCallback, PVOID pClass)	{ return SetCallback( CB_EV_REQUEST	, pfCallback , pClass ); }
	BOOL	SetCallbackRequestGranted	(ApModuleDefaultCallBack pfCallback, PVOID pClass)	{ return SetCallback( CB_EV_REQUESTGRANTED	, pfCallback , pClass ); }
	BOOL	SetCallbackGacha			(ApModuleDefaultCallBack pfCallback, PVOID pClass)	{ return SetCallback( CB_EV_GACHA	, pfCallback , pClass ); }
	BOOL	SetCallbackResult			(ApModuleDefaultCallBack pfCallback, PVOID pClass)	{ return SetCallback( CB_EV_RESULT	, pfCallback , pClass ); }

	static BOOL	CBAction(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);
};