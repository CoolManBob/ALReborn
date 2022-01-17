#if !defined(AFX_AGCMADMINDLGXT_BAN_H__45091173_8AD7_4E6E_8633_04590588DCB0__INCLUDED_)
#define AFX_AGCMADMINDLGXT_BAN_H__45091173_8AD7_4E6E_8633_04590588DCB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Ban.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Ban dialog

class AgcmAdminDlgXT_Ban : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Ban(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Ban)
	enum { IDD = IDD_BAN };
	CButton	m_csChatBanCheckBox;
	CButton	m_csCharBanCheckBox;
	CButton	m_csAccBanCheckBox;
	CXTFlatComboBox	m_csChatBanTimeCB;
	CXTFlatComboBox	m_csCharBanTimeCB;
	CXTFlatComboBox	m_csAccBanTimeCB;
	CXTButton	m_csLogoutBtn;
	CXTButton	m_csCancelBtn;
	CXTButton	m_csApplyBtn;
	CString	m_szAccBanStart;
	CString	m_szCharBanStart;
	CString	m_szCharName;
	CString	m_szChatBanStart;
	CString	m_szAccName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Ban)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	// Callback Function Pointer
	ADMIN_CB m_pfCBBan;

	// stAgpdAdminBan
	stAgpdAdminBan m_stBan;

	CCriticalSection m_csLock;

public:
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	// Callback Fuction Registration
	BOOL SetCBBan(ADMIN_CB pfCBBan);
	
	void Lock();
	void Unlock();

	BOOL RequestBanData(CHAR* szCharName);
	BOOL SetBanData(stAgpdAdminBan* pstBan);

	BOOL ShowData();

	void ConvertDateTimeToString(INT32 lTimeStamp, char* szDate, INT32 lSize = 256);
	
	INT8 GetDefineTimeFromChatBanCBIndex(int iIndex);
	INT8 GetDefineTimeFromCharBanCBIndex(int iIndex);
	INT8 GetDefineTimeFromAccBanCBIndex(int iIndex);

	INT32 GetChatBanCBIndexFromDefineTime(INT8 cDefineTime);
	INT32 GetCharBanCBIndexFromDefineTime(INT8 cDefineTime);
	INT32 GetAccBanCBIndexFromDefineTime(INT8 cDefineTime);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Ban)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnChChatbanModify();
	afx_msg void OnChCharbanModify();
	afx_msg void OnChAccbanModify();
	afx_msg void OnSelchangeCbChatBanTime();
	afx_msg void OnSelchangeCbCharBanTime();
	afx_msg void OnSelchangeCbAccBanTime();
	afx_msg void OnBBanLogout();
	afx_msg void OnBBanCancel();
	afx_msg void OnBBanApply();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_BAN_H__45091173_8AD7_4E6E_8633_04590588DCB0__INCLUDED_)
