#pragma once

#include "ApMutualEx.h"
#include "ApServerData.h"

#define SIZE_MAX_SHARED_DATA_NAME	256

class AuSharedMemory
{
public:
	~AuSharedMemory(void);

	static AuSharedMemory& GetInstance();
	BOOL Create(const char *i_szName);
	AssmServerData* GetServerData() { return m_pSharedData; }
	const char* GetSharedMemoryName() { return m_szName; }
	BOOL IsCreated() { return m_bCreated; }
	BOOL IsOpened() { return !m_bCreated; }

private:
	AuSharedMemory();

private:
	HANDLE			m_hFileMapping;
	AssmServerData*	m_pSharedData;
	char			m_szName[SIZE_MAX_SHARED_DATA_NAME];
	BOOL			m_bCreated;

	static AuSharedMemory* pInstance;
};
