#include "AgpmTransform.h"
#include "AuExcelTxtLib.h"

AgpmTransform::AgpmTransform()
{
	m_mapCharacterInfo.clear();
	m_mapItemInfo.clear();
}

AgpmTransform::~AgpmTransform()
{
}

BOOL	AgpmTransform::Initialize( char* szCharacter, char* szItem, BOOL bDecryption )
{
	if( !szCharacter || !szItem )		return FALSE;
	
	AuExcelTxtLib	cExcel;
	if( cExcel.OpenExcelFile( szCharacter, TRUE, bDecryption ) )
		for( int nRow = 1; nRow < cExcel.GetRow(); ++nRow )
			m_mapCharacterInfo.insert( make_pair( atoi( cExcel.GetData( 1, nRow ) ), new TransformInfo( atoi( cExcel.GetData( 2, nRow ) ), atoi( cExcel.GetData( 3, nRow ) ), atoi( cExcel.GetData( 4, nRow ) ) ) ) );

	if( cExcel.OpenExcelFile( szItem, TRUE, bDecryption ) )
		for( int nRow = 1; nRow < cExcel.GetRow(); ++nRow )
			m_mapItemInfo.insert( make_pair( atoi( cExcel.GetData( 1, nRow ) ), new TransformInfo( atoi( cExcel.GetData( 2, nRow ) ), atoi( cExcel.GetData( 3, nRow ) ), atoi( cExcel.GetData( 4, nRow ) ) ) ) );
														
	return TRUE;
}

void	AgpmTransform::Release()
{
	for( TransformInfoMapItr Itr = m_mapCharacterInfo.begin(); Itr != m_mapCharacterInfo.end(); ++Itr )
	{
		if( (*Itr).second )
		{
			delete (*Itr).second;
			(*Itr).second = NULL;
		}
	}
	m_mapCharacterInfo.clear();

	for( TransformInfoMapItr Itr = m_mapItemInfo.begin(); Itr != m_mapItemInfo.end(); ++Itr )
	{
		if( (*Itr).second )
		{
			delete (*Itr).second;
			(*Itr).second = NULL;
		}
	}
	m_mapItemInfo.clear();
}

INT32	AgpmTransform::GetCharacterID( INT32 nID, AuCharClassType eClassType )
{
	TransformInfoMapItr Itr = m_mapCharacterInfo.find( nID );
	if( Itr == m_mapCharacterInfo.end() )	return -1;

	switch( eClassType )
	{
	case AUCHARCLASS_TYPE_KNIGHT:	return (*Itr).second->m_nKnight;
	case AUCHARCLASS_TYPE_RANGER:	return (*Itr).second->m_nRanger;
	case AUCHARCLASS_TYPE_MAGE:		return (*Itr).second->m_nMage;
	default:						return -1;
	}
}

INT32	AgpmTransform::GetItemID( INT32 nID, AuCharClassType eClassType )
{
	TransformInfoMapItr Itr = m_mapItemInfo.find( nID );
	if( Itr == m_mapItemInfo.end() )	return -1;

	switch( eClassType )
	{
	case AUCHARCLASS_TYPE_KNIGHT:	return (*Itr).second->m_nKnight;
	case AUCHARCLASS_TYPE_RANGER:	return (*Itr).second->m_nRanger;
	case AUCHARCLASS_TYPE_MAGE:		return (*Itr).second->m_nMage;
	default:						return -1;
	}
}