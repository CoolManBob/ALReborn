// MagUnpackManager.cpp: implementation of the CMagUnpackManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"
#include "aplib.h"
#include "MagDebug.h"
#include "MagUnpackManager.h"
#include "ApMemoryTracker.h"

CMagUnpackManager::ReturnMemory	CMagUnpackManager::m_sReturnMemory;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT	XORReturn( UINT data ) // 암호화 중요 자료 암호화 저장용
{
	// XOR Pattern  binary 0110100101101001
	//				hexa   0x6969
	// you can change this for your own binary mask
	// if client do not have correct mask , its extraction will be fail
	// use this Twice time to same value, you can get original value
	return data ^ 0x6969;
}

UINT	GetParity( BYTE * pdata , UINT size )
{
	// do nothing yet
	// return XORReturn( *( UINT * ) ( pdata + ( size * 2 / 3 ) ) );
	return 0;
}

CMagUnpackManager::CMagUnpackManager() : m_pMemoryBuffer( NULL )
{
	strcpy( m_strFilename , "" ); // The main packed file name.
	m_nFileCount	= 0		; // Total File Count;

	// 마고자 (2005-09-14 오후 6:58:46) : 초기메모리 할당
	CMagUnpackManager::m_sReturnMemory.GetMemory( 170000 );
}

CMagUnpackManager::~CMagUnpackManager()
{
	CloseFile();
	FreeMemory();
}

BOOL	CMagUnpackManager::AllocMemory( int nSize )
{
	ASSERT( nSize > 0 );
	ASSERT( m_pMemoryBuffer == NULL );

	FreeMemory();

	VERIFY( m_pMemoryBuffer = new BYTE[ nSize ] );
	if( m_pMemoryBuffer )
		return TRUE;
	else
		return FALSE;
}

void	CMagUnpackManager::FreeMemory()
{
	if( m_pMemoryBuffer )
	{
		delete [] m_pMemoryBuffer;
		m_pMemoryBuffer = NULL;
	}
}

