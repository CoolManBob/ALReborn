#pragma once

#include "ApBase.h"

class stBuddyRowset
{
public:
	enum
	{
		BUDDY_MAX_QUERY_LENGTH = 512,
		BUDDY_MAX_HEADER_STRING_LENGTH = 512,
		BUDDY_MAX_COLUMN_SIZE = 32,
	};

	INT32	m_lQueryIndex;						// query index, if ne 0
	CHAR	m_szQuery[BUDDY_MAX_QUERY_LENGTH];		// custom query string

	CHAR	m_szHeaders[BUDDY_MAX_HEADER_STRING_LENGTH];		// column headers
	UINT32	m_ulRows;									// row count
	UINT32	m_ulCols;									// column count

	PVOID	m_pBuffer;							// data buffer ptr.
	UINT32	m_ulRowBufferSize;					// size of 1 row
	UINT32	m_ulTotalBufferSize;				// total buffer size. m_ulRowBufferSize * m_ulCols
	ApSafeArray<INT32, BUDDY_MAX_COLUMN_SIZE>		m_lOffsets;
	
public:
	stBuddyRowset()
		{
		m_lQueryIndex = 0;
		ZeroMemory(m_szQuery, sizeof(m_szQuery));

		ZeroMemory(m_szHeaders, sizeof(m_szHeaders));
		m_ulRows = 0;
		m_ulCols = 0;

		m_pBuffer = NULL;
		m_ulRowBufferSize = 0;
		m_ulTotalBufferSize = 0;
		m_lOffsets.MemSetAll();;
		}
		
	CHAR*	Get(UINT32 ulRow, UINT32 ulCol)
		{
		if (ulRow >= m_ulRows || ulCol > m_ulCols)
			return NULL;

		ASSERT(ulCol < BUDDY_MAX_COLUMN_SIZE);

		return (CHAR *)m_pBuffer + m_lOffsets[ulCol] + (ulRow * m_ulRowBufferSize);
		}
};