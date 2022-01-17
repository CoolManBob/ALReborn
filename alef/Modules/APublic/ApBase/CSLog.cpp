// CSLog.cpp: implementation of the CCSLog class.
//
//////////////////////////////////////////////////////////////////////

#include "CSLog.h"

#include <stdio.h>

#pragma warning( push )
#pragma warning( disable : 4996 )
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool	CSLogManager::Push		( CCSLog	* pLog )
{
	ApAutoWriterLock	csLock( m_Lock );

	if( !GetIndicator() ) return false;

	if( m_vecLog.size() > 100 )
	{
		// 갯수초과.

		CSLogInfo * pLogInfo = &m_vecLog[ m_vecLog.size() - 1 ];
		if( pLogInfo->eState == CSLogInfo::STACKOVERFLOW )
		{
			pLogInfo->uSerial	++;
		}
		else
		{
			pLogInfo->eState = CSLogInfo::STACKOVERFLOW;
			pLogInfo->uSerial	= 0;
		}
		return false;
	}

	CSLogInfo	stTag;
	stTag.uSerial	= pLog->uSerial;
	stTag.eState	= CSLogInfo::ENTER;

	m_vecLog.push_back( stTag );
	return true;
}

bool	CSLogManager::Pop			( CCSLog	* pLog )
{
	ApAutoWriterLock	csLock( m_Lock );

	if( !GetIndicator() ) return false;

	CSLogInfo	stTag;

	// pLog가 깨졌을때를 대비함..
	try
	{
		stTag.uSerial	= pLog->uSerial;
		stTag.eState	= CSLogInfo::ESCAPE;
	}
	catch( ... )
	{
		stTag.uSerial	= 0;
		stTag.eState	= CSLogInfo::ESCAPEERROR;
	}

	if( m_vecLog.size() )
	{
		// 마지막걸지운다.
		m_vecLog.pop_back();
	}
	return true;
}

bool	CSLogManager::Print		( char * buf )
{
	
	CSLogManager	* pManager		= GetInstance();
	CSLogIndicator	* pIndicator	= pManager->GetIndicator();

	ApAutoWriterLock	csLock( pManager->m_Lock );

	char buf2[512];

	if( !pIndicator )
	{
		sprintf( buf2 , "Stack Log - Nothing\r\n" );
		strncat(buf, buf2, sizeof(buf) - strlen(buf) - 1);
		return true;
	}
	else
	{
		sprintf( buf2 , "Start Stack Log:::\r\n" );
		strncat(buf, buf2, sizeof(buf) - strlen(buf) - 1);
	}

	sprintf( buf2 , "<%s>:" , pIndicator->strIndicatorLog.c_str() );

	for( vector< CSLogInfo >::iterator iter = pManager->m_vecLog.begin();
		iter != pManager->m_vecLog.end();
		iter ++ )
	{
		CSLogInfo * pLogInfo = &*iter;
		
		switch( pLogInfo->eState )
		{
		case	CSLogInfo::NON_INITIALIZED	:	break;
		case	CSLogInfo::ENTER			:	sprintf( buf2 , "Enter %d\r\n, " , pLogInfo->uSerial );				break;
		case	CSLogInfo::ESCAPE			:	sprintf( buf2 , "Quit %d\r\n" , pLogInfo->uSerial );				break;
		case	CSLogInfo::ESCAPEERROR		:	sprintf( buf2 , "EscapeError\r\n" );								break;
		case	CSLogInfo::STACKOVERFLOW	:	sprintf( buf2 , "STACKOVERFLOW (%d)\r\n" , pLogInfo->uSerial);	break;
		}

		strncat(buf, buf2, sizeof(buf) - strlen(buf) - 1);
	}

	sprintf( buf2 , "\r\n<End Log>\r\n" );
	strncat(buf, buf2, sizeof(buf) - strlen(buf) - 1);
	return true;
}

#pragma warning( pop )