BOOL CMagUnpackManager::SetFile(char *filename)
{
	// Determine this file is MPF file
	// and read the header and contruct the file db

	// open that file
	HANDLE	hfile;
	hfile = CreateFile(
		filename				,
		GENERIC_READ			,
		FILE_SHARE_READ			,
		0						,
		OPEN_EXISTING			,
		FILE_ATTRIBUTE_NORMAL	,
		0						);

	if( hfile == INVALID_HANDLE_VALUE)
	{
		//TRACE( "CMagUnpackManager::SetFile File Open Error(%s)" , filename );
		return FALSE;
	}

	// 파일 정보 초기화..
	CloseFile();
	strncpy( m_strFilename , filename , 256 );
	
	CMagPackHeaderInfo	*pHeaderInfo = NULL;
	DWORD	numofread;
	
	BYTE	headerBuffer [ 1 + 255 + 4 ];

	INT32	nHeaderSize = 0;

	// Read the header and check the right form
	if( 
		!ReadFile( 
			hfile,
			headerBuffer,
			MAGPACKFILEHEADERSIZE,
			&numofread,
			NULL)
		||
		numofread != MAGPACKFILEHEADERSIZE 
		)
	{
		TRACE( "CMagUnpackManager::SetFile File Read Error(%s)" , filename );
		CloseHandle( hfile );
		return FALSE;
	}
	nHeaderSize += numofread ;

	// Checking Header Text
	if( strcmp( ( char * ) headerBuffer , MAGPACKFILEHEADERTXT ) )
	{
		TRACE( "CMagUnpackManager::Header Incorrect(%s)" , filename );
		CloseHandle( hfile );
		return FALSE;
	}

	BYTE	filenamelength;
	int		nTotalSize = 0;
	int		count = 0;
	for(count = 0 ; ; ++count )
	{
		// Read String Length
		if( 
			!ReadFile( 
				hfile			,
				&filenamelength	,
				1				,
				&numofread		,
				NULL			)
			||
			numofread != 1 
			)
		{
			TRACE( "CMagUnpackManager::SetFile Filename length read error(%s)" , filename );
			CloseHandle( hfile );
			return FALSE;
		}
		nHeaderSize += numofread ;

		// Determine the end of list
		if( filenamelength == 0 )
		{
			// List is ended
			break;
		}

		// Read the filename + Size
		if( 
			!ReadFile( 
				hfile				,
				&headerBuffer		,
				filenamelength + 4	,
				&numofread			,
				NULL)
			||
			numofread !=  ( DWORD ) ( filenamelength + 4 )
			)
		{
			TRACE( "CMagUnpackManager::SetFile Filename length read error(%s)" , filename );
			CloseHandle( hfile );
			return FALSE;
		}
		nHeaderSize += numofread ;

		// Allocation new list
#ifdef MAGPACK_USE_ARRAY
		ASSERT( count < MAGPACK_MAX_ELEMENT_SIZE );
		pHeaderInfo = &m_arrayList[ count ];
		pHeaderInfo->Clean();
#else
		pHeaderInfo = new CMagPackHeaderInfo;
#endif

		if( pHeaderInfo == NULL )
		{
			TRACE( "CMagUnpackManager::SetFile List Memory Allocation%s)" , filename );
			CloseHandle( hfile );
			return FALSE;
		}
		// Fill the list
		pHeaderInfo->num = count;
		memset( ( void * ) pHeaderInfo->filename , 0 , 256 );
		memcpy( ( void * ) pHeaderInfo->filename , ( void * ) headerBuffer ,
			filenamelength );
		pHeaderInfo->size = 
			XORReturn ( *( UINT *) ( headerBuffer + filenamelength ) );
			// 암호화 제거

		nTotalSize += pHeaderInfo->size;

		// Add to the list
#ifdef MAGPACK_USE_ARRAY
		// do nothing..
#else
		m_listFile.AddTail( pHeaderInfo );
#endif
	}

	m_nFileCount = count;

	if( m_nFileCount )
	{
		// Packed 파일을 읽어들인다.

#ifdef MAGPACK_USE_ARRAY
		// do nothing
		if( m_nFileCount != MAGPACK_MAX_ELEMENT_SIZE )
		{
			TRACE( "데이타이상!\n" );
			return FALSE;
		}
#else
		AuNode< CMagPackHeaderInfo * >	*	pPos		= m_listFile.GetHeadNode();
#endif
		CMagPackHeaderInfo				*	pFileInfo	;

		// 메모리 할당..
		AllocMemory( nTotalSize );
		int	nOffset = 0;

		DWORD	numofread	;
		UINT	parity		;
		for( unsigned int i = 0 ; i < m_nFileCount ; ++i )
		{
#ifdef MAGPACK_USE_ARRAY
			pFileInfo = &m_arrayList[ i ];
#else
			pFileInfo = m_listFile.GetNext( pPos );
#endif
			pFileInfo->pPackedData	= m_pMemoryBuffer + nOffset;
			nOffset += pFileInfo->size;

			// Read Data;
			if( 
				!ReadFile( 
					hfile					,
					pFileInfo->pPackedData	,
					pFileInfo->size			,
					&numofread				,
					NULL					)
				||
				numofread != pFileInfo->size
				)
			{
				TRACE( "CMagUnpackManager::GetFileBinary File Read Error(%s)" , m_strFilename );
				return FALSE;
			}

			// Read Parity
			if( 
				!ReadFile( 
					hfile			,
					&parity			,
					sizeof( UINT )	,
					&numofread		,
					NULL			)
				||
				numofread != sizeof( UINT )
				)
			{
				TRACE( "CMagUnpackManager::GetFile File Read Error(%s)" , m_strFilename );
				return FALSE;
			}

			// Parity Check
			if( parity != GetParity( pFileInfo->pPackedData + 4 , pFileInfo->size ) )
			{
				TRACE( "CMagUnpackManager::GetFile File Read Error - Parity check(%s)" , m_strFilename );
				return FALSE;
			}
		}

		// 다 읽어 들였다..

	}

	// 파일 핸들 클로즈..
	if( hfile ) CloseHandle( hfile );

	return TRUE;
}

