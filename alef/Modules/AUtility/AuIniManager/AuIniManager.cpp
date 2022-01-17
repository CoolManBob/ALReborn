/*****************************************************************
*   Comment  : 
*   Date&Time : 2002-12-05, 오후 7:53
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ApBase.h"
#include "AuIniManager.h"
#include "AuMD5Encrypt.h"

#include "ApMemoryTracker.h"

#define CRLF						"\r\n"

#define HASH_KEY_STRING				"1111"

//@{ 2006/07/28 burumal
int g_nCRLELength = 0;
//@}

int	print_compact_float( char * pStr , float fValue )
{
	sprintf( pStr , "%f" , fValue );

	int nLength = ( int ) strlen( pStr );

	for( int i = nLength - 1 ; i > 0 ; i -- )
	{
		if( pStr[ i ] == '0' )
		{
			pStr [ i ] = '\0';
		}
		else
		if( pStr[ i ] == '.' )
		{
			pStr [ i ] = '\0';
			break;
		}
		else break;
	}
	nLength = ( int ) strlen( pStr );
	return nLength;
}

int print_compact_format( char * pStr , const char * pFmt , ... )
{
	va_list	ap;

	int nSum = 0;

	const char *	pCurrentFmt = pFmt;
	char *			pCurrentStr = pStr;

	bool bArgument = false;

	va_start( ap , pFmt );
	/*
	int nValue = va_arg( ap , int );
	nValue = va_arg( ap , int );
	nValue = va_arg( ap , int );
	double fValue = va_arg( ap , double );
	fValue = va_arg( ap , double );
	fValue = va_arg( ap , double );
	*/

	while( *pCurrentFmt != '\0' )
	{
		if( bArgument )
		{
			switch( *pCurrentFmt )
			{
			case 'f':
			case 'F':
				{
					float fValue = ( float ) ( va_arg( ap , double ) );
					pCurrentStr += print_compact_float( pCurrentStr , fValue );
					break;
				}
			case 'd':
			case 'D':
				{
					int nValue = va_arg( ap , int );
					pCurrentStr += sprintf( pCurrentStr , "%d" , nValue );
					break;
				}
			case 's':
			case 'S':
				{
					const char * pValue = va_arg( ap , const char * );
					pCurrentStr += sprintf( pCurrentStr , "%s" , pValue );
					break;
				}
			case 'x':
			case 'X':
				{
					int nValue = va_arg( ap , int );
					pCurrentStr += sprintf( pCurrentStr , "%x" , nValue );
					break;
				}
			}
			bArgument = false;
		}
		else
		if( *pCurrentFmt == '%' )
		{
			bArgument = true;
		}
		else
		{
			bArgument = false;
			*pCurrentStr = *pCurrentFmt;
			pCurrentStr++;
			*pCurrentStr = '\0';
		}

		pCurrentFmt ++;
	}

	va_end( ap );

	int nLength = ( int ) strlen( pStr );
	return nLength;
}

void throw_exception( LPCSTR szFmt , ... )
{
	static char szOutBuff [ 1024 ] ;

	va_list  args ;

	va_start ( args , szFmt ) ;

	wvsprintfA ( szOutBuff , szFmt , args ) ;

	va_end ( args ) ;

	throw szOutBuff ;
}

const char * AuIniManagerA::GetKeyNameTable( int nIndex )
{
	ASSERT( nIndex < ( int ) m_vecKeyTable.size() );

	return m_vecKeyTable.at( nIndex ).c_str();
}

int		AuIniManagerA::GetKeyIndex	( const char * pStr )
{
	int nOffset = 0;
	for( vector< string >::iterator iTer = m_vecKeyTable.begin();
		iTer != m_vecKeyTable.end();
		iTer ++ , nOffset ++ )
	{
		string *pKey = &*iTer;

		if( !strncmp( pStr , pKey->c_str() , AUINIMANAGER_MAX_NAME ) )
		{
			// 빙고.
			return nOffset;
		}
	}

	// 요까지 오면 없는겨..
	// 키인덱스 추가함

	m_vecKeyTable.push_back( pStr );
	return nOffset;
}

AuIniManagerA::AuIniManagerA():
	m_eMode			( AUINIMANAGER_MODE_NORMAL	),
	m_eType			( AUINIMANAGER_TYPE_NORMAL	),
	m_stSection		( NULL						),
	lSectionNum		( 0							),
	lHalfSectionNum	( 0							),
	bIsSectionNumOdd( false						),
	m_eProcessMode	( AuIniManagerA::TXT		)
{
	m_lPartNum = 256;	
	m_plPartIndices = new int[m_lPartNum];	

	//@{ 2006/07/28 burumal
	g_nCRLELength = (int) strlen(CRLF);
	//@}

	Initialize();
}

AuIniManagerA::~AuIniManagerA()
{
	if ( m_stSection != NULL )
	{
		for ( int i = 0 ; i < lSectionNum ; ++i )
		{
			if ( m_stSection[i].stKey != NULL ) 
			{
				delete [] m_stSection[i].stKey;
				m_stSection[i].stKey = NULL;
			}
		}

		delete [] m_stSection;
		m_stSection = NULL;
	}

	if ( m_plPartIndices != NULL )
	{
		delete [] m_plPartIndices;
		m_plPartIndices = NULL;
	}
}

