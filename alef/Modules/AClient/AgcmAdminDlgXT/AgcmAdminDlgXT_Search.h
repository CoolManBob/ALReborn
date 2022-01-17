/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Search dialog

class AgcmAdminDlgXT_Search : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Search(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Search)
	enum { IDD = IDD_SEARCH };
	CEdit	m_csSearchEdit;
	CListCtrl	m_csResultListView;
	CXTFlatComboBox	m_csTypeCB;
	CXTFlatComboBox	m_csFieldCB;
	CXTButton	m_csSearchBtn;
	CXTButton	m_csModifyBtn;
	CXTButton	m_csGoBtn;
	CXTButton	m_csGMBtn;
	CXTButton	m_csAddBtn;
	CString	m_szSearchName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Search)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	// Callback Function Pointer
	ADMIN_CB m_pfCBSearch;

	// Member Data
	stAgpdAdminCharData m_stCharData;
	list<stAgpdAdminSearchResult*> m_listSearchResult;

	CString m_szClickName;	// 한번 클릭하면 저장해 놓는다.

	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	void Lock();
	void Unlock();
	
	// Callback Function Regist
	BOOL SetCBSearch(ADMIN_CB pfCBSearch);

	// Operation
	BOOL SetCharData(stAgpdAdminCharData* pstCharData);
	stAgpdAdminCharData* GetCharData();
	BOOL SetResultList(PVOID pList);	// 일단 List 자체는 PVOID 형태로 받아온다.
	BOOL SetResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* pstSearchResult);	// 단일 아이템이 들어온다.
	BOOL IsInResultList(stAgpdAdminSearchResult* pstSearchResult);	// 이미 Result List 에 있는 지 확인한다.

	stAgpdAdminSearchResult* GetSearchResult(CString& szName);
	
	BOOL ProcessSearch(CHAR* szName);	// 외부에서 Search 함수를 부르고 싶을 때 사용한다.

	BOOL ShowData();
	BOOL ClearResultList();
	BOOL ClearResultListView();

protected:
	// Init ListView
	BOOL OnInitResultListView();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Search)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSearchAdd();
	afx_msg void OnBtnSearchGm();
	afx_msg void OnBtnSearchGo();
	afx_msg void OnBtnSearchModify();
	afx_msg void OnBtnSearchSearch();
	afx_msg void OnClose();
	afx_msg void OnClickLvSearchResult(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkLvSearchResult(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
