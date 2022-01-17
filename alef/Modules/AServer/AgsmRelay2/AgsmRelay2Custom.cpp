/*====================================================================

	AgsmRelay2Custom.cpp

====================================================================*/

#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"

//
//	==========		Custom		==========
//
void AgsmRelay2::InitPacketCustom()
	{
	m_csPacketCustom.SetFlagLength(sizeof(INT16));
	m_csPacketCustom.SetFieldType(AUTYPE_INT16,			1,		// eAgsmRelay2Operation
								  AUTYPE_INT32,			1,		// Query Index
								  AUTYPE_MEMORY_BLOCK,	1,		// Query Text
								  AUTYPE_MEMORY_BLOCK,	1,		// Headers
								  AUTYPE_UINT32,		1,		// No. of Rows
								  AUTYPE_UINT32,		1,		// No. of Cols
								  AUTYPE_UINT32,		1,		// Row Buffer Size
								  AUTYPE_MEMORY_BLOCK,	1,		// Buffer
								  AUTYPE_MEMORY_BLOCK,	1,		// Offset
								  AUTYPE_END,			0
								  );
	}


BOOL AgsmRelay2::OnParamCustom(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdDBParamCustom* pcsRelayCustom = new AgsdDBParamCustom;

	//INT16	nOperation		= 0;
	CHAR*	pszQuery		= NULL;
	INT16	nQueryLength	= 0;
	PVOID	pHeaders		= NULL;
	INT16	nHeadersLength	= 0;
	//UINT32	ulRows			= 0;
	//UINT32	ulCols			= 0;
	//PVOID		pBuffer			= NULL;
	//UINT32	ulRowBufferSize	= 0;
	//UINT32	ulTotalBufferSize	= 0;
	INT16	nTotalBufferSize	= 0;
	PVOID	pOffsets		= NULL;
	INT16	nOffsetsLength	=0;

	m_csPacketCustom.GetField(FALSE, pvPacket, 0,
							  &pcsRelayCustom->m_eOperation,		// op.
							  &pcsRelayCustom->m_lQueryIndex,		// query index
							  &pszQuery,							// query text
							  &nQueryLength,						// query text length
							  &pHeaders,							// headers text
							  &nHeadersLength,						//  headers text length
							  &pcsRelayCustom->m_ulRows,			// no. of rows
							  &pcsRelayCustom->m_ulCols,			// no. of cols
							  &pcsRelayCustom->m_ulRowBufferSize,	// 1 row's buffer size
							  &pcsRelayCustom->m_pBuffer,			// buffer
							  &nTotalBufferSize,					// total buffer size
							  &pOffsets,
							  &nOffsetsLength
							  );
	pcsRelayCustom->m_ulNID = ulNID;

	strncpy(pcsRelayCustom->m_szQuery, pszQuery ? pszQuery : _T(""), _MAX_QUERY_LENGTH);
	if (nHeadersLength < _MAX_HEADER_STRING_LENGTH)
		memcpy(pcsRelayCustom->m_szHeaders, pHeaders, nHeadersLength);
	else
		memcpy(pcsRelayCustom->m_szHeaders, pHeaders, _MAX_HEADER_STRING_LENGTH);
	pcsRelayCustom->m_ulTotalBufferSize = nTotalBufferSize;
	pcsRelayCustom->m_lOffsets.MemSetAll();
	// 여기서 nOffsetLength를 sizeof(INT32) 로 안나눠주면 메모리 침범한다.
	pcsRelayCustom->m_lOffsets.MemCopy(0, (INT32 *) pOffsets, nOffsetsLength / sizeof(INT32));

	return EnumCallback(AGSMDATABASE_PARAM_CUSTOM, (PVOID)pcsRelayCustom, (PVOID)nParam);
	}

BOOL AgsmRelay2::CBOperationCustom(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParamCustom *pAgsdRelay2 = (AgsdDBParamCustom *) pData;
	INT16 nParam = (INT16) pCustData;

	ASSERT(AGSMDATABASE_PARAM_CUSTOM == nParam);

	//#############	
	pAgsdRelay2->m_nParam = nParam;
	
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	if (0 == pAgsdRelay2->m_lQueryIndex)
		pQuery->Set(pAgsdRelay2->m_szQuery);
	else
		pQuery->m_nIndex = pAgsdRelay2->m_lQueryIndex;	
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperation,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}

