// AgpmStartupEncryption.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#ifndef _AGPMSTARTUPENCRYPTION_H_
#define _AGPMSTARTUPENCRYPTION_H_

#include "ApModule.h"
#include "AuPacket.h"

typedef enum _eAgpmStartupEncryptionPacketType
{
	AGPMSTARTUPENCRYPTION_PACKET_REQUEST_PUBLIC = 0,
	AGPMSTARTUPENCRYPTION_PACKET_PUBLIC,
	AGPMSTARTUPENCRYPTION_PACKET_MAKE_PRIVATE,
	AGPMSTARTUPENCRYPTION_PACKET_COMPLETE,
	AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PUBLIC,
	AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PRIVATE,
	AGPMSTARTUPENCRYPTION_PACKET_ALGORITHM_TYPE,
} eAgpmStartupEncryptionPacketType;

typedef enum _eAgpmStartupEncryptionCallback
{
	AGPMSTARTUPENCRYPTION_CB_REQUEST_PUBLIC = 0,
	AGPMSTARTUPENCRYPTION_CB_PUBLIC,
	AGPMSTARTUPENCRYPTION_CB_MAKE_PRIVATE,
	AGPMSTARTUPENCRYPTION_CB_COMPLETE,
	AGPMSTARTUPENCRYPTION_CB_CHECK_COMPLETE,
	AGPMSTARTUPENCRYPTION_CB_DYNCODE_PUBLIC,
	AGPMSTARTUPENCRYPTION_CB_DYNCODE_PRIVATE,
	AGPMSTARTUPENCRYPTION_CB_ALGORITHM_TYPE,
} eAgpmStartupEncryptionCallback;

//const int AGPMSTARTUPENCRYPTION_MAX_PUBLIC_SIZE				= AUCRYPT_KEY_LENGTH_32BYTE;
//const int AGPMSTARTUPENCRYPTION_MAX_ENCRYPTED_PUBLIC_SIZE	= AGPMSTARTUPENCRYPTION_MAX_PUBLIC_SIZE + 8;

class AgpmStartupEncryption : public ApModule
{
public:
	AuPacket m_csPacket;

public:
	AgpmStartupEncryption();
	virtual ~AgpmStartupEncryption();

	BOOL OnAddModule();
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL OnReceiveAlgorithmType(BYTE* pData, INT16 nSize, UINT32 ulNID);
	BOOL OnReceiveRequestPublic(UINT32 ulNID);
	BOOL OnReceivePublic(BYTE* pData, INT16 nSize, UINT32 ulNID);
	BOOL OnReceiveMakePrivate(BYTE* pData, INT16 nSize, UINT32 ulNID);
	BOOL OnReceiveComplete(UINT32 ulNID);
	BOOL OnReceiveDynCodePublic(BYTE* pCode, INT16 nSize, UINT32 ulNID);
	BOOL OnReceiveDynCodePrivate(BYTE* pCode, INT16 nSize, UINT32 ulNID);

	BOOL SetCallbackAlgorithmType(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRequestPublic(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPublic(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMakePrivate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCheckComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDynCodePublic(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDynCodePrivate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	PVOID MakeStartupPacket(INT16* pnPacketLength, INT8 cOperation, BYTE* pData = NULL, INT16 nSize = 0);

	BOOL CheckCompleteStartupEncryption(UINT32 ulNID);
};

#endif//_AGPMSTARTUPENCRYPTION_H_