#pragma once

#include "DirectoryChanges.h"
#include <list>
using namespace std;

class CPatcher2Dlg;

class CDirectoryChangeHandler_LB
	:	public	CDirectoryChangeHandler
{
private:
	typedef std::list<CString> FileListInfo;

public:
	CDirectoryChangeHandler_LB	( CListBox& listbox );
	~CDirectoryChangeHandler_LB	( VOID );

	FileListInfo&		GetList( VOID )	{	return m_FileInfoList;	}

protected:
	void On_FileAdded(const CString & strFileName);
	void On_FileModified(const CString & strFileName);
	void On_FileRemoved(const CString & strFileName);
	void On_FileNameChanged(const CString & strOldFileName, const CString & strNewFileName);

	void On_WatchStarted(DWORD dwError, const CString & strDirectoryName);
	void On_WatchStopped(const CString & strDirectoryName);

	BOOL _start	( const CString& strFileName );
	void _end	( VOID );

	CListBox&		m_ListBox;
	CString			m_LastFileName;
	FileListInfo					m_FileInfoList;
	CPatcher2Dlg*	m_pDlg;
};