BOOL CMagUnpackManager::GetAllFiles(char *pdirdest , int (__cdecl *filecallback ) (unsigned int, unsigned int))
{
	// Get all files form this file
	// simple this function call GetFile for file count times
	if( m_nFileCount == 0 || strlen( m_strFilename ) == 0 )
	{
		TRACE( "CMagUnpackManager::GetAllFiles Did not Initialize!" );
		return FALSE;
	}

#ifdef MAGPACK_USE_ARRAY
	ASSERT( m_nFileCount <= MAGPACK_MAX_ELEMENT_SIZE );
#else
	AuNode< CMagPackHeaderInfo * >	*pPos		= m_listFile.GetHeadNode();
#endif
	CMagPackHeaderInfo				*pFileInfo	;

	for( unsigned int i = 0 ; i < m_nFileCount ; ++i )
	{
#ifdef MAGPACK_USE_ARRAY
		pFileInfo = &m_arrayList[ i ];
#else
		pFileInfo = m_listFile.GetNext( pPos );
#endif	
		if( filecallback ) filecallback( i + 1 , pFileInfo->size );

		GetFile( i , pdirdest );
	}
	// extract all file to specific directory
	return TRUE;
}

BOOL CMagUnpackManager::GetFile(UINT num, char *pdirdest)
{
	if( m_nFileCount == 0 || strlen( m_strFilename ) == 0 )
	{
		TRACE( "CMagUnpackManager::GetFile Did not Initialize!" );
		return FALSE;
	}
	
	if( num >= m_nFileCount )
	{
		TRACE( "CMagUnpackManager::GetFile Index Overflow( %d , total : %d )" , num , m_nFileCount );
		return FALSE;
	}

	CMagPackHeaderInfo				*pFileInfo	;

#ifdef MAGPACK_USE_ARRAY
	ASSERT( num < MAGPACK_MAX_ELEMENT_SIZE );
	pFileInfo	= &m_arrayList[ num ];
#else
	{
		AuNode< CMagPackHeaderInfo * >	*pPos		= m_listFile.GetHeadNode();
		UINT i = num;
		while( i-- )
		{
			m_listFile.GetNext( pPos );
		}

		ASSERT( NULL != pPos );

		pFileInfo = pPos->GetData();
	}
#endif

	ASSERT( NULL != pFileInfo->pPackedData );
	
	UINT	uPackedSize;

	uPackedSize = XORReturn( * ( ( UINT * ) pFileInfo->pPackedData ) ) ; // 암호화 제거

	// Memory Allocation
	BYTE *pPackedBuffer = pFileInfo->pPackedData + 4 ;
	BYTE *pOutputBuffer = CMagUnpackManager::m_sReturnMemory.GetMemory( uPackedSize );

	if( !pPackedBuffer || !pOutputBuffer )
	{
		TRACE( "CMagUnpackManager::GetFile Temporary Memory alloc error(%s)" , m_strFilename );
		if( pOutputBuffer ) delete [] pOutputBuffer;
		return FALSE;
	}

#ifndef WIN64
	// Depack
	int depacked_length = aP_depack_asm_fast( pPackedBuffer , pOutputBuffer );
	if( depacked_length != ( int ) uPackedSize )
	{
		TRACE( "CMagUnpackManager::GetFile Depack , Size incorrect(%s)" , m_strFilename );
		if( pOutputBuffer ) delete [] pOutputBuffer;
		return FALSE;
	}
#endif

	// Write the stream;
	char outfilename[ 1024 ];
	ZeroMemory(outfilename, sizeof(char) * 1024);
	if( pdirdest )
	{
		if( pdirdest[ strlen( pdirdest ) ] == '\\' )
				wsprintf( outfilename , "%s%s"		, pdirdest , pFileInfo->filename );
		else	wsprintf( outfilename , "%s\\%s"	, pdirdest , pFileInfo->filename );
	}
	else strncpy( outfilename , pFileInfo->filename , 1024 );
	HANDLE	houtfile;
	houtfile	= CreateFile( 
		outfilename				,
		GENERIC_WRITE			,
		0						,
		0						,
		CREATE_ALWAYS			,
		FILE_ATTRIBUTE_NORMAL	,
		0						);

	if( houtfile == INVALID_HANDLE_VALUE)
	{
		TRACE( "CMagUnpackManager::GetFile Write file open error(%s)" , m_strFilename );
		if( pOutputBuffer ) delete [] pOutputBuffer;
		return FALSE;
	}

	DWORD	numofread	;

	WriteFile(
		houtfile		,
		pOutputBuffer	,
		uPackedSize		,
		&numofread		,
		NULL			);
	
	CloseHandle( houtfile );

	return TRUE;
}

