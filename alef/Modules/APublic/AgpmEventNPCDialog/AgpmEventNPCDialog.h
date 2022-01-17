#ifndef	__AGPM_EVENT_NPCDIALOG_H__
#define __AGPM_EVENT_NPCDIALOG_H__

#include "ApModule.h"
#include "AuExcelTxtLib.h"
#include "AuRandomNumber.h"
#include "AgpdEventNPCDialog.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventNPCDialogD" )
#else
#pragma comment ( lib , "AgpmEventNPCDialog" )
#endif
#endif
//@} Jaewon

#define AGPMEVENT_NPCDIALOG_INI_NAME_START		"NPCDialogStart"
#define AGPMEVENT_NPCDIALOG_INI_NAME_END		"NPCDialogEnd"
#define AGPMEVENT_NPCDIALOG_INI_TEMPLATE		"Template"

#define AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE		1024
#define AGPMEVENT_NPCTDIALOG_MAX_TEMPLATE		1000

#define	AGPMEVENT_NPCDIALOG_MAX_USE_RANGE		600

//NPC Dialog Operation
typedef enum
{
	AGPDEVENTNPCDIALOG_REQUESTDIALOG	= 0,
} AgpdNPCDialogOperation;

//NPC Dialog Operation Result
typedef enum
{
	AGPDEVENTNPCDIALOG_RESULT_SUCCEEDED	= 0,
} AgpdNPCDialogResult;

typedef enum _AgpmEventNPCDialogPacketOperation {
	AGPMEVENT_NPCDIALOG_PACKET_OPERATION_REQUEST			= 0,
	AGPMEVENT_NPCDIALOG_PACKET_OPERATION_GRANT,
	AGPMEVENT_NPCDIALOG_PACKET_OPERATION_REQUEST_EX,
	AGPMEVENT_NPCDIALOG_PACKET_OPERATION_GRANT_EX,
} AgpmEventNPCDialogPacketOperation;

//CallBack함수 리스트.
typedef enum AgpmEventNPCDialogCallbackPoint			// Callback ID in Character Module 
{
	AGPMEVENT_NPCDIALOG_CB_ID_A			= 0,
	AGPMEVENT_NPCDIALOG_CB_ID_B,
	AGPMEVENT_NPCDIALOG_CB_REQUEST,
	AGPMEVENT_NPCDIALOG_CB_GRANT,
	AGPMEVENT_NPCDIALOG_CB_REQUEST_EX,
	AGPMEVENT_NPCDIALOG_CB_GRANT_EX,
	AGPMEVENT_NPCDIALOG_CB_REQUEST_MESSAGEBOX,
	AGPMEVENT_NPCDIALOG_CB_GRANT_MESSAGEBOX,
	AGPMEVENT_NPCDIALOG_CB_REQUEST_MENU,
	AGPMEVENT_NPCDIALOG_CB_GRANT_MENU,
} AgpmEventNPCDialogCallbackPoint;

class AgpmEventNPCDialog : public ApModule  
{
	AuExcelTxtLib		m_csExcelTxtLib;

	ApmEventManager		*m_pcsApmEventManager;
	AgpmCharacter		*m_pcsAgpmCharacter;

	ApAdmin				m_csNPCDialogTemplate;
	ApAdmin				m_csMobDialogTemplate;
	ApAdmin				m_csMobDialogKeyTemplate;

	MTRand				m_csRand;

public:
	AuPacket			m_csPacket;
	AuPacket			m_csPacketEventData;

public:
	AgpmEventNPCDialog();
	virtual ~AgpmEventNPCDialog();

	BOOL			OnAddModule();
	BOOL			OnDestroy();

	static BOOL			CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL LoadMobDialogRes( char *pstrFileName , BOOL bDecryption = TRUE );
	BOOL LoadNPCDialogRes( char *pstrFileName , BOOL bDecryption = TRUE );

	ApAdmin *GetNPCTemplate();
	ApAdmin *GetMobTemplate();

	AgpdEventNPCDialogTemplate *GetNPCDialogTemplate( INT32 lTemplateTID );

	AgpdEventMobDialogKeyTemplate *GetMobKeyDialogTemplate( INT32 lMobKeyTID );
	AgpdEventMobDialogTemplate *GetMobDialogTemplate( INT32 lDialogID );

	char *GetMobDialog( INT32 lMobTID, eAgpdEventNPCDialogMob lDialogType );

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				OnOperationRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationGrant(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);

	BOOL				OnOperationRequestEx(PVOID pvPacket);
	BOOL				OnOperationGrantEx(PVOID pvPacket);

	BOOL				SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID				MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);

	static BOOL			CBActionNPCDialog(PVOID pData, PVOID pClass, PVOID pCustData);

	PVOID				MakePacketEventData(ApdEvent *pcsEvent);
	BOOL				ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData);
	static BOOL			CBMakePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBParsePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData);

};

#endif