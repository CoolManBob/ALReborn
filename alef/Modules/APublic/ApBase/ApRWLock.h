// ApRWLock.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2007. 04. 23.
//
// Reader Lock 은 여러 쓰레드에서 들어갈 수 있다.
// Reader Lock 이 걸려 있으면 다른 쓰레드에서 Writer Lock 할 수 없다.
// Writer Lock 걸려 있으면 다른 쓰레드에서 Reader / Writer Lock 할 수 없다.
//


#pragma once

#ifndef _APRWLOCK_H_
#define _APRWLOCK_H_

#include "ApDefine.h"
#include "MagDebug.h"

class ApRWLock
{
private:
	HANDLE m_hNobodyIsReading;
	HANDLE m_hWritingMutex;
	CRITICAL_SECTION m_csReading;
	int m_nReaders;

	BOOL m_bInit;

public:
	ApRWLock();
	ApRWLock(const ApRWLock& param);

	virtual ~ApRWLock();

	void Initialize();

	bool LockReader(DWORD dwTimeOut = INFINITE);
	void UnlockReader();
	bool LockWriter(DWORD dwTimeOut = INFINITE);
	void UnlockWriter();
};

// Auto Reader Lock
class ApAutoReaderLock
{
private:
	ApRWLock& m_csRWLock;

public:
	ApAutoReaderLock(ApRWLock& csRWLock) : m_csRWLock(csRWLock) { m_csRWLock.LockReader(); }
	virtual ~ApAutoReaderLock() { m_csRWLock.UnlockReader(); }
};

// Auto Writer Lock
class ApAutoWriterLock
{
private:
	ApRWLock& m_csRWLock;

public:
	ApAutoWriterLock(ApRWLock& csRWLock) : m_csRWLock(csRWLock) { m_csRWLock.LockWriter(); }
	virtual ~ApAutoWriterLock() { m_csRWLock.UnlockWriter(); }
};

#endif//_APRWLOCK_H_