BOOL CMagUnpackManager::GetFile(char *filename, char *pdirdest)
{
	if( m_nFileCount == 0 || strlen( m_strFilename ) == 0 )
	{
		TRACE( "CMagUnpackManager::GetFile Did not Initialize!" );
		return FALSE;
	}
	
	// get file by name
#ifdef MAGPACK_USE_ARRAY
	ASSERT( m_nFileCount <= MAGPACK_MAX_ELEMENT_SIZE );
#else
	AuNode< CMagPackHeaderInfo * >	*pPos		= m_listFile.GetHeadNode();
#endif
	CMagPackHeaderInfo				*pFileInfo	;

	for( unsigned int i = 0 ; i < m_nFileCount ; ++i )
	{
#ifdef MAGPACK_USE_ARRAY
		pFileInfo = &m_arrayList[ i ];
#else
		pFileInfo = m_listFile.GetNext( pPos );
#endif	
		if( !strcmp( pFileInfo->filename , filename ) )
		{
			return GetFile( i , pdirdest );
		}
	}
	TRACE( "CMagUnpackManager::GetFile File not found in the list!" );
	return FALSE;
}

UINT CMagUnpackManager::GetFileCount()
{
	// return the file count;
	return m_nFileCount;
}

UINT CMagUnpackManager::GetFileBinary(UINT num, BYTE *&ptr)
{
	if( m_nFileCount == 0 || strlen( m_strFilename ) == 0 )
	{
		TRACE( "CMagUnpackManager::GetFileBinary Did not Initialize!" );
		return FALSE;
	}

	CMagPackHeaderInfo * pFileInfo;
#ifdef MAGPACK_USE_ARRAY
	// ASSERT( num < MAGPACK_MAX_ELEMENT_SIZE );
	if( num >= MAGPACK_MAX_ELEMENT_SIZE )
	{
		TRACE( "CMagUnpackManager::GetFileBinary ( num < MAGPACK_MAX_ELEMENT_SIZE ) 인덱스 이상\n" );
		return FALSE;
	}

	pFileInfo	= &m_arrayList[ num ];
#else
	{
		AuNode< CMagPackHeaderInfo * >	*pPos		= m_listFile.GetHeadNode();
		UINT i = num;
		while( i-- )
		{
			m_listFile.GetNext( pPos );
		}

		ASSERT( NULL != pPos );

		pFileInfo = pPos->GetData();
	}
#endif

	ASSERT( NULL != pFileInfo->pPackedData );

	UINT	uPackedSize;
	uPackedSize = XORReturn( * ( ( UINT * ) pFileInfo->pPackedData ) ) ; // 암호화 제거

	// Memory Allocation
	BYTE *pPackedBuffer = pFileInfo->pPackedData + 4 ;

	// 
	BYTE *pOutputBuffer = CMagUnpackManager::m_sReturnMemory.GetMemory( uPackedSize	);
	if( !pPackedBuffer || !pOutputBuffer )
	{
		TRACE( "CMagUnpackManager::GetFileBinary Temporary Memory alloc error(%s)" , m_strFilename );
		if( pOutputBuffer ) delete [] pOutputBuffer;
		return FALSE;
	}

#ifndef WIN64
	// Depack
	int depacked_length = aP_depack_asm_fast( pPackedBuffer , pOutputBuffer );
	if( depacked_length != ( int ) uPackedSize )
	{
		TRACE( "CMagUnpackManager::GetFileBinary Depack , Size incorrect(%s)" , m_strFilename );
		if( pOutputBuffer ) delete [] pOutputBuffer;
		return FALSE;
	}
#endif

	// if( pPackedBuffer ) delete [] pPackedBuffer;
	ptr = pOutputBuffer ; // Save the pointer

	return uPackedSize;
}
UINT CMagUnpackManager::GetFileBinary(char *filename, BYTE *&ptr)
{
	if( m_nFileCount == 0 || strlen( m_strFilename ) == 0 )
	{
		TRACE( "CMagUnpackManager::GetFileBinary Did not Initialize!" );
		return FALSE;
	}
	
	// get file by name
#ifdef MAGPACK_USE_ARRAY
	ASSERT( m_nFileCount <= MAGPACK_MAX_ELEMENT_SIZE );
	if( m_nFileCount > MAGPACK_MAX_ELEMENT_SIZE )
	{
		CHAR szTemp[128];
		sprintf(szTemp, "File Count Error (%d)", m_nFileCount);
		MessageBox(NULL, szTemp, "Error", MB_OK);
		TRACE( "CMagUnpackManager::GetFileBinary ( m_nFileCount < MAGPACK_MAX_ELEMENT_SIZE ) 인덱스 이상\n" );
		return FALSE;
	}
#else
	AuNode< CMagPackHeaderInfo * >	*pPos		= m_listFile.GetHeadNode();
#endif
	CMagPackHeaderInfo	*pFileInfo;

	for( unsigned int i = 0 ; i < m_nFileCount ; ++i )
	{
#ifdef MAGPACK_USE_ARRAY
		pFileInfo = &m_arrayList[ i ];
#else
		pFileInfo = m_listFile.GetNext( pPos );
#endif	
		if( !strcmp( pFileInfo->filename , filename ) )
		{
			return GetFileBinary( i , ptr );
		}
	}
	TRACE( "CMagUnpackManager::GetFileBinary File not found in the list!" );
	return FALSE;
}

