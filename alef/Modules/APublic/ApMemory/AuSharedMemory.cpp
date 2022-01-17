#include "AuSharedMemory.h"
#include <stdio.h>

AuSharedMemory* AuSharedMemory::pInstance;
ApCriticalSection m_CriticalSection;

AuSharedMemory& AuSharedMemory::GetInstance()
{
	if (NULL == pInstance)
	{
		AuAutoLock lock(m_CriticalSection);
		if (NULL == pInstance)
		{
			pInstance = new AuSharedMemory;
			pInstance->Create("Global\\AlefSMServerData");
		}
	}

	return *pInstance;
}

AuSharedMemory::AuSharedMemory()
{
	m_hFileMapping	= NULL;
	m_pSharedData	= NULL;
}

BOOL AuSharedMemory::Create(const char *i_szName)
{
	strncpy(m_szName, i_szName, SIZE_MAX_SHARED_DATA_NAME);

	m_hFileMapping = CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(AssmServerData), m_szName);

	if (m_hFileMapping == NULL)
	{
		// Could not create file mapping object.
		DWORD dwErr = GetLastError();
		char szTemp[256]; sprintf(szTemp, "err: %d", dwErr);
		MessageBox(NULL, szTemp, "Err", MB_OK);
		return FALSE;
	}

	m_bCreated = TRUE;

	if (m_hFileMapping != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Created, already exists. OK.
		m_bCreated = FALSE;
	}

	m_pSharedData = (AssmServerData*)MapViewOfFile(m_hFileMapping,			// handle to mapping object
											FILE_MAP_READ|FILE_MAP_WRITE,	// read/write permission
											0,								// max. object size
											0,								// size of hFile
											0);								// map entire file

	if (m_pSharedData == NULL)
	{
		// Could not map view of file.
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
		return FALSE;
	}

	return TRUE;
}

AuSharedMemory::~AuSharedMemory(void)
{
	if (m_pSharedData != NULL)
	{
		UnmapViewOfFile(m_pSharedData);
		m_pSharedData = NULL;
	}

	if (m_hFileMapping != NULL)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}
}
