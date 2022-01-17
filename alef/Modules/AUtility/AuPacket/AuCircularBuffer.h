// AuCircularBuffer.h: interface for the AuCircularBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUCIRCULARBUFFER_H__AAE7AAF3_844E_446F_A1B7_F11C5641FB0D__INCLUDED_)
#define AFX_AUCIRCULARBUFFER_H__AAE7AAF3_844E_446F_A1B7_F11C5641FB0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

class AuCircularBuffer  
{
private:
	CHAR* m_pBuffer;
	LONG m_lSize;
	LONG m_lCurrentIndex;
	ApCriticalSection	m_Mutex;

public:
	AuCircularBuffer();
	virtual ~AuCircularBuffer();

	BOOL Init(LONG lSize);
	VOID Destroy();
	PVOID Alloc(LONG lAllocSize);
	void Free(PVOID pvPacket);
};

#endif // !defined(AFX_AUCIRCULARBUFFER_H__AAE7AAF3_844E_446F_A1B7_F11C5641FB0D__INCLUDED_)
