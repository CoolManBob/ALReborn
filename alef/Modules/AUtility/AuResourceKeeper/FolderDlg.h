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

#ifndef __FOLDERDLG_H__
#define __FOLDERDLG_H__
#if defined( _MSC_VER ) && ( _MSC_VER >= 1020 )
	#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXDLGS_H__
	#include < AfxDlgs.h >
#endif

#ifndef __ATLCONV_H__
	#include < AtlConv.h >	// MBCS/Unicode Conversion Macros
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef BFFM_SETOKTEXT					// Version 5.0 or later
	#define BFFM_SETOKTEXT				( WM_USER + 105 )	// Unicode only, req. BIF_USENEWUI
	#define BFFM_SETEXPANDED			( WM_USER + 106 )	// Unicode only, req. BIF_USENEWUI
#endif

#ifndef BIF_NEWDIALOGSTYLE				// Version 5.0 or later
	#define BIF_NEWDIALOGSTYLE			0x0040	
	#define BIF_BROWSEINCLUDEURLS		0x0080
	#define BIF_UAHINT					0x0100				// Req. BIF_NEWDIALOGSTYLE
	#define BIF_NONEWFOLDERBUTTON		0x0200
	#define BIF_NOTRANSLATETARGETS		0x0400	
	#define BIF_SHAREABLE				0x8000				// Req. BIF_USENEWUI
	#define BIF_USENEWUI				( BIF_NEWDIALOGSTYLE | BIF_EDITBOX )	
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef		_delete2
	#define _delete2( p )			{ if( p != NULL ){ delete[] p; p = NULL; } }
#endif

#ifndef		_releaseInterface
	#define	_releaseInterface( p )	{ if( p != NULL ){ p->Release(); p = NULL; } }
#endif

#ifndef _NOINLINE
	#if ( _MSC_VER < 1700 )
		#define _NOINLINE	
	#else
		#define _NOINLINE			__declspec( noinline )	
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////

class CFolderDialog : public CCommonDialog
{
	DECLARE_DYNAMIC( CFolderDialog )

public:
	CFolderDialog(	IN LPCTSTR	lpszTitle	= NULL, 
					IN LPCTSTR	lpszSelPath	= NULL,
					IN CWnd*	pParentWnd	= NULL,
					IN UINT		uFlags		= BIF_RETURNONLYFSDIRS );
	virtual ~CFolderDialog( void );

public:
	#if ( _MFC_VER < 0x0700 )
		virtual INT		DoModal( void );
	#else
		virtual INT_PTR DoModal( void );
	#endif
	
public:
	AFX_INLINE BOOL		SetSelectedFolder( IN LPCTSTR lpszPath );
	AFX_INLINE LPCTSTR	GetFolderPath( void )  const;
	AFX_INLINE LPCTSTR	GetFolderName( void )  const;
	AFX_INLINE INT		GetFolderImage( void ) const;		
	AFX_INLINE LPCTSTR	GetSelectedFolder( void ) const;
	AFX_INLINE BROWSEINFO&		 GetBI( void );
	AFX_INLINE const BROWSEINFO& GetBI( void ) const;

protected:	
	BROWSEINFO	m_bi;
	TCHAR		m_szSelPath[ MAX_PATH ];
	TCHAR		m_szFolPath[ MAX_PATH ];

protected:
	DECLARE_MESSAGE_MAP()
	
	virtual void OnInitialized( void );
	virtual void OnSelChanged( IN LPITEMIDLIST  lpItemIDList );
	virtual INT	 OnValidateFailed( IN LPCTSTR /*lpszFolderPath*/ );		

protected: // Windows XP or later
	virtual void OnIUnknown( IN IUnknown* /*lpIUnknown*/ );
	
protected: // Shell version 5.0 or later:
	_NOINLINE void SetExpanded( IN LPCTSTR lpszFolderPath );
	_NOINLINE void SetOKText( IN LPCTSTR lpszText );	

protected: // Valid to call only from the above handlers
	AFX_INLINE void EnableOK( IN BOOL bEnable = TRUE );
	AFX_INLINE void SetSelection( IN LPITEMIDLIST lpItemIDList );
	AFX_INLINE void SetSelection( IN LPCTSTR lpszFolderPath );	
	AFX_INLINE void SetStatusText( IN LPCTSTR lpszText );

protected: // Shell version 5.0 or later:
	AFX_INLINE void SetExpanded( IN LPITEMIDLIST lpItemIDList );
	
private:
	HWND	m_hWnd; // used only in the callback function
	static INT CALLBACK BrowseCallbackProc(
		HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData
	);
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CFolderDialog::SetSelectedFolder( IN LPCTSTR lpszPath )
	{ ASSERT ( lpszPath != NULL ); 
		return( ::lstrcpy( m_szSelPath, lpszPath ) != NULL ); }

AFX_INLINE LPCTSTR CFolderDialog::GetSelectedFolder( void ) const
	{ return m_szSelPath; }

AFX_INLINE BROWSEINFO& CFolderDialog::GetBI( void )
	{ return m_bi; }

AFX_INLINE const BROWSEINFO& CFolderDialog::GetBI( void ) const
	{ return m_bi; }

/////////////////////////////////////////////////////////////////////////////
// Filled after a call to DoModal

AFX_INLINE LPCTSTR CFolderDialog::GetFolderPath( void ) const
	{ return m_szFolPath; }

AFX_INLINE LPCTSTR CFolderDialog::GetFolderName( void ) const
	{ return m_bi.pszDisplayName; }

AFX_INLINE INT CFolderDialog::GetFolderImage( void ) const
	{ return m_bi.iImage; }

/////////////////////////////////////////////////////////////////////////////
// Commands, valid to call only from handlers

AFX_INLINE void CFolderDialog::EnableOK( IN BOOL bEnable /*TRUE*/ )
	{ ASSERT( m_hWnd != NULL ); 
		::SendMessage( m_hWnd, BFFM_ENABLEOK, (WPARAM)bEnable, 0L );}

AFX_INLINE void CFolderDialog::SetSelection( IN LPITEMIDLIST lpItemIDList )
	{ ASSERT( m_hWnd != NULL ); 
		::SendMessage( m_hWnd, BFFM_SETSELECTION, (WPARAM)FALSE, (LPARAM)lpItemIDList ); }

AFX_INLINE void CFolderDialog::SetSelection( IN LPCTSTR lpszFolderPath )
	{ ASSERT( m_hWnd != NULL );
		::SendMessage( m_hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpszFolderPath ); }

AFX_INLINE void CFolderDialog::SetStatusText( IN LPCTSTR lpszText )
	{ ASSERT( m_hWnd != NULL );
		::SendMessage( m_hWnd, BFFM_SETSTATUSTEXT, (WPARAM)0, (LPARAM)lpszText ); }

// Shell version 5.0 or later:

AFX_INLINE void CFolderDialog::SetExpanded( IN LPITEMIDLIST lpItemIDList )
	{ ASSERT( m_hWnd != NULL ); 
		::SendMessage( m_hWnd, BFFM_SETEXPANDED, (WPARAM)FALSE, (LPARAM)lpItemIDList ); }

/////////////////////////////////////////////////////////////////////////////
#endif // __FOLDERDLG_H__
/////////////////////////////////////////////////////////////////////////////