/*****************************************************************
*   Function : Initialize
*   Comment  : 
*   Date&Time : 2002-12-05, 오후 8:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AuIniManagerA::Initialize( )
{
	ClearAllSectionKey();		
}

/*****************************************************************
*   Function : SetPath
*   Comment  : 
*   Date&Time : 2002-12-05, 오후 7:54
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AuIniManagerA::SetPath(	const char* pszPath	)
{
	m_strPathName = pszPath;
	//	Initialize();		-			2003_01_09 수정 
}

/*****************************************************************
*   Function : 
*   Comment  : 
*   Date&Time : 2002-12-05, 오후 8:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AuIniManagerA::ReadFile	( int lIndex, BOOL bDecryption )
{
	//변수들을 초기화
	if ( m_strPathName.size() == 0 )
		return FALSE;

	ClearAllSectionKey();

	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		return FromFile(lIndex, bDecryption);
	}
	else
	{
		//@{ 2006/09/05 burumal
		//return FromMemory(lIndex, bDecryption);
		return FromMemory(bDecryption);
		//@}
	}
}	

//@{ 2006/09/05 burumal
// 이 함수는 AUINIMANAGER_TYPE_PART_INDEX 타입으로 사용되지 않으므로 혼란을 피하기 위해 변경함
//BOOL	AuIniManagerA::FromMemory( int lIndex, BOOL bDecryption)
BOOL	AuIniManagerA::FromMemory( BOOL bDecryption)
//@}
{
	int	 lMaxCharacter = AUINIMANAGER_MAX_NAME + AUINIMANAGER_MAX_KEYVALUE + 1;

	char szDummy[AUINIMANAGER_MAX_NAME + AUINIMANAGER_MAX_KEYVALUE + 1];

	int	lFileSectionNum								= 0		;		//Section이 몇개인가. 

	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM]	= {0,}	;		//각 Section당 Key가 몇개씩인가 

	int		i = 0;
	int		lStartPos = 0;
	int		lEndPos	= 0;
	__int64	lCurPos = 0;

	size_t iOffsetBytes = 0;
	size_t iOffset		= 0;

	//File 열기
	FILE*		file;
	char*		pBuffer;
	size_t		iFileSize;
	bool		bFileReadResult;

	bFileReadResult = true;
	pBuffer = NULL;
	iFileSize = 0;

	if( !pBuffer )
	{
		file = fopen( m_strPathName.c_str(), "rb" );

		if( file )
		{
			fseek( file, 0, SEEK_END );
			iFileSize = ftell( file );
			fseek( file, 0, SEEK_SET );

			if( iFileSize )
			{
				pBuffer = new char[iFileSize + 1];

				//@{ 2006/11/22 burumal
				//memset(pBuffer, 0, iFileSize + 1);
				//@}

				if( fread( pBuffer, 1, iFileSize, file ) != iFileSize )
				{
					bFileReadResult = false;
				}

				pBuffer[iFileSize] = 0;
			}
			else
			{
				bFileReadResult = false;
			}

			fclose( file );
		}
		else
		{
			return FALSE;
		}
	}

	

	if ( bFileReadResult == false )
		return FALSE;

	if ( bDecryption )
	{
		AuMD5Encrypt MD5;

#ifdef _AREA_CHINA_
		if (!MD5.DecryptString(MD5_HASH_KEY_STRING, pBuffer, (unsigned long)iFileSize))
#else
		if (!MD5.DecryptString(HASH_KEY_STRING, pBuffer, (unsigned long)iFileSize))
#endif
		{
			delete [] pBuffer;
			return FALSE;
		}
	}
	
	//@{ 2006/09/05 burumal
	/*
	// Parn님 작업 Part Index type일 경우는 앞에서 index 를 읽어서 그 부분으로 넘어가서 읽는다.
	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		if ( !(iOffset = ReadPartIndices( pBuffer + iOffsetBytes )) || lIndex >= m_lPartNum )
		{
			delete [] pBuffer;
			return FALSE;
		}

		iOffsetBytes += iOffset;

		lStartPos = m_plPartIndices[lIndex];

		if ( lIndex + 1 < m_lPartNum )
		{
			lEndPos = m_plPartIndices[lIndex + 1];
		}		
	}
	// Parn님 작업
	*/
	//@}

	int nDummyLen;

	BOOL bIniReadFlag = TRUE;
	int	nKeyIndexCount = 0;

	if ( bIniReadFlag == TRUE )
	{
		// lSectionNum과 lKeyNum 구하기
		while( iOffsetBytes < iFileSize )
		{
			iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
			
			//@{ 2006/07/28 burumal
			//iOffset += strlen(CRLF);

			if( iOffsetBytes + iOffset + g_nCRLELength <= iFileSize )
				iOffset += g_nCRLELength;
			//@}
			
			//@{ 2006/08/31 burumal
			//memset(szDummy, 0, sizeof(szDummy));
			//@}

			if( iOffsetBytes + iOffset > iFileSize )
			{
				throw_exception( "파일의 마지막이 깨짐 : %s", m_strPathName.c_str() );
			}

			memcpy_s(szDummy, sizeof(szDummy), pBuffer + iOffsetBytes, iOffset);

			//@{ 2006/08/31 burumal
			if ( iOffset < sizeof(szDummy) )
			{
				szDummy[iOffset] = '\0';
				if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
				{
					szDummy[ iOffset - 1 ] = 0;
				}
			}
			else
			{
				szDummy[sizeof(szDummy)-1] = '\0';
				throw_exception( "비정상적인 파일 정보, 너무김\n %s : %s", m_strPathName.c_str(), szDummy );
			}
			//@}

			iOffsetBytes += iOffset;

			//@{ 2006/08/31 burumal
			nDummyLen = (int)strlen(szDummy);
			//@}

			//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
			if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
			{
				szDummy[strcspn(szDummy, ";")] = '\0';
				nDummyLen = (int)strlen(szDummy);
			}
			
			//Section발견 
			//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
			if ( (int)strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
			{
				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
					++lFileSectionNum;
				else
					throw_exception( "파일 색션 최대갯수 초과 : %s", m_strPathName.c_str() );
			}		
			//Key발견
			else
			//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
			if ( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
				++lFileKeyNum[lFileSectionNum - 1];
			else if( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 )
				nKeyIndexCount ++;

			if (lEndPos)
			{
				if ( (INT32) iOffsetBytes >= lEndPos)
					break;
			}
		}

		//메모리 할당 
		lSectionNum = lFileSectionNum;
		if(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum)
			m_stSection = new stIniSection[ lSectionNum ];
		else
		{
			ASSERT(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum);
			return FALSE;
		}

		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		#ifdef _DEBUG
			/*
			sprintf(pDbgMsg, "Ini File[M] ===> (%s)\n", m_pszPathName);
			OutputDebugStr(pDbgMsg);

			sprintf(pDbgMsg, "\t\t SectionNum: %d\n", lSectionNum);
			OutputDebugStr(pDbgMsg);
			*/
		#endif

		for ( i = 0 ; i < lSectionNum ; ++i )
		{
			m_stSection[i].lKeyNum = 0;

			if ( lFileKeyNum[i] )
				m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
			else
				m_stSection[i].stKey = NULL;

			#ifdef _DEBUG
				/*
				sprintf(pDbgMsg, "\t\t KeyNum[%d]: %d\n", i, lFileKeyNum[i]);
				OutputDebugStr(pDbgMsg);
				*/
			#endif
		}
	}

	// 키인덱스가 있는경우는 , 인덱스를 사용하는 경우 밖에 없으니까 여기서 플래그 확인..
	bool bUseKeyIndex = false;
	if( nKeyIndexCount ) bUseKeyIndex = true;

	// 버퍼의 위치를 처음으로 다시 초기화
	iOffsetBytes = 0;
		
	//파일로 부터 입력 
	lFileSectionNum = 0;
	
	//@{ 2006/07/27 burumal
	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
	//@}

	while( iOffsetBytes < iFileSize )
	{
		// CRLF가 있는 곳을 찾는다.
		iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
		
		//@{ 2006/07/28 burumal
		//iOffset += strlen(CRLF);
		if( iOffsetBytes + iOffset + g_nCRLELength <= iFileSize )
			iOffset += g_nCRLELength;
		//@}

		// CRLF가 발견된 곳까지 내용을 szDummy에 복사한다.
		
		//@{ 2006/08/31 burumal
		//memset(szDummy, 0, sizeof(szDummy));
		//@}

		memcpy_s(szDummy, sizeof(szDummy), pBuffer + iOffsetBytes, iOffset);

		//@{ 2006/08/31 burumal
		if ( iOffset < sizeof(szDummy) )
		{
			szDummy[iOffset] = '\0';
			if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
			{
				szDummy[ iOffset - 1 ] = 0;
			}
		}
		//@}
		
		iOffsetBytes += iOffset;

		//@{ 2006/08/31 burumal
		nDummyLen = (int) strlen(szDummy);
		//@}
		if ( nDummyLen == 0 )
			continue;
		
		//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
		if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
		{
			szDummy[strcspn(szDummy, ";")] = '\0';			
			nDummyLen = (int) strlen(szDummy);
		}

		//i = strlen(szDummy) - 1;
		i = nDummyLen - 1;

		while ( i >= 0 && (szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' || szDummy[i] == 13 ) )
		{
			szDummy[i] = '\0';
			--i;

			//@{ 2006/08/31 burumal
			--nDummyLen;
			//@}
		}	
		
		//Section 발견 
		//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
		if ( (int) strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
		{
			if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
						szDummy, 
						nDummyLen,
						1, 
						(int) strcspn( szDummy, "]" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy(m_stSection[lFileSectionNum].SectionName, "" );
			}

			++lFileSectionNum;
		}
		//Key발견 
		else 
		//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
		if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
		{
			//Key, Value입력하기
			char	strKeyName[ AUINIMANAGER_MAX_NAME ];

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}
			
			// 여기서 키 인덱스를 사용하는경우 
			// 그냥 atoi만 하는 것으로 수정함.
			if( bUseKeyIndex )
			{
				int	nKeyIndex = atoi( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}
			else
			{
				int	nKeyIndex = GetKeyIndex( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}

			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
			}

			++m_stSection[lFileSectionNum-1].lKeyNum;
			++lFileKeyNum[lFileSectionNum-1];
		}
		else if( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 && bUseKeyIndex )
		{
			// 키 섹션을 여기서 입력받아 vector에 정리함.
			//Key, Value입력하기
			char	strKeyName	[ AUINIMANAGER_MAX_NAME		] = "";
			char	strKeyValue	[ AUINIMANAGER_MAX_KEYVALUE	] = "";

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}

			if ( !StringMid( strKeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy( strKeyValue , "" );
			}

			if( strlen( strKeyName ) && strlen( strKeyValue ) )
			{
				int	nKeyIndex = atoi( strKeyName );
				int nKeyIndexReturned;

				nKeyIndexReturned = GetKeyIndex( strKeyValue );

				ASSERT( nKeyIndex == nKeyIndexReturned );
			}
		}

		if ( lEndPos )
		{
			if ( (INT32) iOffsetBytes >= lEndPos)
				break;
		}
	}

	delete [] pBuffer;

	if( lSectionNum == 0 )
		return FALSE;

	return TRUE;
}

BOOL AuIniManagerA::ParseMemory( char* pBuffer, unsigned long nBufferSize, BOOL bDecryption )
{
	if( !pBuffer ) return FALSE;

	if ( bDecryption )
	{
		AuMD5Encrypt MD5;

		if (!MD5.DecryptString(HASH_KEY_STRING, pBuffer, nBufferSize))
		{
			return FALSE;
		}
	}
	
	int nDummyLen;

	BOOL bIniReadFlag = TRUE;
	int	nKeyIndexCount = 0;
	size_t iOffsetBytes = 0;
	size_t iOffset		= 0;

	int	lFileSectionNum								= 0		;		//Section이 몇개인가. 

	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM]	= {0,}	;		//각 Section당 Key가 몇개씩인가 

	int		i = 0;
	int		lStartPos = 0;
	int		lEndPos	= 0;
	__int64	lCurPos = 0;

	char szDummy[AUINIMANAGER_MAX_NAME + AUINIMANAGER_MAX_KEYVALUE + 1] = { 0, };


	if ( bIniReadFlag == TRUE )
	{
		// lSectionNum과 lKeyNum 구하기
		while( iOffsetBytes < nBufferSize )
		{
			iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
			
			//@{ 2006/07/28 burumal
			//iOffset += strlen(CRLF);
			iOffset += g_nCRLELength;
			//@}
			
			//@{ 2006/08/31 burumal
			//memset(szDummy, 0, sizeof(szDummy));
			//@}

			memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

			//@{ 2006/08/31 burumal
			if ( iOffset < sizeof(szDummy) )
			{
				szDummy[iOffset] = '\0';
				if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
				{
					szDummy[ iOffset - 1 ] = 0;
				}
			}
			//@}

			iOffsetBytes += iOffset;

			//@{ 2006/08/31 burumal
			nDummyLen = (int)strlen(szDummy);
			//@}

			//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
			if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
			{
				szDummy[strcspn(szDummy, ";")] = '\0';
				nDummyLen = (int)strlen(szDummy);
			}
			
			//Section발견 
			//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
			if ( (int)strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
			{
				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
					++lFileSectionNum;
			}		
			//Key발견
			else
			//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
			if ( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
				++lFileKeyNum[lFileSectionNum - 1];
			else if( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 )
				nKeyIndexCount ++;

			if (lEndPos)
			{
				if ( (INT32) iOffsetBytes >= lEndPos)
					break;
			}
		}

		//메모리 할당 
		lSectionNum = lFileSectionNum;
		if(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum)
			m_stSection = new stIniSection[ lSectionNum ];
		else
		{
			ASSERT(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum);
			return FALSE;
		}

		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		#ifdef _DEBUG
			/*
			sprintf(pDbgMsg, "Ini File[M] ===> (%s)\n", m_pszPathName);
			OutputDebugStr(pDbgMsg);

			sprintf(pDbgMsg, "\t\t SectionNum: %d\n", lSectionNum);
			OutputDebugStr(pDbgMsg);
			*/
		#endif

		for ( i = 0 ; i < lSectionNum ; ++i )
		{
			m_stSection[i].lKeyNum = 0;

			if ( lFileKeyNum[i] )
				m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
			else
				m_stSection[i].stKey = NULL;

			#ifdef _DEBUG
				/*
				sprintf(pDbgMsg, "\t\t KeyNum[%d]: %d\n", i, lFileKeyNum[i]);
				OutputDebugStr(pDbgMsg);
				*/
			#endif
		}
	}

	// 키인덱스가 있는경우는 , 인덱스를 사용하는 경우 밖에 없으니까 여기서 플래그 확인..
	bool bUseKeyIndex = false;
	if( nKeyIndexCount ) bUseKeyIndex = true;

	// 버퍼의 위치를 처음으로 다시 초기화
	iOffsetBytes = 0;
		
	//파일로 부터 입력 
	lFileSectionNum = 0;
	
	//@{ 2006/07/27 burumal
	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
	//@}

	while( iOffsetBytes < nBufferSize )
	{
		// CRLF가 있는 곳을 찾는다.
		iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
		
		//@{ 2006/07/28 burumal
		//iOffset += strlen(CRLF);
		iOffset += g_nCRLELength;
		//@}

		// CRLF가 발견된 곳까지 내용을 szDummy에 복사한다.
		
		//@{ 2006/08/31 burumal
		//memset(szDummy, 0, sizeof(szDummy));
		//@}

		memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

		//@{ 2006/08/31 burumal
		if ( iOffset < sizeof(szDummy) )
		{
			szDummy[iOffset] = '\0';
			if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
			{
				szDummy[ iOffset - 1 ] = 0;
			}
		}
		//@}
		
		iOffsetBytes += iOffset;

		//@{ 2006/08/31 burumal
		nDummyLen = (int) strlen(szDummy);
		//@}
		if ( nDummyLen == 0 )
			continue;
		
		//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
		if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
		{
			szDummy[strcspn(szDummy, ";")] = '\0';			
			nDummyLen = (int) strlen(szDummy);
		}

		//i = strlen(szDummy) - 1;
		i = nDummyLen - 1;

		while ( i >= 0 && (szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' || szDummy[i] == 13 ) )
		{
			szDummy[i] = '\0';
			--i;

			//@{ 2006/08/31 burumal
			--nDummyLen;
			//@}
		}	
		
		//Section 발견 
		//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
		if ( (int) strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
		{
			if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
						szDummy, 
						nDummyLen,
						1, 
						(int) strcspn( szDummy, "]" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy(m_stSection[lFileSectionNum].SectionName, "" );
			}

			++lFileSectionNum;
		}
		//Key발견 
		else 
		//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
		if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
		{
			//Key, Value입력하기
			char	strKeyName[ AUINIMANAGER_MAX_NAME ];

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}
			
			// 여기서 키 인덱스를 사용하는경우 
			// 그냥 atoi만 하는 것으로 수정함.
			if( bUseKeyIndex )
			{
				int	nKeyIndex = atoi( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}
			else
			{
				int	nKeyIndex = GetKeyIndex( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}

			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
			}

			++m_stSection[lFileSectionNum-1].lKeyNum;
			++lFileKeyNum[lFileSectionNum-1];
		}
		else if( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 && bUseKeyIndex )
		{
			// 키 섹션을 여기서 입력받아 vector에 정리함.
			//Key, Value입력하기
			char	strKeyName	[ AUINIMANAGER_MAX_NAME		] = "";
			char	strKeyValue	[ AUINIMANAGER_MAX_KEYVALUE	] = "";

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}

			if ( !StringMid( strKeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy( strKeyValue , "" );
			}

			if( strlen( strKeyName ) && strlen( strKeyValue ) )
			{
				int	nKeyIndex = atoi( strKeyName );
				int nKeyIndexReturned;

				nKeyIndexReturned = GetKeyIndex( strKeyValue );

				ASSERT( nKeyIndex == nKeyIndexReturned );
			}
		}

		if ( lEndPos )
		{
			if ( (INT32) iOffsetBytes >= lEndPos)
				break;
		}
	}

	delete [] pBuffer;

	if( lSectionNum == 0 )
		return FALSE;

	return TRUE;
}

// BOOL	AuIniManagerA::FromFile( int lIndex, BOOL bDecryption)
// {
// 	FILE* pFile;
// 	int lMaxCharacter = AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1;
// 	char szDummy[AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1];
// 	int i = 0;
// 	bool bSuccessKey = false, bSuccessValue = false;
// 	int	lFileSectionNum		= 0					;		//Section이 몇개인가. 
// 	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM];		//각 Section당 Key가 몇개씩인가 
// 	int lStartPos = 0;
// 	int lEndPos = 0;
// 	__int64 lCurPos = 0;
// 
// 	//@{ 2006/07/27 burumal
// 	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
// 	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
// 	//@}
// 	
// 	//File 열기 
// 	if ( (pFile = fopen( m_pszPathName , "r" )) == NULL )	//File Open
// 		return FALSE;
// 
// 	// Parn님 작업 Part Index type일 경우는 앞에서 index를 읽어서 그 부분으로 넘어가서 읽는다.
// 	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
// 	{
// 		if ( !ReadPartIndices( pFile ) || lIndex >= m_lPartNum )
// 		{
// 			fclose( pFile );
// 			return FALSE;
// 		}
// 
// 		lStartPos = m_plPartIndices[lIndex];
// 
// 		if ( fseek( pFile, lStartPos, SEEK_SET) )
// 		{
// 			fclose( pFile );
// 			return FALSE;
// 		}
// 
// 		if ( lIndex + 1 < m_lPartNum)
// 			lEndPos = m_plPartIndices[lIndex + 1];
// 	}
// 	// Parn님 작업
// 
// 	int nDummyLen;
// 
// 	BOOL bIniReadFlag = TRUE;
// 
// 	if ( bIniReadFlag )
// 	{
// 		// lSectionNum과 lKeyNum 구하기 
// 		while ( !feof(pFile) )
// 		{
// 			//@{ 2006/08/31 burumal
// 			/*
// 			fgets( szDummy, lMaxCharacter, pFile );
// 
// 			if ( strcspn(szDummy, ";" ) < strlen(szDummy) )
// 				szDummy[strcspn(szDummy, ";")] = '\0';
// 
// 			//Section발견 
// 			if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
// 			{
// 				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
// 					++lFileSectionNum;
// 			}		
// 			//Key발견 
// 			else 
// 			if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
// 				++lFileKeyNum[lFileSectionNum-1];
// 
// 			if ( lEndPos )
// 			{
// 				if ( fgetpos( pFile, &lCurPos ) )
// 					break;
// 
// 				if ( lCurPos >= lEndPos )
// 					break;
// 			}
// 			*/
// 			fgets(szDummy, lMaxCharacter, pFile);
// 			
// 			nDummyLen = (int) strlen(szDummy);
// 
// 			if ( (int) strcspn(szDummy, ";") < nDummyLen )
// 			{
// 				szDummy[strcspn(szDummy, ";")] = '\0';
// 				nDummyLen = (int) strlen(szDummy);
// 			}
// 
// 			//Section발견
// 			if ( (int) strcspn(szDummy, "[" ) < nDummyLen )
// 			{
// 				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
// 					++lFileSectionNum;
// 			}
// 			//Key발견
// 			else
// 			if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
// 				++lFileKeyNum[lFileSectionNum - 1];
// 
// 			if ( lEndPos )
// 			{
// 				if ( fgetpos( pFile, &lCurPos ) )
// 					break;
// 
// 				if ( lCurPos >= lEndPos )
// 					break;
// 			}
// 			//@}
// 		}
// 
// 		//메모리 할당
// 		lSectionNum = lFileSectionNum;
// 		m_stSection = new stIniSection[lSectionNum];
// 
// 		//@{ 2006/11/22 burumal
// 		lHalfSectionNum = lSectionNum / 2;
// 		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
// 		//@}
// 
// 	#ifdef _DEBUG
// 		/*
// 		sprintf(pDbgMsg, "Ini File[F] ===> (%s) [%d]\n", m_pszPathName, lIndex);
// 		OutputDebugStr(pDbgMsg);
// 
// 		sprintf(pDbgMsg, "\t\t SectionNum: %d\n", lSectionNum);
// 		OutputDebugStr(pDbgMsg);
// 		*/
// 	#endif
// 
// 		for ( i = 0 ; i < lSectionNum ; ++i )
// 		{
// 			m_stSection[i].lKeyNum = 0;
// 
// 			if ( lFileKeyNum[i] )
// 				m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
// 			else
// 				m_stSection[i].stKey = NULL;
// 
// 		#ifdef _DEBUG		
// 			/*
// 			TRACE("\t\t KeyNum[%d]: %d\n", i, lFileKeyNum[i]);
// 			*/
// 		#endif
// 		}
// 	}
// 
// 	fseek(pFile, lStartPos, SEEK_SET);
// 
// 	//파일로 부터 입력 
// 	lFileSectionNum = 0;
// 	
// 	//@{ 2006/07/28 burumal
// 	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
// 	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
// 	//@}
// 
// 	while( !feof(pFile) )
// 	{
// 		fgets( szDummy, lMaxCharacter, pFile );
// 
// 		//@{ 2006/08/31 burumal
// 		nDummyLen = (int) strlen(szDummy);
// 		//@}
// 
// 		//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
// 		if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
// 		{
// 			szDummy[strcspn(szDummy, ";")] = '\0';
// 			nDummyLen = (int) strlen(szDummy);
// 		}
// 
// 		//i = (int)strlen(szDummy) - 1;
// 		i = nDummyLen - 1;
// 
// 		while(szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ')
// 		{
// 			szDummy[i] = '\0';
// 			--i;
// 
// 			//@{ 2006/08/31 burumal
// 			--nDummyLen;
// 			//@}
// 		}
// 
// 		//Section 발견 
// 		//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
// 		if ( (int) strcspn(szDummy, "[" ) < nDummyLen )
// 		{
// 			if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
// 						szDummy, 
// 						nDummyLen,
// 						1, 
// 						(int)strcspn( szDummy, "]" ) - 1, 
// 						AUINIMANAGER_MAX_NAME ) )
// 			{
// 				strcpy(m_stSection[lFileSectionNum].SectionName, "" );
// 			}
// 			
// 			++lFileSectionNum;
// 		}
// 		//Key발견 
// 		else
// 		//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
// 		if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
// 		{
// 			//Key, Value입력하기
// 			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyName, 
// 						szDummy, 
// 						nDummyLen,
// 						0, 
// 						(int)strcspn( szDummy, "=" ) - 1, 
// 						AUINIMANAGER_MAX_NAME ) )
// 			{
// 				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyName, "" );
// 			}
// 			
// 			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
// 						szDummy, 
// 						nDummyLen,
// 						(int)strcspn( szDummy, "=" ) + 1, 
// 						//(int)strlen( szDummy ) - 1 , 
// 						nDummyLen - 1,
// 						AUINIMANAGER_MAX_NAME ) )
// 			{
// 				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
// 			}
// 			
// 			++m_stSection[lFileSectionNum-1].lKeyNum;
// 			++lFileKeyNum[lFileSectionNum-1];
// 		}
// 
// 		if ( lEndPos )
// 		{
// 			if ( fgetpos( pFile, &lCurPos ) )
// 				break;
// 
// 			if ( lCurPos >= lEndPos )
// 				break;
// 		}
// 	}
// 	
// 	fclose(pFile);											//File Close
// 
// 	if( lSectionNum == 0 )	return FALSE;
// 
// 	return FALSE;	
// }

BOOL	AuIniManagerA::FromFile( int lIndex, BOOL bDecryption)
{
	FILE* pFile;
	int lMaxCharacter = AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1;	
	char* szDummyPtr;
	int i = 0;
	bool bSuccessKey = false, bSuccessValue = false;
	int	lFileSectionNum		= 0					;		//Section이 몇개인가. 
	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM];		//각 Section당 Key가 몇개씩인가 
	int lStartPos = 0;
	int lEndPos = 0;
	__int64 lCurPos = 0;

	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));

	char* pSrcReadData = 0;
	size_t fileSize = 0;
	int nDummyLen = 0;	
	int nTotalDataSize = 0;

	//if( m_strPathName == "Ini\\Obj01817.ini" )
	//	ASSERT(FALSE);

	char * orgStream = pSrcReadData;


	//File 열기 
	if ( (pFile = fopen( m_strPathName.c_str() , "r" )) == NULL )	//File Open
		return FALSE;

	// Parn님 작업 Part Index type일 경우는 앞에서 index를 읽어서 그 부분으로 넘어가서 읽는다.
	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		if ( !ReadPartIndices( pFile ) || lIndex >= m_lPartNum )
		{
			fclose( pFile );
			return FALSE;
		}

		lStartPos = m_plPartIndices[lIndex];

		if ( fseek( pFile, lStartPos, SEEK_SET) )
		{
			fclose( pFile );
			return FALSE;
		}

		if ( lIndex + 1 < m_lPartNum)
			lEndPos = m_plPartIndices[lIndex + 1];
	}
	// Parn님 작업

	if ( lEndPos == 0 )
	{
		fseek(pFile, 0, SEEK_END);
		lEndPos = ftell(pFile);
		fseek(pFile, lStartPos, SEEK_SET);
	}

	if ( lEndPos <= lStartPos )
	{
		fclose(pFile);
		return FALSE;
	}

	nTotalDataSize = (lEndPos - lStartPos);
	pSrcReadData = new char[nTotalDataSize];
	if ( !pSrcReadData )
	{
		fclose(pFile);
		return FALSE;
	}

	fread(pSrcReadData, nTotalDataSize, 1, pFile);
	fclose(pFile);

	static DebugValueString	_stValue( "no file" , "Ini Access File = %s" );
	_stValue	= m_strPathName.c_str();

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
		char nBackupChar;
		char* pBackupCharPtr;
		int nBackupDummyPos;

		size_t iOffsetBytes = 0;
		size_t iOffset		= 0;

		BOOL bIniReadFlag = TRUE;

		if ( bIniReadFlag )
		{		
			while ( true )
			{
				iOffset = strcspn(pSrcReadData + iOffsetBytes, CRLF);

				//if ( pSrcReadData[iOffset] == '\r' && pSrcReadData[iOffset + 1] == '\n' )
				if ( pSrcReadData[iOffsetBytes+iOffset] == '\r' && pSrcReadData[iOffsetBytes+iOffset + 1] == '\n' )
				{
					iOffset += g_nCRLELength;
				}
				else
				{
					iOffset++;
					nTotalDataSize--;
				}

				nBackupChar = *(pSrcReadData + iOffsetBytes + iOffset);
				*(pSrcReadData + iOffsetBytes + iOffset) = NULL;	

				szDummyPtr = pSrcReadData + iOffsetBytes;

				iOffsetBytes += iOffset;

				nDummyLen = (int) strlen(szDummyPtr);

				if( nDummyLen > 0 && szDummyPtr[nDummyLen-1] == 13 )
				{
					--nDummyLen;
					szDummyPtr[nDummyLen] = 0;
				}

				if ( (nBackupDummyPos = (int)strcspn(szDummyPtr, ";" )) < nDummyLen )
				{
					szDummyPtr[nBackupDummyPos] = '\0';
					nDummyLen = (int) strlen(szDummyPtr);
				}
				else
				{
					nBackupDummyPos = -1;
				}

				//Section발견
				if ( (int) strcspn(szDummyPtr, "[" ) < nDummyLen )
				{
					if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
						++lFileSectionNum;
				}		
				//Key발견
				else
					if ( (int) strcspn(szDummyPtr, "=" ) < nDummyLen && lFileSectionNum > 0 )
						++lFileKeyNum[lFileSectionNum - 1];

				if ( nBackupDummyPos != -1 )
					szDummyPtr[nBackupDummyPos] = ';';

				*(pSrcReadData + iOffsetBytes) = nBackupChar;

				if ( (INT32) iOffsetBytes >=  nTotalDataSize )
					break;
			}

			//메모리 할당
			lSectionNum = lFileSectionNum;
			m_stSection = new stIniSection[lSectionNum];

			lHalfSectionNum = lSectionNum / 2;
			bIsSectionNumOdd = (lSectionNum % 2) ? true : false;

			for ( i = 0 ; i < lSectionNum ; ++i )
			{
				m_stSection[i].lKeyNum = 0;

				if ( lFileKeyNum[i] )
					m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
				else
					m_stSection[i].stKey = NULL;
			}
		}

		iOffsetBytes = 0;

		//파일로 부터 입력 
		lFileSectionNum = 0;

		ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));

		while ( true )
		{
			iOffset = strcspn(pSrcReadData + iOffsetBytes, CRLF);

			if ( pSrcReadData[iOffsetBytes+iOffset] == '\r' && pSrcReadData[iOffsetBytes+iOffset + 1] == '\n' )
				iOffset += g_nCRLELength;
			else
				iOffset++;

			pBackupCharPtr = pSrcReadData + iOffsetBytes + iOffset - 1;
			while ( pBackupCharPtr && (*pBackupCharPtr == '\n' || *pBackupCharPtr == '\t' || *pBackupCharPtr == '\r' || *pBackupCharPtr == ' ' || *pBackupCharPtr == 13 ) )
				pBackupCharPtr--;

			pBackupCharPtr++;
			nBackupChar = *pBackupCharPtr;
			*pBackupCharPtr = NULL;

			szDummyPtr = pSrcReadData + iOffsetBytes;

			iOffsetBytes += iOffset;

			nDummyLen = (int) strlen(szDummyPtr);

			if( nDummyLen > 0 && szDummyPtr[nDummyLen-1] == 13 )
			{
				--nDummyLen;
				szDummyPtr[nDummyLen] = 0;
			}

			if ( (nBackupDummyPos = (int)strcspn(szDummyPtr, ";" )) < nDummyLen )
			{
				szDummyPtr[nBackupDummyPos] = '\0';
				nDummyLen = (int) strlen(szDummyPtr);
			}
			else
			{
				nBackupDummyPos = -1;
			}

			//Section발견
			if ( (int) strcspn(szDummyPtr, "[" ) < nDummyLen )
			{
				if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
					szDummyPtr, 
					nDummyLen,
					1, 
					(int)strcspn( szDummyPtr, "]" ) - 1, 
					AUINIMANAGER_MAX_NAME ) )
				{
					strcpy(m_stSection[lFileSectionNum].SectionName, "" );
				}

				++lFileSectionNum;
			}		
			//Key발견
			else if ( (int) strcspn(szDummyPtr, "=" ) < nDummyLen && lFileSectionNum > 0 )
			{
				char	strKeyName[ AUINIMANAGER_MAX_NAME ];
				//Key, Value입력하기
				if ( !StringMid( strKeyName , 
					szDummyPtr, 
					nDummyLen,
					0, 
					(int)strcspn( szDummyPtr, "=" ) - 1, 
					AUINIMANAGER_MAX_NAME ) )
				{
					strcpy( strKeyName, "" );
				}

				int nKeyIndex = GetKeyIndex( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;

				if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
					szDummyPtr, 
					nDummyLen,
					(int)strcspn( szDummyPtr, "=" ) + 1, 				
					nDummyLen - 1,
					AUINIMANAGER_MAX_NAME ) )
				{
					strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
				}

				++m_stSection[lFileSectionNum-1].lKeyNum;
				++lFileKeyNum[lFileSectionNum-1];
			}

			if ( nBackupDummyPos != -1 )
				szDummyPtr[nBackupDummyPos] = ';';

			*pBackupCharPtr = nBackupChar;

			if ( (INT32) iOffsetBytes >=  nTotalDataSize )
				break;
		}	
	}
	else if( IsProcessMode( AuIniManagerA::BIN ) )
	{
		// Binary 데이타 읽기....
		// 어떻게 짜야하나 orz..
	}

	ASSERT(pSrcReadData);

	delete [] pSrcReadData;

	if( lSectionNum == 0 )	return FALSE;

	return FALSE;	
}

