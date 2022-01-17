#if !defined(AFX_SAVEDLG_H__E8D34011_5851_45C8_BDF2_5D09783641DE__INCLUDED_)
#define AFX_SAVEDLG_H__E8D34011_5851_45C8_BDF2_5D09783641DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveDlg.h : header file
//

#include "AuList.h"
#include "UITileList_PluginBase.h"
/////////////////////////////////////////////////////////////////////////////
// CSaveDlg dialog

class CSaveDlg : public CDialog
{
// Construction
public:
	CSaveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveDlg)
	enum { IDD = IDD_SAVE };
	BOOL	m_bMapInfo			;
	BOOL	m_bObjectList		;
	BOOL	m_bObjectTemplate	;
	BOOL	m_bTileList			;
	BOOL	m_bTileVertexColor	;
	BOOL	m_bDungeonData		;
	//}}AFX_DATA

	struct	PluginSave
	{
		PluginSave()
		{
			pButton	= NULL;
			pPlugin	= NULL;
		}
		BOOL	bSave		;
		CString	strName		;
		CButton	* pButton	;
		CUITileList_PluginBase * pPlugin ;
	};

	AuList< PluginSave >	m_listPlugin;
	BOOL	AddPluginSaveData( BOOL bSaveDefault , char * pStr , CUITileList_PluginBase * pPlugin );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveDlg)
	afx_msg void OnNo();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEDLG_H__E8D34011_5851_45C8_BDF2_5D09783641DE__INCLUDED_)
