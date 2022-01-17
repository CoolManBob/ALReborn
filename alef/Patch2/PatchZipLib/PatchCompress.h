#pragma once

#include <malloc.h>
#include "LinkedList.h"

#define nCOMP_MEM_POOL_COUNT			2
#define nCOMP_MEM_POOL_BLOCK_SIZE		(4*1024*1024)		// 4M

class CCompress
{
private:
	FILE			*m_fpSourceFile, *m_fpDestFile;
	FILE			*m_fpPackFile;

	int				m_iSubDirFiles;

	CDList<char *>	m_cPackFileList;

	char*			m_pMemPool[nCOMP_MEM_POOL_COUNT];
	int				m_lMemPoolSize[nCOMP_MEM_POOL_COUNT];

public:

	CCompress()
	{
		memset( m_pMemPool, NULL, sizeof( char* ) * nCOMP_MEM_POOL_COUNT );
		memset( &m_lMemPoolSize, 0, sizeof( int ) * nCOMP_MEM_POOL_COUNT );
	}

	~CCompress()
	{
		RemoveMemPool();
	}

	void InitMemPool()
	{
		for ( int i = 0; i < nCOMP_MEM_POOL_COUNT; i++ )
		{
			m_pMemPool[i] = (char*) malloc(nCOMP_MEM_POOL_BLOCK_SIZE * 2);
			if ( m_pMemPool[i] )
				m_lMemPoolSize[i] += (nCOMP_MEM_POOL_BLOCK_SIZE * 2);
		}
	}

	void RemoveMemPool()
	{
		for ( int i = 0; i < nCOMP_MEM_POOL_COUNT; i++ )
		{
			if ( m_pMemPool[i] )
			{
				free( m_pMemPool[i] );
				m_pMemPool[i] = NULL;
			}

			m_lMemPoolSize[i] = 0;
		}
	}

	void RaiseMemPool( int need, int index = 0 )
	{
		if( need <= 0 )										return;
		if( index >= nCOMP_MEM_POOL_COUNT || index < 0 )	return;
		if( !m_pMemPool[index] )							return;

		int& size = m_lMemPoolSize[index];
		char*& mem = m_pMemPool[index];
		
		int cntBlock = need / nCOMP_MEM_POOL_BLOCK_SIZE;
		if ( need % nCOMP_MEM_POOL_BLOCK_SIZE > 0 )
			++cntBlock;

		size += ( cntBlock * nCOMP_MEM_POOL_BLOCK_SIZE );
		mem = (char*)realloc( mem, size );

		ASSERT( size == (int)_msize( mem ) );
	}

	void AllocMemPool( int size, int index = 0 )
	{
		if( size <= 0 )										return;
		if( 0 > index || index >= nCOMP_MEM_POOL_COUNT )	return;

		int& sizeMem = m_lMemPoolSize[index];
		char*& mem = m_pMemPool[index];

		if( mem )
		{
			free( mem );
			mem = NULL;
		}

		int cntBlock = size / nCOMP_MEM_POOL_BLOCK_SIZE;
		if ( size % nCOMP_MEM_POOL_BLOCK_SIZE > 0 )
			++cntBlock;

		sizeMem = cntBlock * nCOMP_MEM_POOL_BLOCK_SIZE;
		mem = (char*)malloc( sizeMem );

		ASSERT( sizeMem == (int)_msize( mem ) );
	}

	int GetMemPoolSize(int nIdx = 0)
	{
		if ( nIdx >= nCOMP_MEM_POOL_COUNT || nIdx < 0 )
			return 0;

		return m_lMemPoolSize[nIdx];
	}

	char* GetMemPool(int nIdx = 0)
	{
		if ( nIdx >= nCOMP_MEM_POOL_COUNT || nIdx < 0 )
			return 0;

		return m_pMemPool[nIdx];
	}

	FILE*	OpenFileMakeDir( char *pstrFileName, char *pstrOption );
	void	compressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName );
	void	decompressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName );
	void	decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char *pstrDestDir, char *pstrDestFileName );
	int		decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char** ppDestMem);
};