/*****************************************************************
*   Function : StringMid
*   Date&Time : 2002-12-05, 오후 9:52
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool	AuIniManagerA::StringMid( char* pDest, char* pSource, 
		//@{ 2006/09/01 burumal
		int nSrcLen,
		//@}
		int lStart, int lEnd, int lDestLength )		
{
	if ( lStart > lEnd ) {
		return false;
	}
	
	//@{ 2006/09/01 burumal
	//if ( lStart < 0 || (int)strlen(pSource) <= lEnd ){
	if ( lStart < 0 || nSrcLen <= lEnd ){
	//@}
		ASSERT(false);
		return false;
	}

	if ( lDestLength < lEnd ) {
		ASSERT(false);
		return false;
	}

	int lCopyNum = lEnd - lStart + 1;

	//for ( int i = 0 ; i < lCopyNum ; ++i )
		//pDest[i] = pSource[ lStart + i ];

	memcpy((void*)pDest,(void*)(pSource+lStart),lCopyNum);
	
	pDest[lCopyNum] = '\0';

	return true;
}

/*****************************************************************
*   Function : GetSectionName
*   Comment  : 유효하지 않은 ID가 들어오면 NULL을 리턴 
*   Date&Time : 2002-12-06, 오후 6:04
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
char*	AuIniManagerA::GetSectionName			( int lSectionID ) 
{ 
	if ( lSectionID >= lSectionNum || lSectionID < 0 ) return NULL;

	return m_stSection[lSectionID].SectionName;	
}	

/*****************************************************************
*   Function : GetNumKeys
*   Comment  : 유효하지 않은 ID가 들어오면 -1 리턴
*   Date&Time : 2002-12-06, 오후 6:06
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int		AuIniManagerA::GetNumKeys			( int lSectionID )
{
	if ( lSectionID >= lSectionNum || lSectionID < 0 ) return -1;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
   		// txt mode process
		return m_stSection[lSectionID].lKeyNum;		
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
   		// bin mode process
		return ( int ) m_vecSection[ lSectionID ].vecKey.size();
	}

	return -1;
}	

/*****************************************************************
*   Function : GetKeyName
*   Comment  : 유효하지 않은 ID가 들어오면 NULL을 리턴 
*   Date&Time : 2002-12-06, 오후 6:07
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
const char*	AuIniManagerA::GetKeyName  ( int lSectionID, int lkeyID )
{ 
	if ( lSectionID >= lSectionNum || lSectionID < 0 ) return NULL;
	if ( lkeyID >= GetNumKeys( lSectionID ) || lkeyID < 0 ) return NULL;

	return GetKeyNameTable( m_stSection[lSectionID].stKey[lkeyID].nKeyIndex );
}

/*****************************************************************
*   Function : GetValue
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 10:20
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
char*	AuIniManagerA::GetValue( int lSectionID, int lKeyID, char* defValue )
{
	static char strDummy[] = "";
	
	//default값을 돌려준다. 
	if ( lSectionID >= lSectionNum  || lKeyID >= GetNumKeys( lSectionID ) || lSectionID < 0 || lKeyID < 0 )
	{
		if ( defValue == NULL )
			return strDummy;
		else 
			return defValue;
	}

	return m_stSection[lSectionID].stKey[lKeyID].KeyValue;
}
	
/*****************************************************************
*   Function : GetValue
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 10:20
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
char*	AuIniManagerA::GetValue( const char* pszSectionName, char* pszKeyName, char* defValue )
{
	static char strDummy[] = "";

	//@{ 2006/09/04 burumal
	if ( pszSectionName == NULL || pszKeyName == NULL )
		return NULL;
	//@}

	int SectionID	= 0;
	int KeyID		= 0;
	bool	bSearchSection = false, bSearchKey = false;
	
	for ( int i = 0 ; i < lSectionNum ; ++i )
	{
		if ( !strcmp( pszSectionName, m_stSection[i].SectionName ) )
		{
			SectionID = i;		bSearchSection = true;

			//@{ 2006/09/04 burumal
			break;
			//@}
		}
	}

	int	nKeyIndex = GetKeyIndex( pszKeyName );

	if ( bSearchSection == true )
	{
		for ( int j = 0 ; j < GetNumKeys( SectionID ) ; ++j )
		{
			if ( nKeyIndex == m_stSection[SectionID].stKey[j].nKeyIndex )
			{
				KeyID = j;		bSearchKey = true;

				//@{ 2006/09/04 burumal
				break;
				//@}
			}
		}
	}

	if ( bSearchSection == true && bSearchKey == true )
	{
		return m_stSection[SectionID].stKey[KeyID].KeyValue;
	}
	else 
	{
		if ( defValue == NULL )
			return strDummy;
		else 
			return defValue;
	}

	return strDummy;
}

/*****************************************************************
*   Function : GetStringValue
*   Comment  : Get Value 한 String을 pszDest에 넣어준다. 
*   Date&Time : 2002-12-13, 오후 3:29
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool	AuIniManagerA::GetStringValue( const char* pszSectionName, char* pszKeyName, char* pszDest, int lszDestLength, char* defValue )
{
	char* pszGetString;
	pszGetString = GetValue( pszSectionName, pszKeyName, defValue );
	strncpy( pszDest, pszGetString, lszDestLength );

	return true;
}

/*****************************************************************
*   Function : GetStringValue
*   Comment  : Get Value 한 String을 pszDest에 넣어준다.
*   Date&Time : 2003-02-12, 오후 4:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool	AuIniManagerA::GetStringValue( int lSectionID, int lKeyID, char*	pszDest,  int lszDestLength, char*	defValue )
{
	char* pszGetString;
	pszGetString = GetValue( lSectionID, lKeyID, defValue );
	strncpy( pszDest, pszGetString, lszDestLength );

	return true;
}

/*****************************************************************
*   Function : GetValueI
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 10:45
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int		AuIniManagerA::GetValueI( const char* pszSectionName, char* pszKeyName, int defValue )
{
	char* strValue = GetValue( pszSectionName, pszKeyName );

	//@{ 2006/09/04 burumal
	//if ( !strcmp( strValue, "" ) || strValue == NULL )
	if ( strValue == NULL || strValue[0] == NULL )
	//@}
	{
		return defValue;
	}
	else
		return atoi( strValue );
} 

/*****************************************************************
*   Function : GetValueI64
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 10:45
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
__int64		AuIniManagerA::GetValueI64( const char* pszSectionName, char* pszKeyName, __int64 defValue )
{
	char* strValue = GetValue( pszSectionName, pszKeyName );

	//@{ 2006/09/04 burumal
	//if ( !strcmp( strValue, "" ) || strValue == NULL )
	if ( strValue == NULL || strValue[0] == NULL )
	//@}
	{
		return defValue;
	}
	else
		return _atoi64( strValue );
} 

/*****************************************************************
*   Function : GetValueF
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 10:52
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
double	AuIniManagerA::GetValueF( const char* pszSectionName, char* pszKeyName, double defValue )
{
	char* strValue = GetValue( pszSectionName, pszKeyName );

	//@{ 2006/09/04 burumal
	//if ( !strcmp( strValue, "" ) || strValue == NULL )
	if ( strValue == NULL || strValue[0] == NULL )
	//@}
	{
		return defValue;
	}
	else
		return atof( strValue );
}

/*****************************************************************
*   Function : FindSection
*   Comment  : 찾지 못하면 -1을 돌려준다. 
*   Date&Time : 2002-12-06, 오전 11:12
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::FindSection( const char* pszSection )
{
	//@{ 2006/09/04 burumal
	if ( pszSection == NULL )
		return -1; 
	//@}

	//@{ 2006/11/22 burumal
	/*
	for ( int i = 0 ; i < lSectionNum ; ++i )
	{
		if ( !strcmp( pszSection, m_stSection[i].SectionName ) )
				return i;
	}
	*/
	if ( lSectionNum <= 0 )
		return -1;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		for ( int i = 0; i < lHalfSectionNum; ++i )
		{
			if ( !strcmp( pszSection, m_stSection[i].SectionName ) )
				return i;

			if ( !strcmp( pszSection, m_stSection[lSectionNum - i - 1].SectionName ) )
				return (lSectionNum - i - 1);
		}

		if ( bIsSectionNumOdd )
		{
			if ( !strcmp( pszSection, m_stSection[lHalfSectionNum].SectionName ) )
				return lHalfSectionNum;
		}
		//@}	
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		for ( int i = 0; i < lHalfSectionNum; ++i )
		{
			if ( !strcmp( pszSection, m_vecSection[i].strSection.c_str() ) )
				return i;

			if ( !strcmp( pszSection, m_vecSection[lSectionNum - i - 1].strSection.c_str() ) )
				return (lSectionNum - i - 1);
		}

		if ( bIsSectionNumOdd )
		{
			if ( !strcmp( pszSection, m_vecSection[lHalfSectionNum].strSection.c_str() ) )
				return lHalfSectionNum;
		}
		//@}
	}
	return -1;
}

