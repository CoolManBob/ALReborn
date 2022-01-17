//
// File: MemMappedFile.h
// Date: 2005/11/16
// Author: SeongKyeong@NHN games, skcho@nhncorp.com
//

#pragma once

#include <winsock2.h>

class MemMappedFile
{
public:
	MemMappedFile(void);
	~MemMappedFile(void);

	bool OpenForReadOnly(const char* fileName);
	void Close();
	
	DWORD GetFileSize()		  { return m_fileSize; }
	const void* GetStartPtr() { return m_ptr; }

private:
	HANDLE	m_file;
	HANDLE	m_map;
	void*	m_ptr;

	DWORD	m_fileSize;
};
