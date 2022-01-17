// AgpmSearch.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006. 06. 27.

#include "AgpmSearch.h"

AgpmSearch::AgpmSearch()
{
	SetModuleName("AgpmSearch");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	SetPacketType(AGPMSEARCH_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_MEMORY_BLOCK,	1,	// AgpdSearch Structure
							AUTYPE_END,				0
							);
}

AgpmSearch::~AgpmSearch()
{
}

BOOL AgpmSearch::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	PVOID pvSearch = NULL;
	INT16 nSearchSize = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvSearch, &nSearchSize
						);

	// 이상한 데이터가 오면 낭패
	if(!pvSearch || sizeof(AgpdSearch) != nSearchSize)
		return FALSE;

	AgpdSearch& stSearch = *(static_cast<AgpdSearch*>(pvSearch));

	switch(cOperation)
	{
		case APGMSEARCH_PACKET_SEARCH_NAME:
			OnReceiveSearchName(stSearch, ulNID);
			break;

		case AGPMSEARCH_PACKET_SEARCH_TID:
			OnReceiveSearchTID(stSearch, ulNID);
			break;

		case AGPMSEARCH_PACKET_SEARCH_LEVEL:
			OnReceiveSearchLevel(stSearch, ulNID);
			break;

		case AGPMSEARCH_PACKET_SEARCH_TID_LEVEL:
			OnReceiveSearchTIDLevel(stSearch, ulNID);
			break;

		case AGPMSEARCH_PACKET_RESULT:
			OnReceiveResult(stSearch, ulNID);
			break;

		case AGPMSEARCH_PACKET_FAIL:
			OnReceiveFail(stSearch, ulNID);
			break;
	}

	return TRUE;
}

BOOL AgpmSearch::OnReceiveSearchName(AgpdSearch& stSearch, UINT32 ulNID)
{
	if(stSearch.m_szName[0] == '\0' || ulNID < 1)
		return FALSE;

	EnumCallback(AGPMSEARCH_CB_SEARCH_NAME, &stSearch, &ulNID);
	return TRUE;
}

BOOL AgpmSearch::OnReceiveSearchTID(AgpdSearch& stSearch, UINT32 ulNID)
{
	if(!stSearch.m_lTID || ulNID < 1)
		return FALSE;

	EnumCallback(AGPMSEARCH_PACKET_SEARCH_TID, &stSearch, &ulNID);
	return TRUE;
}

BOOL AgpmSearch::OnReceiveSearchLevel(AgpdSearch& stSearch, UINT32 ulNID)
{
	if(!stSearch.m_lMinLevel || !stSearch.m_lMaxLevel || ulNID < 1)
		return FALSE;

	EnumCallback(AGPMSEARCH_PACKET_SEARCH_LEVEL, &stSearch, &ulNID);
	return TRUE;
}

BOOL AgpmSearch::OnReceiveSearchTIDLevel(AgpdSearch& stSearch, UINT32 ulNID)
{
	if(!stSearch.m_lMinLevel || !stSearch.m_lMaxLevel || !stSearch.m_lTID || ulNID < 1)
		return FALSE;

	EnumCallback(AGPMSEARCH_PACKET_SEARCH_TID_LEVEL, &stSearch, &ulNID);
	return TRUE;
}

BOOL AgpmSearch::OnReceiveResult(AgpdSearch& stSearch, UINT32 ulNID)
{
	// 이름이 없으면 GG
	if(stSearch.m_szName[0] == '\0')
		return FALSE;

	EnumCallback(AGPMSEARCH_CB_RESULT, &stSearch, NULL);
	return TRUE;
}

BOOL AgpmSearch::OnReceiveFail(AgpdSearch& stSearch, UINT32 ulNID)
{
	EnumCallback(AGPMSEARCH_CB_FAIL, &stSearch, NULL);
	return TRUE;
}

PVOID AgpmSearch::MakeSearchPacket(INT16 *pnPacketLength, INT8 cOperation, AgpdSearch& stSearch)
{
	if(!pnPacketLength)
		return NULL;

	INT16 nSearchSize = sizeof(stSearch);
	return m_csPacket.MakePacket(TRUE, pnPacketLength, (INT8)AGPMSEARCH_PACKET_TYPE,
					&cOperation,
					&stSearch, &nSearchSize);
}

BOOL AgpmSearch::SetCallbackSearchName(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSEARCH_CB_SEARCH_NAME, pfCallback, pClass);
}

BOOL AgpmSearch::SetCallbackSearchTID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSEARCH_CB_SEARCH_TID, pfCallback, pClass);
}

BOOL AgpmSearch::SetCallbackSearchLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSEARCH_CB_SEARCH_LEVEL, pfCallback, pClass);
}

BOOL AgpmSearch::SetCallbackSearchTIDLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSEARCH_CB_SEARCH_TID_LEVEL, pfCallback, pClass);
}

BOOL AgpmSearch::SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSEARCH_CB_RESULT, pfCallback, pClass);
}

BOOL AgpmSearch::SetCallbackFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSEARCH_CB_FAIL, pfCallback, pClass);
}