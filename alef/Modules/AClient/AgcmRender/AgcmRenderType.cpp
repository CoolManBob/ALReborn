#include "AgcmRenderType.h"
#include "AcuObject.h"
#include "ApMemoryTracker.h"
#include "ApModuleStream.h"

AgcmRenderType::AgcmRenderType()
{
}

AgcmRenderType::~AgcmRenderType()
{
}

BOOL AgcmRenderType::NewClumpRenderType(AgcdClumpRenderType *pcsType, INT32 lNum)
{
	if (lNum < 1)
		return FALSE;

	// 옮길 녀석을 임시 변수에 넣는다.
	AgcdClumpRenderType	stTemp;

	// 임시 변수에 데이터를 복사한다.
	if (pcsType->m_lSetCount > 0)
	{
		stTemp.m_lSetCount						= pcsType->m_lSetCount;

		stTemp.m_csRenderType.Alloc( pcsType->m_lSetCount );
	}

	// 현재 데이터를 날린다.
	DeleteClumpRenderType(pcsType);

	// 현재 데이터를 할당한다.
	pcsType->m_csRenderType.Alloc( lNum );
	// 초기화한다.

	pcsType->m_csRenderType.MemsetRenderType( 0 );
	pcsType->m_csRenderType.MemsetCustData( D_AGCD_CLUMP_RENDER_TYPE_DATA_CUST_DATA_NONE );

	pcsType->m_lSetCount		= lNum;

	// 임시 변수에 무언가 있으면...
	if (stTemp.m_lSetCount > 0 && stTemp.m_lSetCount <= lNum )
	{
		// 현재 데이터의 크기만큼만 복사한다.
		//memcpy(pcsType->m_csRenderType.m_plRenderType	+ (pcsType->m_lSetCount - stTemp.m_lSetCount), stTemp.m_csRenderType.m_plRenderType,	sizeof(INT32) * lNum);
		//memcpy(pcsType->m_csRenderType.m_plCustData		+ (pcsType->m_lSetCount - stTemp.m_lSetCount), stTemp.m_csRenderType.m_plCustData,		sizeof(INT32) * lNum);

		vector< INT32 >::reverse_iterator	iterDst , iterSrc;
		for(	iterSrc = stTemp.m_csRenderType.m_vecRenderType.rbegin(),
				iterDst = pcsType->m_csRenderType.m_vecRenderType.rbegin() ;
				iterSrc != stTemp.m_csRenderType.m_vecRenderType.rend();
				iterSrc++ , iterDst++ )
		{
			*iterDst = *iterSrc;
		}

		for(	iterSrc = stTemp.m_csRenderType.m_vecCustData.rbegin(),
				iterDst = pcsType->m_csRenderType.m_vecCustData.rbegin() ;
				iterSrc != stTemp.m_csRenderType.m_vecCustData.rend();
				iterSrc++ , iterDst++ )
		{
			*iterDst = *iterSrc;
		}

		// 임시 변수는 쓸모가 없으므로 해제한다.
		DeleteClumpRenderType(&stTemp);
	}

	return TRUE;
}

RpAtomic *ART_UTIL_SEARCH_CLUMP_RENDER_TYPE_CB(RpAtomic *atomic, void *data)
{
	AgcdClumpRenderType	*pcsType		= (AgcdClumpRenderType *)(data);
	
	// 2004.11.4 gemani
	//pcsType->m_csRenderType.m_plRenderType[pcsType->m_lSetCount - pcsType->m_lCBCount]	= (INT32)((pstArray[RI_BMODE] << 16) | (pstArray[RI_RTYPE]));

	INT32				blendmode		= atomic->stRenderInfo.blendMode;
	INT32				rendertype		= atomic->stRenderInfo.renderType;
	
	pcsType->m_csRenderType.m_vecRenderType[pcsType->m_lSetCount - pcsType->m_lCBCount]	= (INT32)((blendmode << 16) | (rendertype));
	
	return atomic;
}

