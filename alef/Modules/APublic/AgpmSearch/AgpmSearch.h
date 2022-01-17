// AgpmSearch.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006. 06. 27.

#ifndef _AGPMSEARCH_H_
#define _AGPMSEARCH_H_

#include "ApModule.h"
#include "AuPacket.h"

#include "AgpdSearch.h"

typedef enum _eAgpmSearchPacketType
{
	APGMSEARCH_PACKET_SEARCH_NAME = 0,
	AGPMSEARCH_PACKET_SEARCH_TID,
	AGPMSEARCH_PACKET_SEARCH_LEVEL,
	AGPMSEARCH_PACKET_SEARCH_TID_LEVEL,
	AGPMSEARCH_PACKET_RESULT,
	AGPMSEARCH_PACKET_FAIL,
} eAgpmSearchPacketType;

typedef enum _eAgpmSearchCallback
{
	AGPMSEARCH_CB_SEARCH_NAME = 0,
	AGPMSEARCH_CB_SEARCH_TID,
	AGPMSEARCH_CB_SEARCH_LEVEL,
	AGPMSEARCH_CB_SEARCH_TID_LEVEL,
	AGPMSEARCH_CB_RESULT,
	AGPMSEARCH_CB_FAIL,
} eAgpmSearchCallback;

const int AGPMSEARCH_MAX_COUNT = 30;

class AgpmSearch : public ApModule
{
public:
	AuPacket m_csPacket;

private:
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnReceiveSearchName(AgpdSearch& stSearch, UINT32 ulNID);
	BOOL OnReceiveSearchTID(AgpdSearch& stSearch, UINT32 ulNID);
	BOOL OnReceiveSearchLevel(AgpdSearch& stSearch, UINT32 ulNID);
	BOOL OnReceiveSearchTIDLevel(AgpdSearch& stSearch, UINT32 ulNID);
	BOOL OnReceiveResult(AgpdSearch& stSearch, UINT32 ulNID);
	BOOL OnReceiveFail(AgpdSearch& stSearch, UINT32 ulNID);

public:
	AgpmSearch();
	virtual ~AgpmSearch();

	PVOID MakeSearchPacket(INT16* pnPacketLength, INT8 cOperation, AgpdSearch& stSearch);

	BOOL SetCallbackSearchName(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchTID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchTIDLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif//_AGPMSEARCH_H_
