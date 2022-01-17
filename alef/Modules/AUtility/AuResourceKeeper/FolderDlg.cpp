/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CFolderDialog  - Folder Selection Dialog Class	
	http://www.codeproject.com/dialog/cfolderdialog.asp

NOTES:
	Copyright(C) Armen Hakobyan, 2002
	mailto:armen.h@web.am
	
VERSION HISTORY:
	24 Mar 2002 - First release
	30 Mar 2003 - Some minor changes
				- Added missing in old Platform SDK new flag definitions  
				- Added support for both MFC 6.0 and 7.0
				- Added OnIUnknown handler for Windows XP folder filtration
				- Added SetExpanded and SetOKText and GetSelectedFolder functions
	24 May 2003 - Added OnSelChanged implementation
	14 Jul 2003 - Added custom filtration for Windows XP
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FolderDlg.h"

/////////////////////////////////////////////////////////////////////////////

#ifndef BFFM_VALIDATEFAILED
	#ifndef UNICODE
		#define BFFM_VALIDATEFAILED		3
	#else
		#define BFFM_VALIDATEFAILED		4	
	#endif
#endif

#ifndef BFFM_IUNKNOWN
	#define BFFM_IUNKNOWN				5
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog

IMPLEMENT_DYNAMIC( CFolderDialog, CDialog )

CFolderDialog::CFolderDialog( IN LPCTSTR lpszTitle	 /*NULL*/,
							  IN LPCTSTR lpszSelPath /*NULL*/,
							  IN CWnd*	 pParentWnd	 /*NULL*/,
							  IN UINT	 uFlags		 /*BIF_RETURNONLYFSDIRS*/ )
			 : CCommonDialog( pParentWnd )
			 , m_hWnd( NULL )
{
	::ZeroMemory( m_szFolPath, MAX_PATH );
	::ZeroMemory( m_szSelPath, MAX_PATH );
	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );

	if( lpszSelPath != NULL )
		SetSelectedFolder( lpszSelPath );
	
	// Fill data	
	m_bi.hwndOwner	= pParentWnd->GetSafeHwnd();
	m_bi.pidlRoot	= NULL;	
	m_bi.lpszTitle	= lpszTitle;
	m_bi.ulFlags	= uFlags;
	m_bi.lpfn		= (BFFCALLBACK)BrowseCallbackProc;
	m_bi.lParam		= (LPARAM)this;

	// The size of this buffer is assumed to be MAX_PATH bytes
	m_bi.pszDisplayName = new TCHAR[ MAX_PATH ];	
	::ZeroMemory( m_bi.pszDisplayName, ( MAX_PATH * sizeof( TCHAR ) ) );
}

CFolderDialog::~CFolderDialog( void )
{	
	_delete2( m_bi.pszDisplayName );	
	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );	
}

BEGIN_MESSAGE_MAP( CFolderDialog, CCommonDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog message handlers

#if ( _MFC_VER < 0x0700 )
	INT		CFolderDialog::DoModal( void )
#else
	INT_PTR CFolderDialog::DoModal( void )
#endif
{
	ASSERT_VALID( this );	
	ASSERT( m_bi.lpfn != NULL );
		
	m_bi.hwndOwner = PreModal();	
	INT_PTR nRet   = -1;
	LPITEMIDLIST lpItemIDList = ::SHBrowseForFolder( &m_bi );

	if( lpItemIDList != NULL )
	{
		if( ::SHGetPathFromIDList( lpItemIDList, m_szFolPath ) )
		{
			IMalloc* lpMalloc = NULL;
			if( SUCCEEDED( ::SHGetMalloc( &lpMalloc ) ) )
			{
				lpMalloc->Free( lpItemIDList );
				_releaseInterface( lpMalloc );
			}
			nRet = IDOK;
		}
		else
			nRet = IDCANCEL;

		lpItemIDList = NULL;
	}

	PostModal();
	return nRet;	
}

/////////////////////////////////////////////////////////////////////////////
// Overridables:

void CFolderDialog::OnInitialized( void )
{
	if( ::lstrlen( m_szSelPath ) > 0 )
		SetSelection( m_szSelPath );
}

void CFolderDialog::OnSelChanged( IN LPITEMIDLIST lpItemIDList )
{
	if( m_bi.ulFlags & BIF_STATUSTEXT )
	{
		TCHAR szSelFol[ MAX_PATH ] = { 0 };
		if( ::SHGetPathFromIDList( lpItemIDList, szSelFol ) )
			SetStatusText( szSelFol );
	}
}

INT CFolderDialog::OnValidateFailed( IN LPCTSTR /*lpszFolderPath*/ )
{	
	::MessageBeep( MB_ICONHAND );
	return 1; // Return 1 to leave dialog open, 0 - to end one
}

void CFolderDialog::OnIUnknown( IN IUnknown* /*lpIUnknown*/ )
{
}

/////////////////////////////////////////////////////////////////////////////
// Callback function used with the SHBrowseForFolder function. 

INT CALLBACK CFolderDialog::BrowseCallbackProc( HWND hWnd, 
					UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	CFolderDialog* pThis = (CFolderDialog*)lpData;
	pThis->m_hWnd = hWnd;
	INT nRet = 0;

	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		pThis->OnInitialized();
		break;
	case BFFM_SELCHANGED:
		pThis->OnSelChanged( (LPITEMIDLIST)lParam );
		break;
	case BFFM_VALIDATEFAILED:
		nRet = pThis->OnValidateFailed( (LPCTSTR)lParam );
		break;
	case BFFM_IUNKNOWN:
		pThis->OnIUnknown( (IUnknown*)lParam );
		break;
	default:
		ASSERT( FALSE );
		break;
	}

	pThis->m_hWnd = NULL;
	return nRet;	
}

/////////////////////////////////////////////////////////////////////////////

void CFolderDialog::SetExpanded( IN LPCTSTR lpszFolderPath )
{
	ASSERT( m_hWnd != NULL );
	
	USES_CONVERSION;	
	::SendMessage( m_hWnd, BFFM_SETEXPANDED, (WPARAM)TRUE, 
		(LPARAM)(LPCWSTR)T2W( const_cast<LPTSTR>( lpszFolderPath ) ) );
}

void CFolderDialog::SetOKText( IN LPCTSTR lpszText )
{
	ASSERT( m_hWnd != NULL );
	
	USES_CONVERSION;
	::SendMessage( m_hWnd, BFFM_SETOKTEXT, (WPARAM)0, 
		(LPARAM)(LPCWSTR)T2W( const_cast<LPTSTR>( lpszText ) ) );
}


/////////////////////////////////////////////////////////////////////////////