BOOL AgcmRenderType::SearchClumpRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsType, INT32 lNum)
{
	DeleteClumpRenderType(pcsType);

	pcsType->m_csRenderType.Alloc( lNum );

	pcsType->m_csRenderType.MemsetRenderType( 0 );
	pcsType->m_csRenderType.MemsetCustData( D_AGCD_CLUMP_RENDER_TYPE_DATA_CUST_DATA_NONE );

	pcsType->m_lSetCount		= lNum;
	
	pcsType->m_lCBCount = 0;
	RpClumpForAllAtomics(pstClump, ART_UTIL_SEARCH_CLUMP_RENDER_TYPE_CB, (void *)(pcsType));

	return TRUE;
}

//@{ 2006/06/16 burumal	
BOOL AgcmRenderType::SearchClumpRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsSrcType, 
										   AgcdClumpRenderType *pcsDstType)
{
	if ( !pstClump || !pcsSrcType || !pcsDstType )
		return FALSE;

	if ( pcsSrcType->m_lSetCount > pcsDstType->m_lSetCount )
		return FALSE;

	RpAtomic* pFirstAtomic = pstClump->atomicList;
	RpAtomic* pCurAtomic = pFirstAtomic;

	while ( pCurAtomic )
	{
		if ( pCurAtomic->id >= 0 && pCurAtomic->id < pcsSrcType->m_lSetCount ) 
		{
			pcsDstType->m_csRenderType.m_vecRenderType[ pcsDstType->m_lSetCount - 1 - pCurAtomic->id ] =
				pcsSrcType->m_csRenderType.m_vecRenderType[ pcsSrcType->m_lSetCount - 1 - pCurAtomic->id ];
		}

		pCurAtomic = pCurAtomic->next;
		if ( pCurAtomic == pFirstAtomic )	
			break;
	}

	return TRUE;
}
//@}

VOID AgcmRenderType::DeleteClumpRenderType(AgcdClumpRenderType *pcsType)
{
	pcsType->m_csRenderType.Clear();
	pcsType->m_lSetCount					= 0;
}

INT32 AgcmRenderType::StreamReadClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType)
{
	CHAR		szValue[256];
	INT32		lTemp1, lTemp2, lTemp3;

	if (!strncmp(pcsStream->GetValueName(), AGCM_CLUMP_RENDER_TYPE_STREAM_NUM, strlen(AGCM_CLUMP_RENDER_TYPE_STREAM_NUM)))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d", &lTemp1);

		if (!NewClumpRenderType(pcsType, lTemp1))
			return E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_ERROR;

		return E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_READ;
	}
	else if (!strncmp(pcsStream->GetValueName(), AGCM_CLUMP_RENDER_TYPE_STREAM_RENDERTYPE, strlen(AGCM_CLUMP_RENDER_TYPE_STREAM_RENDERTYPE)))
	{
		pcsStream->GetValue(szValue, 256);
		// BOB님 작업(컨버젼용, 140704)
		if (sscanf(szValue, "%d:%d:%d", &lTemp1, &lTemp2, &lTemp3) < 3)
			lTemp3 = 0;

		pcsType->m_csRenderType.m_vecRenderType[lTemp1]	= lTemp2;
		pcsType->m_csRenderType.m_vecCustData[lTemp1]	= lTemp3;

		return E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_READ;
	}

	return E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_PASS;
}

VOID AgcmRenderType::StreamWriteClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType)
{
	CHAR	szValueName[256], szValue[256];

	if ( pcsType->m_lSetCount )
	{
		sprintf(szValue, "%d", pcsType->m_lSetCount);
		pcsStream->WriteValue(AGCM_CLUMP_RENDER_TYPE_STREAM_NUM, szValue);

		for (INT32 lCount = 0; lCount < pcsType->m_lSetCount; ++lCount)
		{
			sprintf(szValueName, "%s%d", AGCM_CLUMP_RENDER_TYPE_STREAM_RENDERTYPE, lCount);
			sprintf(szValue, "%d:%d:%d", lCount, pcsType->m_csRenderType.m_vecRenderType[lCount], pcsType->m_csRenderType.m_vecCustData[lCount]);
			pcsStream->WriteValue(szValueName, szValue);
		}
	}

	return;
}