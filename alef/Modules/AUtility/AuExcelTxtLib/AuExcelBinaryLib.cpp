#include "AuExcelBinaryLib.h"
#include "aplib.h"

using namespace AuExcel;

Element * Element::DetermineType( BYTE	* pBuffer )
{
	// Factory..
	TYPE	type = static_cast< TYPE >( *pBuffer );
	switch( type )
	{
	case	ET_EMPTY	:	return new Element		;
	case	ET_INT		:	return new ElementINT	;
	case	ET_DOUBLE	:	return new ElementDOUBLE;
	case	ET_STRING	:	return new ElementSTRING;
	default:				return NULL;
	}
}

size_t	Element::GetMaxBlockSize()
{
	size_t	uMax = sizeof Element;
	size_t	uSize;
	uSize = sizeof ElementINT;
	if( uMax < uSize ) uMax = uSize;
	uSize = sizeof ElementDOUBLE;
	if( uMax < uSize ) uMax = uSize;
	uSize = sizeof ElementSTRING;
	if( uMax < uSize ) uMax = uSize;
	return uMax;
}

Element * Element::ReadTypeFromString( const char * pString )	// Factory Function
{
	if( !pString )
		return new Element();

	char	str[ 65536 ];
	// 먼저 int 인지 점검..
	{
		INT_TYPE	nData = atoi( pString );

		sprintf( str , "%d" , nData );
		
		if( !strcmp( pString , str ) )
		{
			return new ElementINT( nData );
		}
	}

	// Double 인지 점검..
	{
		DOUBLE_TYPE	dData = atof( pString );

		// 소수점 두자리 까지만 적는다..
		sprintf( str , "%.2f" , dData );
		
		if( !strcmp( pString , str ) )
		{
			return new ElementDOUBLE( dData );
		}
	}

	// 그외엔 스트링이다.

	return new ElementSTRING( pString );
}


AuExcelBinaryLib::AuExcelBinaryLib(AuExcelTxtLib &txtlib)
	: m_iColumn(0) , m_iRow(0)
{
	// 복사과정..

	m_iColumn	= txtlib.GetColumn();
	m_iRow		= txtlib.GetRow();

	// 메모리 준비.
	CreateMemoryBlock( Element::GetMaxBlockSize() * m_iColumn * m_iRow );

	for( int nRow = 0 ; nRow < m_iRow ; nRow ++)
	{
		for( int nColumn = 0 ; nColumn < m_iColumn ; nColumn ++)
		{
			char * pData	= txtlib.GetData( nColumn , nRow );

			// 데이타 분석..
			// int , double , string 을 구분한다.
			Element * pElement = Element::ReadTypeFromString( pData );
			
			ASSERT( pElement );

			vecElement.push_back( pElement );
		}
	}
}

BOOL AuExcelBinaryLib::ReadExcelFile	( const char *pstrFileName)
{
#ifndef WIN64
	FILE	* pFile	= fopen( pstrFileName , "rb" );
	if( !pFile ) return FALSE;

	Header	stHeader;
	fread( ( void * ) &stHeader , sizeof Header , 1 , pFile );

	// Validation
	if( stHeader.nIdentifier	!= cIdentifier	) return FALSE;
	if( stHeader.nVersion		!= cVersion		) return FALSE;
	if( !stHeader.nColumn						) return FALSE;
	if( !stHeader.nRow							) return FALSE;
	if( !stHeader.nDataSize						) return FALSE;
	if( !stHeader.nPackedSize					) return FALSE;

	Clear();

	m_iColumn	= stHeader.nColumn;
	m_iRow		= stHeader.nRow;

	// 읽어 들이기
	size_t	nPackedSize	= stHeader.nPackedSize;
	BYTE	* pPackedBuffer	= new BYTE[ nPackedSize ];

	if( !pPackedBuffer ) return FALSE;

	fread( ( void * ) pPackedBuffer , sizeof BYTE , nPackedSize , pFile );

	// 압축 풀기

	size_t	nTotalSize	= stHeader.nDataSize;
	BYTE * pBuffer		;
	{
		pBuffer		= new BYTE[ nTotalSize ];
		int depacked_length = aP_depack_asm_fast( pPackedBuffer , pBuffer );

		ASSERT( depacked_length == nTotalSize );
	}

	// 메모리 준비.
	CreateMemoryBlock( Element::GetMaxBlockSize() * m_iColumn * m_iRow );

	// 데이타 읽기

	BYTE	*pPointer = pBuffer;
	size_t	nCurrent	= 0;

	while( nCurrent < nTotalSize )
	{
		Element	* pElement = NULL;

		pElement = Element::DetermineType( pPointer );
		if( !pElement ) break;

		pPointer = pElement->ReadType( pPointer );
		pPointer = pElement->ReadData( pPointer );

		nCurrent += pElement->GetSize();

		vecElement.push_back( pElement );
	}

	TRACE( "'%s' %d Columns %d Rows .. %d Elements\n" , pstrFileName , m_iColumn , m_iRow , vecElement.size() );

	ASSERT( m_iColumn * m_iRow == vecElement.size() );

	delete [] pBuffer;
	delete [] pPackedBuffer;

	return TRUE;
#else
	return FALSE;
#endif
}

