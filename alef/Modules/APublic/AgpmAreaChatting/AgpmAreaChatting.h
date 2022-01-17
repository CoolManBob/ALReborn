#if !defined(__AGPMAREACHATTING_H__)
#define	__AGPMAREACHATTING_H__

#include "ApDefine.h"
#include "ApAdmin.h"

#include "ApModule.h"

#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmAreaChattingD" )
#else
#pragma comment ( lib , "AgpmAreaChatting" )
#endif
#endif

typedef enum _AgpmAreaChattingOperation {
	AGPMAREACHATTING_OPERATION_SEND_MESSAGE	= 0
} AgpmAreaChattingOperation;

typedef enum _AgpmAreaChattingCB {
	AGPMAREACHATTING_CB_RECV_SEND_MESSAGE	= 0
} AgpmAreaChattingCB;

class AgpmAreaChatting : public ApModule {
public:
	AuPacket	m_csPacket;

	AgpmAreaChatting();
	~AgpmAreaChatting();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL	SetCallbackRecvSendMessage(ApModuleDefaultCallBack fnCallback, PVOID pClass);

	PVOID	MakePacketSendMessage(INT32 lCID, INT8 cIsAdmin, INT8 cIsAddHeader, UINT32 ulTextColorRGB, CHAR *szMessage, UINT16 unMessageLength, INT16 *pnPacketLength);
};

#endif	//__AGPMAREACHATTING_H__