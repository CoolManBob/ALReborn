// ApmFactors.cpp: implementation of the ApmFactors class.
//
//////////////////////////////////////////////////////////////////////

#include "ApmFactors.h"
#include "ApMemoryTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static CHAR *g_aszFactorDataTypeName[APM_FACTOR_DATATYPE_END] = 
{
	"", 
	"INT8", 
	"UINT8", 
	"INT16", 
	"UINT16", 
	"INT32", 
	"UINT32", 
	"INT64", 
	"UINT64", 
	"FLOAT"
};

ApmFactors::ApmFactors()
{
	SetModuleName("ApmFactors");

	m_nFactor = 0;
	memset(m_anFactorNumber, 0, sizeof(INT16) * APM_FACTOR_MAX_FACTORS);
	memset(m_panFactorDataType, 0, sizeof(ApmFactorDataType *) * APM_FACTOR_MAX_FACTORS);
	memset(m_paszFactorName, 0, sizeof(CHAR **) * APM_FACTOR_MAX_FACTORS);
}

ApmFactors::~ApmFactors()
{
	OnDestroy();
}

BOOL ApmFactors::OnInit()
{
	return TRUE;
}

BOOL ApmFactors::OnDestroy()
{
	for (m_nFactor--; m_nFactor >= 0; --m_nFactor)
	{
		for (m_anFactorNumber[m_nFactor]--; m_anFactorNumber[m_nFactor] >= 0; --m_anFactorNumber[m_nFactor])
		{
			if (m_paszFactorName[m_nFactor][m_anFactorNumber[m_nFactor]])
				delete [] (BYTE*)(m_paszFactorName[m_nFactor][m_anFactorNumber[m_nFactor]]);
//				free(m_paszFactorName[m_nFactor][m_anFactorNumber[m_nFactor]]);
		}

		if (m_panFactorDataType[m_nFactor])
			delete [] (BYTE*)(m_panFactorDataType[m_nFactor]);

		if (m_paszFactorName[m_nFactor])
			delete [] (BYTE*)(m_paszFactorName[m_nFactor]);
	}

	m_nFactor = 0;
	memset(m_anFactorNumber, 0, sizeof(INT16) * APM_FACTOR_MAX_FACTORS);
	memset(m_panFactorDataType, 0, sizeof(ApmFactorDataType *) * APM_FACTOR_MAX_FACTORS);
	memset(m_paszFactorName, 0, sizeof(CHAR **) * APM_FACTOR_MAX_FACTORS);

	return TRUE;
}

BOOL ApmFactors::AddEntry(UINT8 nType, const CHAR *szName, ApmFactorDataType nDataType)
{
	if (nType >= m_nFactor)
		return FALSE;

	if (SearchFactorName(szName, NULL, NULL))
		return FALSE;

	++m_anFactorNumber[nType];

	if (m_panFactorDataType[nType])
	{
		delete [] (BYTE*)m_panFactorDataType[nType];
	}
	m_panFactorDataType[nType] = (ApmFactorDataType *) new BYTE[sizeof(ApmFactorDataType) * m_anFactorNumber[nType]];

	if (m_paszFactorName[nType])
	{
		delete [] (BYTE*)m_paszFactorName[nType];
	}
	m_paszFactorName[nType] = (CHAR **) new BYTE[sizeof(CHAR *) * m_anFactorNumber[nType]];

	if (!m_panFactorDataType[nType] || !m_paszFactorName[nType])
		return FALSE;

	m_paszFactorName[nType][m_anFactorNumber[nType] - 1] = (CHAR *) new BYTE[sizeof(CHAR) * strlen(szName) + 1];
	if (!m_paszFactorName[nType][m_anFactorNumber[nType] - 1])
		return FALSE;

	m_panFactorDataType[nType][m_anFactorNumber[nType] - 1] = nDataType;

	strcpy(m_paszFactorName[nType][m_anFactorNumber[nType] - 1], szName);

	return TRUE;
}

