#pragma once

//---------------------------- CBrowseForFolder ---------------------
class CBrowseForFolder
{
public:
	CBrowseForFolder();
	virtual ~CBrowseForFolder()		{			}

	void SetStatusBar(bool val);
	void SetEditBox(bool val);

	bool GetFolder(CString &returnPath, const char *i_InitialDirectory);

protected:
	void GetPidl(CString name, LPITEMIDLIST);

	static int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

public:
	HWND		hWndOwner;			// parent's HWND
	int			Image;				// the same as Image variable in the BROWSEINFO structure ( see help )
	CString		strStartupDir;	// directory to select	
	CString		strTitle;		// title of the browse dialog
	CString		strDirRoot;		// directory to explore from

protected:
	UINT		flags;
};

//---------------------------- CDlgList ---------------------
class CDlgList : public CDialog
{
protected:
	struct	ListCtrlAndSubItem
	{
		ListCtrlAndSubItem(CListCtrl* pListCtrl, INT nSubItem) : m_pListCtrl(pListCtrl) , m_nSubItem(nSubItem)	{		}

		CListCtrl*	m_pListCtrl;
		INT			m_nSubItem;
	};

	BOOL			m_bInit;
	const CPoint	ptEdit;
	const CPoint	PTLIST;	

	static INT	CALLBACK CB_CmpByInt_Up(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem);
	static INT	CALLBACK CB_CmpByInt_Down(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem);
	static INT	CALLBACK CB_CmpByString_Up(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem);
	static INT	CALLBACK CB_CmpByString_Down(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem);

public:
	BOOL	bInit(void)const { return m_bInit; };

// Construction
public:
	CDlgList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgList)
	enum { IDD = IDD_DLG_LIST };
	CListCtrl	m_ctrlListData;
	CEdit		m_ctrlEditInfo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgList)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK()		{		}
	virtual void OnCancel()	{		}
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgList)
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
