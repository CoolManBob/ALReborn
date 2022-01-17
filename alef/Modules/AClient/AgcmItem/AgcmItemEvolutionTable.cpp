#include "AgcmItemEvolutionTable.h"
#include "AuMD5Encrypt.h"
#include "AgcmUILoginSetting.h"





AgcmItemEvolutionTable::AgcmItemEvolutionTable( void )
{
	m_mapEvolutionTable.Clear();
}

AgcmItemEvolutionTable::~AgcmItemEvolutionTable( void )
{
}

BOOL AgcmItemEvolutionTable::OnLoadTable( char* pTableFileName )
{
	if( !pTableFileName || strcmp( pTableFileName, "" ) == 0 ) return FALSE;

	String_stream Stream;
#ifdef USE_MFC
	if( !Stream.readfile( pTableFileName, false, "rt" ) ) return FALSE;
#else
	if( !Stream.readfile( pTableFileName, true, "rb" ) ) return FALSE;
#endif

	m_mapEvolutionTable.Clear();
	
	CHAR strBufferLine[ MAX_PATH ] = { 0, };
	CHAR strTIDSlayer[ 10 ] = { 0, };
	CHAR strTIDOrbiter[ 10 ] = { 0, };
	CHAR strTIDScion[ 10 ] = { 0, };
	CHAR strTIDSummoner[ 10 ] = { 0, };

	while( !Stream.seof() )
	{
		memset( strTIDSlayer, 0, sizeof( CHAR ) * 10 );
		memset( strTIDOrbiter, 0, sizeof( CHAR ) * 10 );
		memset( strTIDScion, 0, sizeof( CHAR ) * 10 );
		memset( strTIDSummoner, 0, sizeof( CHAR ) * 10 );

		memset( strBufferLine, 0, sizeof( CHAR ) * MAX_PATH );
		if( !Stream.sgetf( strBufferLine ) )
		{
			Stream.Empty();
			if( strcmp( strBufferLine, "" ) == 0 ) return TRUE;			
			return FALSE;
		}

		// 순서는 무조건 슬레이어, 오비터, 시온, 서머너의 순서이다.
		if( !_ParseTableLine( strBufferLine, strTIDSlayer, strTIDOrbiter, strTIDScion, strTIDSummoner ) ) continue;
		if( !_IsDigitString( strTIDSlayer ) ) continue;

		stItemEvolutionEntry NewEntry;

		NewEntry.m_nSlayerTID = atoi( strTIDSlayer );
		NewEntry.m_nOrbiterTID = atoi( strTIDOrbiter );
		NewEntry.m_nScionTID = atoi( strTIDScion );
		NewEntry.m_nSummonerTID = atoi( strTIDSummoner );

		m_mapEvolutionTable.Add( NewEntry.m_nScionTID, NewEntry );
	}

	Stream.Empty();
	return TRUE;
}

INT32 AgcmItemEvolutionTable::GetEvolutionTID( INT32 nCommonTID, AuCharClassType eClassType )
{
	stItemEvolutionEntry* pEntry = m_mapEvolutionTable.Get( nCommonTID );
	if( !pEntry ) return -1;

	switch( eClassType )
	{
	case AUCHARCLASS_TYPE_KNIGHT :	return pEntry->m_nSlayerTID;	break;
	case AUCHARCLASS_TYPE_RANGER :	return pEntry->m_nOrbiterTID;	break;
	case AUCHARCLASS_TYPE_MAGE :	return pEntry->m_nSummonerTID;	break;
	case AUCHARCLASS_TYPE_SCION :	return pEntry->m_nScionTID;		break;
	}

	return -1;
}

BOOL AgcmItemEvolutionTable::_IsDigitString( char* pString )
{
	if( pString[ 0 ] < '0' || pString[ 0 ] > '9' ) return FALSE;
	return TRUE;
}

BOOL AgcmItemEvolutionTable::_ParseTableLine( char* pBuffer, char* pSlayer, char* pOrbiter, char* pScion, char* pSummoner )
{
	if( !pBuffer ) return FALSE;

	int nLength = strlen( pBuffer );
	if( nLength <= 0 ) return FALSE;

	if( !pSlayer || !pOrbiter || !pScion || !pSummoner ) return FALSE;

	INT32 nTabCount = 0;
	INT32 nCopyCount = 0;
	CHAR* pDestBuffer = NULL;

	for( INT32 nCount = 0 ; nCount < nLength ; ++nCount )
	{
		// 현재 문자열이 널문자면 다음줄을 읽어야 하니 그만..
		if( pBuffer[ nCount ] == '\n' )
		{
			return TRUE;
		}

		// 탭갯수에 따라 버퍼를 바꿔준다.. 범위밖이면 그만..
		switch( nTabCount )
		{
		case 0 : 	pDestBuffer = pSlayer;		break;
		case 1 : 	pDestBuffer = pOrbiter;		break;
		case 2 : 	pDestBuffer = pScion;		break;
		case 3 : 	pDestBuffer = pSummoner;	break;
		default :	return TRUE;				break;
		}

		if( pBuffer[ nCount ] >= '0' && pBuffer[ nCount ] <= '9' )
		{
			pDestBuffer[ nCopyCount ] = pBuffer[ nCount ];
			nCopyCount++;
		}

		// 탭문자면..
		if( pBuffer[ nCount ] == '\t' )
		{
			nTabCount++;

			// 한글자도 복사된게 없는데 벌써 탭이라면.. 0 이라고 써준다..
			// _IsDigitString() 함수를 통과하기 위한 조치다..
			if( nCopyCount == 0 )
			{
				pDestBuffer[ 0 ] = '0';
			}

			nCopyCount = 0;			
		}
	}

	return TRUE;
}
