#ifndef __COMPRESS_LIB__
#define __COMPRESS_LIB__

#include <malloc.h>

#include "LinkedList.h"


/*#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "PatchZipLibD" )
#pragma comment ( lib , "zLibStat" )
#else
#pragma comment ( lib , "PatchZipLib" )
#pragma comment ( lib , "zLibStat" )
#endif
#endif*/

//@{ 2006/04/11 burumal
#define nCOMP_MEM_POOL_COUNT			2
#define nCOMP_MEM_POOL_BLOCK_SIZE		(4*1024*1024)		// 4M
//@}

class	CCompress
{
private:
	FILE			*m_fpSourceFile, *m_fpDestFile;
	FILE			*m_fpPackFile;

	int				m_iSubDirFiles;

	CDList<char *>	m_cPackFileList;

	//@{ 2006/04/11 burumal
	char*			m_pMemPool[nCOMP_MEM_POOL_COUNT];
	int				m_lMemPoolSize[nCOMP_MEM_POOL_COUNT];
	//@}

public:

	//@{ 2006/04/11 burumal
	CCompress()
	{
		int i;
		for ( i = 0; i < nCOMP_MEM_POOL_COUNT; i++ )
		{
			m_pMemPool[i] = NULL;
			m_lMemPoolSize[i] = 0;
		}
	}

	~CCompress()
	{
		RemoveMemPool();
	}

	void InitMemPool()
	{
		int i;
		for ( i = 0; i < nCOMP_MEM_POOL_COUNT; i++ )
		{
			m_pMemPool[i] = (char*) malloc(nCOMP_MEM_POOL_BLOCK_SIZE * 2);
			if ( m_pMemPool[i] == NULL )
				return;

			m_lMemPoolSize[i] += (nCOMP_MEM_POOL_BLOCK_SIZE * 2);
			//ASSERT(_msize(m_pMemPool[i]) == m_lMemPoolSize[i]);
		}
	}

	void RemoveMemPool()
	{
		int i;

		for ( i = 0; i < nCOMP_MEM_POOL_COUNT; i++ )
		{
			if ( m_pMemPool[i] != NULL )
			{
				free(m_pMemPool[i]);
				m_pMemPool[i] = NULL;

				m_lMemPoolSize[i] = 0;
			}
		}
	}

	void RaiseMemPool(int lNeeds, int nIdx = 0)
	{
		if ( lNeeds <= 0 )
			return;

		if ( nIdx >= nCOMP_MEM_POOL_COUNT || nIdx < 0 )
			return;

		if ( m_pMemPool[nIdx] == NULL )
			return;
		
		int nBlockCnt = (lNeeds / nCOMP_MEM_POOL_BLOCK_SIZE);
		
		if ( (lNeeds % nCOMP_MEM_POOL_BLOCK_SIZE) > 0 )
			nBlockCnt++;

		m_pMemPool[nIdx] = (char*) realloc(m_pMemPool[nIdx], m_lMemPoolSize[nIdx] + (nBlockCnt * nCOMP_MEM_POOL_BLOCK_SIZE));
	
		int lNewSize = (int)_msize(m_pMemPool[nIdx]);
		ASSERT(lNewSize == (m_lMemPoolSize[nIdx] + (nBlockCnt * nCOMP_MEM_POOL_BLOCK_SIZE)));
		m_lMemPoolSize[nIdx] = lNewSize;
	}

	void AllocMemPool(int lSize, int nIdx = 0)
	{
		if ( lSize <= 0 )
			return;

		if ( nIdx >= nCOMP_MEM_POOL_COUNT || nIdx < 0 )
			return;

		int nBlockCnt = (lSize / nCOMP_MEM_POOL_BLOCK_SIZE);

		if ( (lSize % nCOMP_MEM_POOL_BLOCK_SIZE) > 0 )
			nBlockCnt++;

		if ( m_pMemPool[nIdx] )
		{
			free(m_pMemPool[nIdx]);
			m_pMemPool[nIdx] = NULL;
		}

		m_pMemPool[nIdx] = (char*) malloc(nBlockCnt * nCOMP_MEM_POOL_BLOCK_SIZE);

		int lNewSize = (int)_msize(m_pMemPool[nIdx]);
		ASSERT(lNewSize == (nBlockCnt * nCOMP_MEM_POOL_BLOCK_SIZE));
		m_lMemPoolSize[nIdx] = lNewSize;
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
	//@}

	FILE* OpenFileMakeDir( char *pstrFileName, char *pstrOption );

	void decompressDirR(const char* src_filename, char *dest_filename );
	void compressDirR(const char* src_filename, char *dest_filename );
	void decompressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName );
	void decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char *pstrDestDir, char *pstrDestFileName );
	//@{ 2006/04/10 burumal
	int decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char** ppDestMem);
	//@}
	void compressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName );

	int CompressDir( char *pstrSourceDir, char *pstrDestDir, char *pstrFileName );
	int DecompressDir( char *pstrSourceDir, char *pstrDestDir, char *pstrFileName );

	//int PackingComressFiles( char *pstrSourceDir, char *pstrDestDir, char *pstrFileName);
	//void PackingComressFilesR( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath );

	//int UnpackingComressFiles( char *pstrSourceFile, char *pstrDestDir );

	//int GetSubDirFileCount( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath );
	//void GetSubDirFileCountR( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath );

	//void FindSubDirFiles( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath );
};

#endif