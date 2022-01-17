//
// File: MemMappedFile.cpp
// Date: 2005/11/16
// Author: SeongKyeong@NHN games, skcho@nhncorp.com
//

#include "MemMappedFile.h"

// **************************************************************************
// MemMappedFile
// 
MemMappedFile::MemMappedFile()
:	m_file(INVALID_HANDLE_VALUE), m_map(NULL), m_ptr(NULL), m_fileSize(0)
{
}

MemMappedFile::~MemMappedFile()
{
	Close();
}

// 관련 handle을 모두 닫는다.
void MemMappedFile::Close()
{
	if (NULL != m_ptr)
	{
		UnmapViewOfFile(m_ptr);
		m_ptr = NULL;
	}

	if (NULL != m_map)
	{
		CloseHandle(m_map);
		m_map = NULL;
	}

	if (INVALID_HANDLE_VALUE != m_file)
	{
		CloseHandle(m_file);
		m_file = INVALID_HANDLE_VALUE;
	}
}

// 파일을 read only로 오픈하고 mapping한다.
bool MemMappedFile::OpenForReadOnly(const char* fileName)
{
	if (!fileName || strlen(fileName) > _MAX_PATH)
		return false;

	m_file = CreateFile(fileName,				// file name
						GENERIC_READ,			// read only access
						0,						// share mode
						NULL,					// security
						OPEN_EXISTING,			// create option
						FILE_ATTRIBUTE_NORMAL,	// file attribute
						NULL);					// handle to template file
	if (INVALID_HANDLE_VALUE == m_file)
		return false;

	// 전체 파일을 모두 mapping 한다, 뒤에서 3번째와 2번째의 0, 0이 이 역할을 한다.
	m_map = CreateFileMapping(m_file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (NULL == m_map)
	{
		Close();
		return false;
	}

	m_ptr = MapViewOfFile(m_map, FILE_MAP_READ, 0, 0, 0);
	if (NULL == m_ptr)
	{
		Close();
		return false;
	}

	// file 크기도 미리 저장해두자
	m_fileSize = ::GetFileSize(m_file, 0);

	return true;
}



















