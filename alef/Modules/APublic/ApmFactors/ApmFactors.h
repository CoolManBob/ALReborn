// ApmFactors.h: interface for the ApmFactors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMFACTORS_H__BF339BA7_252E_4C47_9413_0D2F8D3AA7DF__INCLUDED_)
#define AFX_APMFACTORS_H__BF339BA7_252E_4C47_9413_0D2F8D3AA7DF__INCLUDED_

#if _MSC_VER > 1000
//#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "ApModule.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApmFactorsD" )
#else
#pragma comment ( lib , "ApmFactors" )
#endif
#endif
#endif
//@} Jaewon


#define APM_FACTOR_MAX_FACTORS		15

#define APM_FACTOR(pFactor, DataType, nType, nIndex)	(((DataType *) pFactor->m_pvFactor[nType])[nIndex])
#define APM_FACTOR_HAS_TYPE(pFactor, nType)				(pFactor->m_pvFactor[nType])

typedef enum ApmFactorDataType
{
	APM_FACTOR_DATATYPE_DEFAULT = 0,
	APM_FACTOR_DATATYPE_INT8,
	APM_FACTOR_DATATYPE_UINT8,
	APM_FACTOR_DATATYPE_INT16,
	APM_FACTOR_DATATYPE_UINT16,
	APM_FACTOR_DATATYPE_INT32,
	APM_FACTOR_DATATYPE_UINT32,
	APM_FACTOR_DATATYPE_INT64,
	APM_FACTOR_DATATYPE_UINT64,
	APM_FACTOR_DATATYPE_FLOAT, 
	APM_FACTOR_DATATYPE_END
} ApmFactorDataType;

typedef struct AgpdFactor
{
	VOID	*m_pvFactor[APM_FACTOR_MAX_FACTORS];
} AgpdFactor;

class ApmFactors : public ApModule  
{
protected:
	CHAR *					m_aszFactorTypeName[APM_FACTOR_MAX_FACTORS];
	INT16					m_nFactor;

private:
	INT16					m_anFactorNumber[APM_FACTOR_MAX_FACTORS];
	ApmFactorDataType *		m_panFactorDataType[APM_FACTOR_MAX_FACTORS];
	CHAR **					m_paszFactorName[APM_FACTOR_MAX_FACTORS];

public:
	BOOL OnInit();
	ApmFactorDataType GetDataType(const CHAR *szDataType);
	CHAR * GetFactorName(UINT16 nType, UINT16 nIndex);
	BOOL SearchFactorName(const CHAR *szFactorName, UINT16 *pnType, UINT16 *pnIndex);
	BOOL DestroyFactor(AgpdFactor *pstFactor);
	BOOL InitFactor(AgpdFactor *pstFactor, UINT16 nType, ApmFactorDataType nDataType);
	BOOL SetFactorNumber(INT16 nFactor);
	BOOL AddEntry(UINT8 nType, const CHAR *szName, ApmFactorDataType nDataType);
	BOOL OnDestroy();

	ApmFactors();
	virtual ~ApmFactors();
};

#endif // !defined(AFX_APMFACTORS_H__BF339BA7_252E_4C47_9413_0D2F8D3AA7DF__INCLUDED_)
