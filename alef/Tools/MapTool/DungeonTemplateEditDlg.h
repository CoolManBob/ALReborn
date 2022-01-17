#if !defined(AFX_DUNGEONTEMPLATEEDITDLG_H__09992E12_A0F0_4DBC_9B0A_B38EC2AA8555__INCLUDED_)
#define AFX_DUNGEONTEMPLATEEDITDLG_H__09992E12_A0F0_4DBC_9B0A_B38EC2AA8555__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DungeonTemplateEditDlg.h : header file
//

//#include "MainFrm.h"
/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateEditDlg dialog
struct CDungeonTemplate;

class CDungeonTemplateEditDlg : public CDialog
{
// Construction
public:
	CDungeonTemplateEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDungeonTemplateEditDlg)
	enum { IDD = IDD_DUNGEON_TEMPLATE_EDIT };
	CTreeCtrl	m_ctlTree;
	CString	m_strTemplateName;
	float	m_afHeight[ 12 ];
	//}}AFX_DATA

	class CCategory
	{
	public:
		CString		str;
		HTREEITEM	pos;
	};
	vector< CCategory >	m_listCategory;

	CDungeonTemplate	* m_pTemplate;
	AuList< void * >	m_listDelete;

	// 창띄우기.. 템플릿을 인자로 받음..
	int		DoModal( CDungeonTemplate * pTemplate );

	BOOL	LoadCategory();
	BOOL	CopyTree( CTreeCtrl * pTree , HTREEITEM root, int level);
	HTREEITEM SearchItemText(const char *strName, HTREEITEM root = TVI_ROOT);

	void	UpdateControls();
	void	CONTROL_UPDATE( UINT32 uID );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDungeonTemplateEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDungeonTemplateEditDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CDungeonWnd;
/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateSelectDlg dialog

class CDungeonTemplateSelectDlg : public CDialog
{
// Construction
public:
	CDungeonTemplateSelectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDungeonTemplateSelectDlg)
	enum { IDD = IDD_DUNGEON_TEMPLATE_SELECT };
	CListBox	m_ctlTemplateList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDungeonTemplateSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	BOOL	m_bListUpdated;
public:		BOOL	GetListUpdated(){ return m_bListUpdated; }
			void	SetListUpdate(){ m_bListUpdated = TRUE; }


public:
	CDungeonWnd *	m_pDungeonWnd;

	int	AddTemplate( CDungeonTemplate * pTemplate );
		// 리턴 : 리스트박스에 인덱스.

	int	m_nTemplateID;


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDungeonTemplateSelectDlg)
	afx_msg void OnEdittemplate();
	afx_msg void OnAddtemplate();
	afx_msg void OnDeleteTemplate();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUNGEONTEMPLATEEDITDLG_H__09992E12_A0F0_4DBC_9B0A_B38EC2AA8555__INCLUDED_)