BOOL AgsmRelay2::SendCustom(INT16 nOperation, CHAR* pszQuery, UINT32 ulCols, PVOID pBuffer,
							INT16 nBufferSize, PVOID pOffsets, INT16 nOffsetsSize)
	{
	if (!pszQuery || !pBuffer || 0 == nBufferSize || !pOffsets || 0 == nOffsetsSize)
		return FALSE;

	if (NULL == m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT32	lQueryIndex = 0;
	INT16	nQueryLength = (INT16)_tcslen(pszQuery) + sizeof(CHAR);
	UINT32	ulRows = 1;
	UINT32	ulRowBufferSize = nBufferSize;

	INT16 nPacketLength = 0;
	PVOID pvPacketCustom = m_csPacketCustom.MakePacket(FALSE, &nPacketLength, 0,
													   &nOperation,
													   &lQueryIndex,				// query index
													   pszQuery,					// query text
													   &nQueryLength,				// query text length
													   NULL,						// headers
																					// headers length
													   &ulRows,						// row count
													   &ulCols,						// col count
													   &ulRowBufferSize,			// row buffer size
													   pBuffer,						// buffer
													   &nBufferSize,				// buffer size
													   pOffsets,					// offsets
													   &nOffsetsSize				// offsetsSize
													   );
	BOOL bResult = MakeAndSendRelayPacket(pvPacketCustom, AGSMDATABASE_PARAM_CUSTOM); //, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacketCustom.FreePacket(pvPacketCustom);
	return bResult;
	}

BOOL AgsmRelay2::SendCustom(INT16 nOperation, INT32 lQueryIndex, UINT32 ulCols, PVOID pBuffer,
							INT16 nBufferSize, PVOID pOffsets, INT16 nOffsetsSize)
	{
	if (!pBuffer || 0 == nBufferSize || !pOffsets || 0 == nOffsetsSize)
		return FALSE;

	if (NULL == m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	UINT32	ulRows = 1;
	UINT32	ulRowBufferSize = nBufferSize;

	INT16 nPacketLength = 0;
	PVOID pvPacketCustom = m_csPacketCustom.MakePacket(FALSE, &nPacketLength, 0,
													   &nOperation,
													   &lQueryIndex,				// query index
													   NULL,						// query text
																					// query text length
													   NULL,						// headers
																					// headers length
													   &ulRows,						// row count
													   &ulCols,						// col count
													   &ulRowBufferSize,			// row buffer size
													   pBuffer,						// buffer
													   &nBufferSize,				// buffer size
													   pOffsets,					// offsets
													   &nOffsetsSize				// offsetsSize
													   );
	BOOL bResult = MakeAndSendRelayPacket(pvPacketCustom, AGSMDATABASE_PARAM_CUSTOM); //, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacketCustom.FreePacket(pvPacketCustom);
	return bResult;
	}

BOOL AgsmRelay2::SendCustom(AgsdDBParamCustom *pCustom, INT16 nParam, INT32 ulNID)
	{
	if (!pCustom)
		return FALSE;

	INT16 nQuerySize = ((INT16)_tcslen(pCustom->m_szQuery) +  1) * sizeof(CHAR);
	INT16 nBufferSize = pCustom->m_ulRowBufferSize * pCustom->m_ulRows;
	INT16 nOffsetsSize = (INT16)pCustom->m_ulCols * sizeof(INT32);

	INT16	nPacketLength = 0;
	INT16 nOperation = pCustom->m_eOperation;
	PVOID pvPacketCustom = m_csPacketCustom.MakePacket(FALSE, &nPacketLength, 0,
													   &nOperation,
													   &pCustom->m_lQueryIndex,		// query index
													   pCustom->m_szQuery,			// query text
													   &nQuerySize,					// query text size
													   NULL,						// headers
																					// headers size
													   &pCustom->m_ulRows,			// row count
													   &pCustom->m_ulCols,			// column count
													   &pCustom->m_ulRowBufferSize,	// row buffer size
													   pCustom->m_pBuffer,			// 
													   &nBufferSize,
													   &pCustom->m_lOffsets[0],			// offset
													   &nOffsetsSize				// offset size
													   );

	BOOL bResult = MakeAndSendRelayPacket(pvPacketCustom, AGSMDATABASE_PARAM_CUSTOM, ulNID);
	m_csPacketCustom.FreePacket(pvPacketCustom);
	return bResult;
	}

