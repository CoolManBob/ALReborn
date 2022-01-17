#if !defined(AFX_AGCMSHRINESETTINGDLG_H__DF9175F7_783A_4CC3_BB6D_965A8AA79E8A__INCLUDED_)
#define AFX_AGCMSHRINESETTINGDLG_H__DF9175F7_783A_4CC3_BB6D_965A8AA79E8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmShrineSettingDlg.h : header file
//

#include "../resource.h"
#include "AgpmShrine.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmShrineSettingDlg dialog

class AgcmShrineDlg;

class AgcmShrineSettingDlg : public CDialog
{
// Construction
public:
	AgcmShrineSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmShrineSettingDlg)
	enum { IDD = IDD_SHRINE_DIALOG };
	CListBox	m_csListSkillList;
	CListBox	m_csListPosition;
	CComboBox	m_csComboUseCondition;
	CComboBox	m_csComboReactiveCondition;
	CComboBox	m_csComboActiveCondition;
	CString	m_strShrineName;
	UINT	m_unShrineLevel;
	CString	m_strSkillName;
	BOOL	m_bIsRandomPosition;
	CString	m_strActiveCondition;
	CString	m_strReactiveCondition;
	CString	m_strUseCondition;
	UINT	m_unMinActiveTime;
	UINT	m_unMaxActiveTime;
	CString	m_strPosition;
	UINT	m_unUseInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmShrineSettingDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmShrineSettingDlg)
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnButtonPosAdd();
	afx_msg void OnButtonPosDelete();
	afx_msg void OnButtonSkillAdd();
	afx_msg void OnButtonSkillDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	AgcmShrineDlg*			m_pcsAgcmShrineDlg;

	AgpdShrineTemplate*		m_pcsShrineTemplate;

public:
	BOOL					SetModule(AgcmShrineDlg	*pcsModule);

	BOOL					Init();
	BOOL					InitData(AgpdShrineTemplate *pcsShrineTemplate);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMSHRINESETTINGDLG_H__DF9175F7_783A_4CC3_BB6D_965A8AA79E8A__INCLUDED_)