/*****************************************************************
*   Function : FindKey
*   Comment  : 찾지 못하면 -1을 돌려준다. 
*   Date&Time : 2002-12-06, 오전 11:12
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::FindKey( const char* pszSection, const char* pszKey )
{
	//@{ 2006/09/04 burumal
	if ( pszSection == NULL || pszKey == NULL )
		return -1;
	//@}

	int SectionID = FindSection( pszSection );
	if ( SectionID < 0 ) return -1;

	return FindKey( SectionID , pszKey );
}

/*****************************************************************
*   Function : FindKey
*   Comment  : 찾지 못하면 -1 돌려준다. 
*   Date&Time : 2002-12-06, 오전 11:12
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::FindKey( int SectionID, const char* pszKey )
{
	//@{ 2006/09/04 burumal
	if ( pszKey == NULL )
		return -1;
	//@}

	if ( SectionID >= lSectionNum || SectionID < 0 ) return -1;

	int nKeyIndex	= GetKeyIndex( pszKey );

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		stIniSection	* pSection = &m_stSection[SectionID];

		if ( pSection->lKeyNum <= 0 )
			return -1;

		for( int j = 0 ; j < pSection->lKeyNum ; j ++ )
		{
			if( pSection->stKey[ j ].nKeyIndex == nKeyIndex )
				return j;
		}

		return -1;
	}
	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		stIniSectionBinary * pSection = &m_vecSection[SectionID];

		if ( pSection->vecKey.size() <= 0 )
			return -1;

		for( int j = 0 ; j < ( int ) pSection->vecKey.size() ; j ++ )
		{
			if( pSection->vecKey[ j ].nKeyIndex == nKeyIndex )
				return j;
		}

		return -1;
	}

	return -1;
}

/*****************************************************************
*   Function : SetValue
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 11:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/	
bool AuIniManagerA::SetValue( int SectionID, int KeyID, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	if ( SectionID >= lSectionNum || SectionID < 0 ) return false;
	if ( KeyID >= GetNumKeys( SectionID ) || KeyID < 0 ) return false;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
		stIniKey	* pKey = &m_stSection[SectionID].stKey[KeyID];

   		// txt mode process
		pKey->eType	= eType;

		switch( eType )
		{
		case KT_I		:	sprintf( pKey->KeyValue , "%d" , data.i );						break;
		case KT_I64		:	sprintf( pKey->KeyValue , "%d" , data.i64 );					break;
		case KT_F		:	
							{
								// 여기서 용량을 좀 줄인다.

								//sprintf( pKey->KeyValue , "%f" , data.f );
								print_compact_float( pKey->KeyValue , data.f );
								break;
							}
		case KT_STR		:	strncpy( pKey->KeyValue, data.str, AUINIMANAGER_MAX_KEYVALUE );	break;
		}
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
   		// bin mode process

		stIniKeyBinary	*pKey	= &m_vecSection[ SectionID ].vecKey[ KeyID ];

		pKey->eType	= eType	;
		pKey->data	= data	;
	}

	return true;
}

/*****************************************************************
*   Function : SetValue
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 11:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValue( const char* pszSectionName, const char* pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	//@{ 2006/09/04 burumal
	if ( pszSectionName == NULL || pszKeyName == NULL )
		return false;
	//@}

	int SectionID	= 0;
	int KeyID		= 0;

	SectionID = FindSection( pszSectionName );

	if ( SectionID >= lSectionNum ) return false;
	if ( SectionID < 0 ) 
	{
		SectionID = AddSection( pszSectionName );		
		
		if ( SectionID < 0 ) return false;
	}
	
	if ( m_eMode != AUINIMANAGER_MODE_NAME_OVERWRITE)
	{
		KeyID = FindKey( SectionID, pszKeyName );
		
		// 아래 주석과 같은경우가 발생해선 안된다.
		//if ( KeyID >= m_stSection[SectionID].lKeyNum ) return false;
	}
	else
		KeyID = -1;

	if ( KeyID < 0 )				//해당하는 KeyName이 없는 경우 
	{
		KeyID = AddKey( SectionID, pszKeyName, eType , data );

		if ( KeyID < 0  ) return false;
		else return true;
	}
	
	SetValue( SectionID , KeyID , eType , data );
	return true;
}

/*****************************************************************
*   Function : SetValueI
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 11:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValueI( const char* pszSectionName, const char* pszKeyName, int value )
{
	stIniKeyBinary::DATA data;
	data.i	= value;
	return SetValue( pszSectionName, pszKeyName, KT_I , data );
}

bool AuIniManagerA::SetValue( const char*	pszSectionName, const char*	pszKeyName, const char* pszvalue)
{	
	stIniKeyBinary::DATA data;
	strncpy( data.str , pszvalue , AUINIMANAGER_MAX_KEYVALUE );
	return SetValue( pszSectionName, pszKeyName, KT_STR , data );
}

/*****************************************************************
*   Function : SetValueI
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 11:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValueI64( const char* pszSectionName, const char* pszKeyName, __int64 value )
{	
	stIniKeyBinary::DATA data;
	data.i64	= value;
	return SetValue( pszSectionName, pszKeyName, KT_I64 , data );
}

/*****************************************************************
*   Function : SetValueF
*   Comment  : 
*   Date&Time : 2002-12-06, 오전 11:51
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValueF( const char* pszSectionName, const char* pszKeyName, double value )
{	
	stIniKeyBinary::DATA data;

	// 여기서 더블값을 날려먹는게 영 신경쓰임.
	data.f	= ( float ) value;
	return SetValue( pszSectionName, pszKeyName, KT_F , data );
}

/*****************************************************************
*   Function : AddSection
*   Comment  : 성공하면 ID 실패하면 -1을 돌려준다. 
*   Date&Time : 2002-12-06, 오전 11:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int AuIniManagerA::AddSection( const char* pszSectionName )
{	
	int FindIndex = FindSection( pszSectionName );
	if ( FindIndex >= 0 ) return -1;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		stIniSection *pszDummy = NULL;
		pszDummy = (stIniSection*) realloc( m_stSection, sizeof(stIniSection) * ( lSectionNum + 1 ) );
		
		if ( pszDummy == NULL )  return -1;

		m_stSection = pszDummy;
		
		++lSectionNum;
				
		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		m_stSection[lSectionNum-1].lKeyNum = 0;
		m_stSection[lSectionNum-1].stKey = NULL;
		strncpy( m_stSection[lSectionNum-1].SectionName, pszSectionName, AUINIMANAGER_MAX_NAME );

		return ( lSectionNum - 1 );	
	}
	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		stIniSectionBinary	stSection;
		stSection.strSection	= pszSectionName;
		
		m_vecSection.push_back( stSection );
			
		lSectionNum = ( int ) m_vecSection.size();
				
		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		return ( lSectionNum - 1 );	
	}

	return -1;
}

/*****************************************************************
*   Function : AddKey
*   Comment  : 성공하면 ID 실패하면 -1을 돌려준다 .
*   Date&Time : 2002-12-06, 오전 11:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int AuIniManagerA::AddKey( int SectionID, const char* pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	//@{ 2006/09/04 burumal
	if ( pszKeyName == NULL )
		return -1;
	//@}

	if ( SectionID >= lSectionNum || SectionID < 0 ) return -1;
	
	int FindIndex;
	
	if( m_eMode != AUINIMANAGER_MODE_NAME_OVERWRITE)
	{
		FindIndex = FindKey( SectionID, pszKeyName );
		if( FindIndex >= 0 ) return -1;
	}

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		stIniKey *pszDummy = NULL;

		pszDummy = (stIniKey*) realloc( m_stSection[SectionID].stKey,  sizeof(stIniKey) * ( m_stSection[SectionID].lKeyNum + 1 ) );

		if ( pszDummy == NULL ) return -1;
		m_stSection[SectionID].stKey = pszDummy;

		++m_stSection[SectionID].lKeyNum;

		int KeyID	= m_stSection[SectionID].lKeyNum - 1;

		int	nKeyIndex = GetKeyIndex( pszKeyName );
		
		m_stSection[SectionID].stKey[ KeyID ].nKeyIndex	= nKeyIndex;
		SetValue( SectionID , KeyID , eType , data );

		return KeyID;
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		stIniKeyBinary	stKey;

		int	nKeyIndex = GetKeyIndex( pszKeyName );
		stKey.nKeyIndex	= nKeyIndex;

		stIniSectionBinary *pSection = &m_vecSection[ SectionID ];

		pSection->vecKey.push_back( stKey );
		int KeyID = ( int ) pSection->vecKey.size() - 1;

		SetValue( SectionID , KeyID , eType , data );
		return KeyID;
	}

	// 요까지 오면 안돼요
	ASSERT( !"머시라!" );
	return -1;
}

/*****************************************************************
*   Function : AddKey
*   Comment  : 성공하면 ID 실패하면 -1 돌려준다 .
*   Date&Time : 2002-12-06, 오전 11:40
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::AddKey( const char* pszSection, const char*pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	int SectionID = FindSection( pszSection );
	if ( SectionID < 0 ) return -1;

	return AddKey( SectionID, pszKeyName, eType , data );
}

/*****************************************************************
*   Function : WriteFile
*   Comment  : 
*   Date&Time : 2002-12-06, 오후 12:11
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AuIniManagerA::WriteFile	(int lIndex,  BOOL bEncryption)
{
	if ( m_strPathName.size() == 0 ) return FALSE;

	FILE *pFile;
	char szDummy[AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1];
	int i = 0 , j = 0;
	char *pPreBuffer = NULL;
	char *pPostBuffer = NULL;
	int lPreBuffer = 0;
	int lPostBuffer = 0;
	__int64 lCurPos;

	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		if ( lIndex && (pFile = fopen( m_strPathName.c_str() , "rt" )) )
		{
			if ( !ReadPartIndices(pFile) )
			{
				fclose(pFile);
				return FALSE;
			}

			fclose(pFile);
		}
		else
		{
			pFile = fopen( m_strPathName.c_str() , "wt" );
			if( pFile ) fclose( pFile );
			else
			{
				TRACE( "AuIniManagerA::WriteFile Write Stream Open Fail\n" );
			}
			
			memset(m_plPartIndices, 0, sizeof(int) * m_lPartNum);
		}

		pFile = fopen( m_strPathName.c_str() , "r+t" );
		if ( !pFile )
		{
			TRACE( "AuIniManagerA::WriteFile Read + Stream Open Fail\n" );
			return FALSE;
		}

		WritePartIndices(pFile);

		fseek(pFile, 0, SEEK_END);

		fgetpos(pFile, &lCurPos);

		for ( i = 0 ; i < lSectionNum ; ++i )
		{
			sprintf( szDummy, "[%s]\n", m_stSection[i].SectionName );
			fputs( szDummy, pFile );
			
			for ( j = 0 ; j < GetNumKeys( i ) ; ++j )
			{
				const char * pKeyName = GetKeyNameTable( m_stSection[i].stKey[j].nKeyIndex );
				sprintf( szDummy, "%s=%s\n", pKeyName , m_stSection[i].stKey[j].KeyValue );
				fputs( szDummy, pFile );
			}
		}

		m_plPartIndices[lIndex] = (int) lCurPos;

		WritePartIndices(pFile);
		fclose(pFile);

		return FALSE;
	}

	if ( (pFile = fopen( m_strPathName.c_str() , "wt" )) == NULL )	//File Open
	{
		//File이 ReadOnly라면 속성을 바꾸고 다시한번 읽어 분다. 
		DWORD lFileAttribute = GetFileAttributes( m_strPathName.c_str() );
		
		if ( FILE_ATTRIBUTE_READONLY == ( lFileAttribute & FILE_ATTRIBUTE_READONLY ) )
		{
			SetFileAttributes( 	m_strPathName.c_str(), lFileAttribute & ~FILE_ATTRIBUTE_READONLY );
			if ( (pFile = fopen( m_strPathName.c_str() , "wt" )) == NULL ) return FALSE;
		}
		else
			return FALSE;
	}

	bool	bUseKeyIndex = GetType() & AUINIMANAGER_TYPE_KEYINDEX ? true : false;
	if( bUseKeyIndex )
	{
		// Key String Write
		vector< string >::iterator	iter = m_vecKeyTable.begin();
		for( int	nCount = 0;
			iter != m_vecKeyTable.end();
			iter++ , nCount++ )
		{
			const char * pKeyName = GetKeyNameTable( nCount );
			sprintf( szDummy, "%d=%s\n", nCount , pKeyName );
			fputs( szDummy, pFile );
		}
	}

	for ( i = 0 ; i < lSectionNum ; ++i )
	{
		sprintf( szDummy, "[%s]\n", m_stSection[i].SectionName );
		fputs( szDummy, pFile );
		
		for ( j = 0 ; j < GetNumKeys( i ) ; ++j )
		{
			if( bUseKeyIndex )
			{
				sprintf( szDummy, "%d=%s\n", m_stSection[i].stKey[j].nKeyIndex , m_stSection[i].stKey[j].KeyValue );
				fputs( szDummy, pFile );
			}
			else
			{
				const char * pKeyName = GetKeyNameTable( m_stSection[i].stKey[j].nKeyIndex );
				sprintf( szDummy, "%s=%s\n", pKeyName , m_stSection[i].stKey[j].KeyValue );
				fputs( szDummy, pFile );
			}
		}
	}

	fclose(pFile);

	if (bEncryption)
		EncryptSave();

	// 마고자 (2004-05-31 오후 6:02:03) : 이게 왜 리턴 폴스지? ;;
	return TRUE;
	//return FALSE;
}

/*****************************************************************
*   Function : ClearAllSectionKey
*   Comment  : All Section, All Key Data Clear
*   Date&Time : 2003-01-29, 오후 3:00
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AuIniManagerA::ClearAllSectionKey()
{
	if ( m_stSection != NULL )
	{
		for ( int i = 0 ; i < lSectionNum ; ++i )
		{
			if ( m_stSection[i].stKey != NULL ) 
			{
				delete [] m_stSection[i].stKey;
				m_stSection[i].stKey = NULL;
			}
		}

		delete [] m_stSection;
		m_stSection = NULL;
	}

	lSectionNum = 0;

	m_vecSection.clear();

	//@{ 2006/11/22 burumal
	lHalfSectionNum = 0;
	bIsSectionNumOdd = false;
	//@}
}

bool	AuIniManagerA::SetMode(AuIniManagerMode eMode)
{
	m_eMode = eMode;

	return true;
}

bool	AuIniManagerA::SetType(DWORD eType)
{
	m_eType = eType;

	return true;
}

//@{ 2006/09/05 burumal
// 사용되지 않는 함수로 판단 삭제함
// bool	AuIniManagerA::AddPartIndices(int lStartIndex, int lEndIndex, int lOffset)
// {
// 	int lOffsetAll = 0;
// 	int lIndex;
// 	int * plNewAlloc = NULL;
// 
// 	if ( lEndIndex >= m_lPartNum )
// 	{
// 		/*
// 		plNewAlloc = new int[lEndIndex + 1];
// 
// 		if ( !plNewAlloc )
// 		{
// 			return false;
// 		}
// 
// 		memset(plNewAlloc, 0, sizeof(int) * (lEndIndex + 1));
// 
// 		if ( m_plPartIndices )
// 		{
// 			memcpy(plNewAlloc, m_plPartIndices, sizeof(int) * m_lPartNum);
// 			delete [] m_plPartIndices;
// 		}
// 
// 		m_plPartIndices = plNewAlloc;
// 
// 		m_lPartNum = lEndIndex + 1;
// 
// 		lOffsetAll = GetStartIndex() - m_plPartIndices[0];
// 
// 		for ( lIndex = 0; lIndex < m_lPartNum; lIndex++ )
// 		{
// 			m_plPartIndices[lIndex] += lOffsetAll;
// 		}
// 		*/
// 		return false;
// 	}
// 	else
// 	{		
// 		for ( lIndex = lStartIndex ; lIndex <= lEndIndex ; ++lIndex )
// 		{
// 			m_plPartIndices[lIndex] += lOffsetAll + lOffset;
// 		}
// 	}
// 
// 	return true;
// }

