#if !defined(AFX_AGCMADMINDLGXT_MOVE_H__FF841710_FA02_4CB7_9FF0_502A8EF611C9__INCLUDED_)
#define AFX_AGCMADMINDLGXT_MOVE_H__FF841710_FA02_4CB7_9FF0_502A8EF611C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Move.h : header file
//

class AgcdAdminMovePlace;

typedef enum _eAgcmAdminDlgXT_MoveMode
{
	AGCMADMINDLGXT_MOVEMODE_NORMAL = 0,
	AGCMADMINDLGXT_MOVEMODE_ADD,
	AGCMADMINDLGXT_MOVEMODE_REMOVE,
} eAgcmAdminDlgXT_MoveMode;

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Move dialog

class AgcmAdminDlgXT_Move : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Move(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Move)
	enum { IDD = IDD_MOVE };
	CXTFlatComboBox	m_csMovePlaceCB;
	CXTButton	m_csMoveBtn;
	CString	m_szMovePC;
	CString	m_szMoveTargetPC;
	CString	m_szMoveX;
	CString	m_szMoveY;
	CString	m_szMoveZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Move)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	// Callback Function Pointer
	ADMIN_CB m_pfCBMove;

	// MovePlace List
	list<AgcdAdminMovePlace*> m_listMovePlace;

	eAgcmAdminDlgXT_MoveMode m_eMoveMode;

	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();
	
	// Callback Function Registration
	BOOL SetCBMove(ADMIN_CB pfCBMove);

	void Lock();
	void Unlock();

	BOOL SetMovePC(CString& szName);
	BOOL SetMoveTargetPC(CString& szName);
	BOOL SetPlace(CString& szName);
	BOOL SetPosition(float fX, float fY, float fZ);

	// Process
	BOOL ProcessMove();
	BOOL ProcessMove(CString& szName, CString& szTargetName, float fX, float fY, float fZ);

	// Make Main Window MovePlaceCB
	BOOL MakeMainWindowMovePlaceCB();

	// Move Place
	BOOL AddPlace(CString& szName, float fX, float fY, float fZ);
	BOOL RemovePlace(CString& szName);
	AgcdAdminMovePlace* GetMovePlace(CString& szName);
	AgcdAdminMovePlace* GetMovePlace(CHAR* szName);
	BOOL ClearMovePlaceList();

	list<AgcdAdminMovePlace*>* GetMovePlaceList();

	// Show
	BOOL ShowData();
	BOOL ShowMovePlaceCB();
	BOOL ClearMovePlaceCB();

	// Move Mode Change
	BOOL OnMoveModeChanged();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Move)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBMoveMoveGo();
	afx_msg void OnSelchangeCbMoveMovePlace();
	afx_msg void OnMoveFileClose();
	afx_msg void OnMoveEditNormal();
	afx_msg void OnMoveEditAdd();
	afx_msg void OnMoveEditRemove();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_MOVE_H__FF841710_FA02_4CB7_9FF0_502A8EF611C9__INCLUDED_)
