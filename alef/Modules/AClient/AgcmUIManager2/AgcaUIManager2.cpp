// AgcaUIManager2.cpp: implementation of the AgcaUIManager2 class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcaUIManager2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcdUI*	AgcaUIManager2::AddUI( AgcdUI *pcsUI )
{
	if( AddObject( ( PVOID )&pcsUI, pcsUI->m_lID, pcsUI->m_szUIName ) )	return pcsUI;
	return NULL;
}

AgcdUI*	AgcaUIManager2::GetUI( INT32 lID )
{
	AgcdUI** ppcsWindow = ( AgcdUI** )GetObject( lID );
	if( !ppcsWindow ) return NULL;
	return *ppcsWindow;
}

AgcdUI*	AgcaUIManager2::GetUI( CHAR *szName )
{
	AgcdUI** ppcsWindow = ( AgcdUI** )GetObject( szName );
	if( !ppcsWindow ) return NULL;
	return *ppcsWindow;
}

BOOL AgcaUIManager2::RemoveUI( INT32 lID, CHAR *szName )
{
	return RemoveObject( lID, szName );
}

