#if !defined(AFX_AGCMADMINDLGXT_GAME_H__62B19665_A060_4D55_88AE_89E551AEE0ED__INCLUDED_)
#define AFX_AGCMADMINDLGXT_GAME_H__62B19665_A060_4D55_88AE_89E551AEE0ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Game.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Game dialog

#ifdef _XTP_INCLUDE_CONTROLS
#define CPropertyGridDlgBase CXTResizeDialog
#else
#define CPropertyGridDlgBase CDialog
#endif

class AgcmAdminDlgXT_Game : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Game(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Game)
	enum { IDD = IDD_GAME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Game)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	void Lock();
	void Unlock();


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Game)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_GAME_H__62B19665_A060_4D55_88AE_89E551AEE0ED__INCLUDED_)