BOOL	CMagUnpackManager::CloseFile		()
{
	// Initializing member values
	m_nFileCount	= 0		;

	strcpy( m_strFilename , "" );

	// 리스트 제거..
#ifdef MAGPACK_USE_ARRAY
	for( int i = 0 ; i < MAGPACK_MAX_ELEMENT_SIZE ; ++ i ) 
	{
		m_arrayList[ i ].Clean();
	}
#else
	while( m_listFile.GetHeadNode() )
	{
		delete m_listFile.GetHead();
		m_listFile.RemoveHead();
	}
#endif	


	return TRUE;
}

int		CMagUnpackManager::GetFileNum		( char *filename )
{
	if( m_nFileCount == 0 || strlen( m_strFilename ) == 0 )
	{
		TRACE( "CMagUnpackManager::GetFileNum Did not Initialize!" );
		return -1;
	}
	
	// get file by name
#ifdef MAGPACK_USE_ARRAY
	ASSERT( m_nFileCount <= MAGPACK_MAX_ELEMENT_SIZE );
#else
	AuNode< CMagPackHeaderInfo * >	*pPos		= m_listFile.GetHeadNode();
#endif
	CMagPackHeaderInfo	*pFileInfo;

	for( int i = 0 ; i < ( int ) m_nFileCount ; ++ i )
	{
#ifdef MAGPACK_USE_ARRAY
		pFileInfo = &m_arrayList[ i ];
#else
		pFileInfo = m_listFile.GetNext( pPos );
#endif	
		if( !strcmp( pFileInfo->filename , filename ) )
		{
			// 찾았다..
			return i;
		}
	}

	// 여기까지 오면 안된다..
	ASSERT( !"이 현상이 발생하기전에 체크를 해줘야한다.." );
	return -1;
}

// Get File Size..
BOOL	CMagUnpackManager::GetFileSize		( UINT	num	, UINT * pSize )
{
	if( num == -1 ) return FALSE;

	if( num >= m_nFileCount ) return FALSE;

#ifdef MAGPACK_USE_ARRAY
	CMagPackHeaderInfo	*pHeader = &m_arrayList[ num ];
#else
	CMagPackHeaderInfo	*pHeader = m_listFile[ num ];
#endif	
	
	ASSERT( NULL != pHeader );
	if( pHeader )
	{
		if( pSize ) *pSize = pHeader->size;
		return TRUE;
	}
	else return FALSE;
}

char *	CMagUnpackManager::GetFileName		( UINT num			)
{
	if( num == -1 ) return FALSE;

	if( num >= m_nFileCount ) return FALSE;

#ifdef MAGPACK_USE_ARRAY
	CMagPackHeaderInfo	*pHeader = &m_arrayList[ num ];
#else
	CMagPackHeaderInfo	*pHeader = m_listFile[ num ];
#endif	

	if( pHeader )
		return pHeader->filename;
	else
		return NULL;
}
