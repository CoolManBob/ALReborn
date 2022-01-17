#ifndef __AGCD_RENDER_TYPE_H__
#define __AGCD_RENDER_TYPE_H__

#include <windows.h>
#include <vector>

using namespace std;

#define D_AGCD_CLUMP_RENDER_TYPE_DATA_CUST_DATA_NONE	-1

class AgcdClumpRenderTypeData
{
public:
	//INT32	*m_plRenderType;
	//INT32	*m_plCustData;
	vector< INT32 >	m_vecRenderType;
	vector< INT32 >	m_vecCustData;

	AgcdClumpRenderTypeData()
	{
		// do nothing..
	}

	virtual ~AgcdClumpRenderTypeData()
	{
		// do nothing..
	}

	void	Alloc( int nSize )
	{
		m_vecRenderType.resize( nSize );
		m_vecCustData.resize( nSize );
	}

	void	MemsetRenderType( INT32 nValue )
	{
		vector< INT32 >::iterator iter;
		for( iter = m_vecRenderType.begin();
			iter != m_vecRenderType.end();
			iter++ )
			*iter = nValue; // 초기화.
	}

	void	MemsetCustData( INT32 nValue )
	{
		vector< INT32 >::iterator iter;
		for( iter = m_vecCustData.begin();
			iter != m_vecCustData.end();
			iter++ )
			*iter = nValue; // 초기화.	
	}

	void	Clear()
	{
		m_vecRenderType.clear();
		m_vecCustData.clear();
	}
};

class AgcdClumpRenderType
{
public:
	INT32						m_lSetCount;
	AgcdClumpRenderTypeData		m_csRenderType;

	INT32						m_lCBCount;
	INT32						*m_plCustData;

	AgcdClumpRenderType()
	{
		m_lCBCount		= 0;
		m_lSetCount		= 0;
		m_plCustData	= NULL;
	}
};

#endif