BOOL ApmFactors::SetFactorNumber(INT16 nFactor)
{
	if (nFactor <= 0 && nFactor > APM_FACTOR_MAX_FACTORS)
		return FALSE;

	m_nFactor = nFactor;

	return TRUE;
}

BOOL ApmFactors::InitFactor(AgpdFactor *pstFactor, UINT16 nType, ApmFactorDataType nDataType)
{
	INT32	nDataSize;

	// 잘못된 값이 들어오면, FALSE
	if (nType >= m_nFactor || !pstFactor)
		return FALSE;

	// 이미 해당 Type의 Factor가 정의되어 있으면, FALSE
	if (pstFactor->m_pvFactor[nType])
		return FALSE;

	switch(nDataType)
	{
	case APM_FACTOR_DATATYPE_INT8:
		nDataSize = sizeof(INT8);
		break;
	case APM_FACTOR_DATATYPE_UINT8:
		nDataSize = sizeof(UINT8);
		break;
	case APM_FACTOR_DATATYPE_INT16:
		nDataSize = sizeof(INT16);
		break;
	case APM_FACTOR_DATATYPE_UINT16:
		nDataSize = sizeof(UINT16);
		break;
	case APM_FACTOR_DATATYPE_INT32:
		nDataSize = sizeof(INT32);
		break;
	case APM_FACTOR_DATATYPE_UINT32:
		nDataSize = sizeof(UINT32);
		break;
	case APM_FACTOR_DATATYPE_INT64:
		nDataSize = sizeof(INT64);
		break;
	case APM_FACTOR_DATATYPE_UINT64:
		nDataSize = sizeof(UINT64);
		break;
	case APM_FACTOR_DATATYPE_DEFAULT:
	case APM_FACTOR_DATATYPE_FLOAT:
		nDataSize = sizeof(FLOAT);
		break;
	default:
		return FALSE;
	}

//	pstFactor->m_pvFactor[nType] = (FLOAT *) malloc(nDataSize * m_anFactorNumber[nType]);
	pstFactor->m_pvFactor[nType] = (FLOAT *) new BYTE[nDataSize * m_anFactorNumber[nType]];
	if (!pstFactor->m_pvFactor[nType])
		return FALSE;

	return TRUE;
}

BOOL ApmFactors::DestroyFactor(AgpdFactor *pstFactor)
{
	INT32 i;

	for (i = 0; i < APM_FACTOR_MAX_FACTORS; ++i)
	{
		if (pstFactor && pstFactor->m_pvFactor[i])
			delete [] (BYTE*)(pstFactor->m_pvFactor[i]);

		pstFactor->m_pvFactor[i] = NULL;
	}

	return TRUE;
}

BOOL ApmFactors::SearchFactorName(const CHAR *szFactorName, UINT16 *pnType, UINT16 *pnIndex)
{
	for (INT32 i = 0; i < m_nFactor; ++i)
	{
		for (INT32 j = 0; j < m_anFactorNumber[i]; ++j)
		{
			if(!strcmp(szFactorName, m_paszFactorName[i][j]))
			{
				if (pnType)
					*pnType = i;

				if (pnIndex)
					*pnIndex = j;

				return TRUE;
			}
		}
	}

	return FALSE;
}

CHAR * ApmFactors::GetFactorName(UINT16 nType, UINT16 nIndex)
{
	if (nType >= m_nFactor || nIndex >= m_anFactorNumber[nType])
		return NULL;

	return m_paszFactorName[nType][nIndex];
}

ApmFactorDataType ApmFactors::GetDataType(const CHAR *szDataType)
{
	for (INT32 i = 0; i < APM_FACTOR_DATATYPE_END; ++i)
	{
		if (!strcmp(szDataType, g_aszFactorDataTypeName[i]))
			return (ApmFactorDataType) i;
	}

	return (ApmFactorDataType) 0;
}
