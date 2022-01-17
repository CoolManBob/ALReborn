// MapTool.h : main header file for the MapTool application
//

#if !defined(AFX_MapTool_H__857E764B_7EAD_48AC_BF5A_0B454E606719__INCLUDED_)
#define AFX_MapTool_H__857E764B_7EAD_48AC_BF5A_0B454E606719__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"         // main symbols
#pragma warning(disable:4786) //disable linker key > 255 chars long (for stl map)
#include <map>
#include "Renderware.h"
/////////////////////////////////////////////////////////////////////////////
// CMapToolApp:
// See MapTool.cpp for the implementation of this class
//
#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include <skeleton.h>
#include <menu.h>
#include <camera.h>
#include <AgcEngine.h>

#include "ProgressDlg.h"

class CMapToolApp : public CWinApp
{
public:
	CMapToolApp();
	virtual           ~CMapToolApp();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapToolApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL
	
	// Implementation
	
public:
	CRecentFileList*  GetRecentFileList();
	void              AppendBackslash(CString* szDirName);
	void              ExtractFileTitle(const CString& szFileName, CString& szFileTitle);
	CRenderWare&      GetRenderWare();
	
	CProgressDlg	* m_pProgressDlg;
	void	SetStartUpProcess( int nPercent );
	
	CString	m_strTitle;

	//{{AFX_MSG(CMapToolApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
protected:
	BOOL				m_bFailSelecting;
	CRenderWare         m_RenderWare;
	static int CALLBACK BrowseProc(HWND hWnd, UINT nMsg, LPARAM, LPARAM lpData);
};

extern CMapToolApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MapTool_H__857E764B_7EAD_48AC_BF5A_0B454E606719__INCLUDED_)