BOOL AuExcelBinaryLib::WriteExcelFile	( const char *pstrFileName)
{
#ifndef WIN64

	ASSERT( m_iColumn * m_iRow == vecElement.size() );

	FILE	* pFile	= fopen( pstrFileName , "wb" );
	if( !pFile ) return FALSE;

	BYTE	*pBuffer = NULL;

	// 전체 크기 확인
	size_t	nTotalSize = 0;

	for( std::vector< Element * >::iterator	iter = vecElement.begin();
		iter != vecElement.end();
		iter ++)
	{
		Element * pElement = *iter;
		nTotalSize += pElement->GetSize();
	}

	// 버퍼준비..

	pBuffer	= new BYTE[ nTotalSize ];
	BYTE * pPointer	= pBuffer;

	for( std::vector< Element * >::iterator	iter = vecElement.begin();
		iter != vecElement.end();
		iter ++)
	{
		Element * pElement = *iter;

		pPointer = pElement->WriteType( pPointer );
		pPointer = pElement->WriteData( pPointer );
	}

	// 압축과정

	size_t	nPackedSize;	
	BYTE	*pPackedBuffer;

	{
		pPackedBuffer = new BYTE[ ( ( nTotalSize * 9 ) / 8 ) + 16 ];
		BYTE * pworking = new BYTE[ 640 * 1024 ];

		nPackedSize = aP_pack(
			pBuffer,
			pPackedBuffer,
			( unsigned int ) nTotalSize,
			pworking,
			NULL );

		delete pworking;
	}

	// Writing 과정

	// Header
	// Version
	// Columns
	// Rows

	Header	stHeader;
	stHeader.nColumn	= GetColumn();
	stHeader.nRow		= GetRow();
	stHeader.nDataSize	= nTotalSize;
	stHeader.nPackedSize= nPackedSize;

	fwrite( ( void * ) &stHeader , sizeof Header , 1 , pFile );
	fwrite( ( void * ) pPackedBuffer , sizeof BYTE , nPackedSize , pFile );
	fclose( pFile );

	delete [] pBuffer;
	delete [] pPackedBuffer;
	return TRUE;
#else
	return FALSE;
#endif
}

BOOL AuExcelBinaryLib::WriteExcelFIleText	( const char *pstrFileName , bool bShowType )
{
	ASSERT( m_iColumn * m_iRow == vecElement.size() );

	FILE	* pFile	= fopen( pstrFileName , "wt" );
	if( !pFile ) return FALSE;

	for( int iRow = 0 ; iRow < GetRow() ; iRow++ )
	{
		bool	bFirstColumn = true;

		for( int iColumn = 0 ; iColumn < GetColumn() ; iColumn++ )
		{
			Element *	pElement = GetElement( iColumn , iRow );

			std::string	strType;
			if( bShowType )
				switch( pElement->GetType() )
				{
				default:
				case	Element::ET_EMPTY	: break;	// nothing..
				case	Element::ET_INT		:	strType = "(I)" ; break;
				case	Element::ET_DOUBLE	:	strType = "(F)" ; break;
				case	Element::ET_STRING	:	strType = "(S)" ; break;
				case	Element::ET_ERROR	:	strType = "(E)" ; break;
				}

			
			const char * pStr = pElement->GetSTRING();
			std::string text;
			
			if( pStr ) text = pStr;
			else if( bShowType ) text = "<empty>";

			if( bFirstColumn )	fprintf( pFile , "%s%s" , strType.c_str() , text.c_str() );
			else				fprintf( pFile , "\t%s%s" , strType.c_str() , text.c_str() );

			bFirstColumn	= false;
		}

		fprintf( pFile , "\n" );
	}

	fclose( pFile );

	return TRUE;
}