int	AuIniManagerA::ReadPartIndices(char* pBuffer)
{
	size_t iOffsetBytes = 0;
	size_t iOffset = 0;
	int i;

	//@{ 2006/08/31 burumal
	//char szDummy[400] = {0,};
	char szDummy[400];
	//@}

	iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);

	//@{ 2006/07/28 burumal
	//iOffset += strlen(CRLF);
	iOffset += g_nCRLELength;
	//@}

	memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

	//@{ 2006/08/31 burumal
	if ( iOffset < 400 )
	{
		szDummy[iOffset] = '\0';
		if( iOffset > 0 && szDummy[iOffset - 1] == 13 )
			szDummy[iOffset - 1] = 0;

	}
	//@}

	iOffsetBytes += iOffset;

	i = (int)strlen(szDummy) - 1;
	while( i > -1 && ( szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' || szDummy[i] == 13 ) )
	{
		szDummy[i] = '\0';
		--i;
	}

	m_lPartNum = atoi(szDummy);

	if ( m_plPartIndices )
		delete [] m_plPartIndices;

	m_plPartIndices = new int[m_lPartNum];
	if ( !m_plPartIndices )
		return -1;

	for ( int iLoop = 0 ; iLoop < m_lPartNum ; ++iLoop )
	{
		iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);

		//@{ 2006/07/28 burumal
		//iOffset += strlen(CRLF);
		iOffset += g_nCRLELength;
		//@}

		memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

		i = (int)strlen(szDummy) - 1;

		szDummy[i+1] = 0;

		while( i > -1 && ( szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' ||szDummy[i] == 13 ) )
		{
			szDummy[i] = '\0';
			--i;
		}

		*(m_plPartIndices + iLoop) = atoi(szDummy);
		//		if ( !(iOffset = sscanf(pBuffer	+ iOffsetBytes, "%10d\n", m_plPartIndices + i)) )
		//			return -1;

		iOffsetBytes += iOffset;
	}

	return (int)iOffsetBytes;
}

bool	AuIniManagerA::ReadPartIndices(FILE *pFile)
{
	if ( !fscanf(pFile, "%10d\n", &m_lPartNum) )
		return false;
	
	if ( m_plPartIndices )
		delete [] m_plPartIndices;	

	m_plPartIndices = new int[m_lPartNum];
	if ( !m_plPartIndices )
		return false;

	for ( int i = 0 ; i < m_lPartNum ; ++i )
	{
		if ( !fscanf(pFile, "%10d\n", m_plPartIndices + i ) )
			return false;
	}

	return true;
}

bool	AuIniManagerA::WritePartIndices(FILE *pFile)
{
	fseek(pFile, 0, SEEK_SET);

	fprintf(pFile, "%10d\n", m_lPartNum);

	for ( int i = 0 ; i < m_lPartNum ; ++i )
	{
		fprintf(pFile, "%10d\n", m_plPartIndices[i]);
	}

	return true;
}

int		AuIniManagerA::GetStartIndex()
{
	return (12 * (m_lPartNum + 1));
}

bool	AuIniManagerA::EncryptSave()
{
	FILE *pReadFile;

	pReadFile = fopen(m_strPathName.c_str(), "rb");

	if (NULL == pReadFile) return false;

	// 파일 사이즈 알아오기
	fseek(pReadFile, 0, SEEK_END);
	
    int lFileSize = ftell(pReadFile);
	fseek(pReadFile, 0, SEEK_SET);

	// 파일을 읽어들임
	char* pBuffer = new char[lFileSize];
	memset(pBuffer, 0, lFileSize);
	int lReadBytes = (int)fread(pBuffer, sizeof(BYTE), lFileSize, pReadFile);
	fclose(pReadFile);

	AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
	MD5.EncryptString(MD5_HASH_KEY_STRING, pBuffer, lFileSize);
#else
	MD5.EncryptString(HASH_KEY_STRING, pBuffer, lFileSize);
#endif
	
	FILE *pWriteFile = fopen(m_strPathName.c_str(), "wb");
	if (NULL == pWriteFile) return false;

	int WriteBytes = (int)fwrite(pBuffer, sizeof(char), lFileSize, pWriteFile);

	fclose(pWriteFile);

	if (WriteBytes == lFileSize) return true;
	else return false;
}