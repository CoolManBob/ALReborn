#ifndef __AGCDPRELOD_H__
#define __AGCDPRELOD_H__

#include "apdefine.h"
#include "acuobject.h"

#define AGCD_LODDATA_BUFFER_SIZE	256

class AgcdPreLODData
{
public:
	INT32		m_lIndex;

	BOOL		m_bUseAtomicIndex;
	BOOL		m_abTransformGeom[AGPDLOD_MAX_NUM];
	IsBillData	m_stBillboard;
	CHAR		m_aszData[AGPDLOD_MAX_NUM][AGCD_LODDATA_BUFFER_SIZE];

	AgcdPreLODData()
	{
		INT32	lCount;

		m_lIndex			= 0;
		m_bUseAtomicIndex	= FALSE;

		m_stBillboard.num	= 0;

		for(lCount = 0; lCount < BILLARRAYNUM; ++lCount)
			m_stBillboard.isbill[lCount] = 0;

		for(lCount = 0; lCount < AGPDLOD_MAX_NUM; ++lCount)
		{
			m_aszData[lCount][0]		= 0;
			m_abTransformGeom[lCount]	= 0;
		}
	}
};

typedef struct _AgcdPreLODList
{
	AgcdPreLODData		m_csData;
	_AgcdPreLODList		*m_pstNext;
} AgcdPreLODList;

typedef struct _AgcdPreLOD
{
	INT32				m_lNum;
	AgcdPreLODList		*m_pstList;
} AgcdPreLOD;

#endif // __AGCDPRELOD_H__