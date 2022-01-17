#include "StdAfx.h"
#include "DirectoryChangeHandler_LB.h"

#include "resource.h"
#include "Patcher2Dlg.h"

CDirectoryChangeHandler_LB::CDirectoryChangeHandler_LB( CListBox& listbox )
: m_ListBox( listbox )
{
	m_pDlg	=	NULL;
}

CDirectoryChangeHandler_LB::~CDirectoryChangeHandler_LB(void)
{
}

void CDirectoryChangeHandler_LB::On_FileAdded(const CString & strFileName)
{
	if( _start( strFileName ) )		
	{
		//m_pDlg->RealTimeSensorFileAdd( strFileName );
		m_pDlg->m_FileUpdateList.push_back( strFileName.GetString() );
	}
}

void CDirectoryChangeHandler_LB::On_FileRemoved(const CString & strFileName)
{
}

void CDirectoryChangeHandler_LB::On_FileModified(const CString & strFileName)
{
	if( _start( strFileName ) )		
	{
		//m_pDlg->RealTimeSensorFileAdd( strFileName );
		m_pDlg->m_FileUpdateList.push_back( strFileName.GetString() );
	}
}

void CDirectoryChangeHandler_LB::On_FileNameChanged(const CString & strOldFileName, const CString & strNewFileName)
{
}

void CDirectoryChangeHandler_LB::On_WatchStarted(DWORD dwError, const CString & strDirectoryName)
{
	m_pDlg		=	static_cast< CPatcher2Dlg* >(m_ListBox.GetParent() );
}

void CDirectoryChangeHandler_LB::On_WatchStopped(const CString & strDirectoryName)
{
	m_pDlg->UpdateFileList();

	m_pDlg		=	NULL;
}

BOOL CDirectoryChangeHandler_LB::_start( const CString& FileName )
{
	if( m_LastFileName	==	FileName )	 
		return FALSE;

	if( FileName.Find( ".svn" ) != -1 )	
		return FALSE;

	if( FileName.Find( ".tmp" ) != -1 )	
		return FALSE;

	if( FileName.Find( "." ) == -1 )	
		return FALSE;

	m_LastFileName		=	FileName;
	return TRUE;
}

void CDirectoryChangeHandler_LB::_end( VOID )
{
	
}