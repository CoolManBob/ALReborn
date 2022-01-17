#ifndef	__AGPMBILLINFO_H__
#define	__AGPMBILLINFO_H__

#include "ApDefine.h"

#include "AgpdBillInfo.h"

#include "AgpmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmBillInfoD" )
#else
#pragma comment ( lib , "AgpmBillInfo" )
#endif
#endif


typedef enum	_AgpmBillInfoOperation {
	AGPMBILL_OPERATION_ADD					= 0,
	AGPMBILL_OPERATION_CASHINFO,			//JK_ºô¸µ
	AGPMBILL_OPERATION_GUID,
	AGPMBILL_OPERATION_VERSIONINFO_CSREQ,			//JK_ºô¸µ
	AGPMBILL_OPERATION_VERSIONINFO_SCACK,			//JK_ºô¸µ
	AGPMBILL_OPERATION_BUYPRODUCTRESULT_SCACK,		//JK_ºô¸µ

} AgpmBillInfoOperation;

typedef enum	_AgpmBillInfoCBID {
	AGPMBILL_UPDATE_BIIL_INFO				= 0,
	AGPMBILL_UPDATE_SALESZONESCRIPTVERSION_INFO,		//JKÀ¥Á¨ºô¸µ
	AGPMBILL_UPDATE_SALESZONESCRIPTVERSION_INFO_ACK,	//JKÀ¥Á¨ºô¸µ
} AgpmBillInfoCBID;

class AgpmBillInfo : public ApModule {
private:
	AgpmCharacter	*m_pcsAgpmCharacter;

	INT32			m_lIndexADCharacter;

public:
	AuPacket		m_csPacket;

public:
	AgpmBillInfo();
	virtual ~AgpmBillInfo();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	AgpdBillInfo	*GetADCharacter(AgpdCharacter *pcsCharacter);

	PVOID	MakePacketBillInfo(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	BOOL	SetCallbackUpdateBillInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	UpdateIsPCRoom(AgpdCharacter *pcsCharacter, BOOL bIsPCRoom);

	BOOL	IsPCBang(AgpdCharacter *pcsCharacter);
	//JKºô¸µ
	BOOL	SetCashGlobal(AgpdCharacter* pcsCharacter, double WCoin, double PCoin);
	BOOL	GetCashGlobal(AgpdCharacter* pcsCharacter, double& WCoin, double& PCoin);

	BOOL	SetGUIDGlobal(AgpdCharacter* pcsCharacter, DWORD AccountGUID);
	DWORD	GetGUIDGlobal(AgpdCharacter* pcsCharacter);

	BOOL	SetPCRoomGUIDGlobal(AgpdCharacter* pcsCharacter, DWORD PCRoomGUID, DWORD ClientCnt);//JK_º¹¼öÅ¬¶óÀÌ¾ðÆ®
	DWORD	GetPCRoomGUIDGlobal(AgpdCharacter* pcsCharacter);
	DWORD	GetClientCntGlobal(AgpdCharacter* pcsCharacter);
	
	BOOL	SetAccountAge( AgpdCharacter* pcsCharacter,  DWORD dwAge );//JK_½É¾ß¼¦´Ù¿î
	DWORD   GetAccountAge( AgpdCharacter* pcsCharacter );//JK_½É¾ß¼¦´Ù¿î

	//JK_À¥Á¨ºô¸µ
	BOOL	SetCallbackUpdateSalesZoneScriptVersionInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateSalesZoneScriptVersionInfoAck(ApModuleDefaultCallBack pfCallback, PVOID pClass);


};

#endif	//__AGPMBILLINFO_H__