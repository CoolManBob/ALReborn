#if !defined(AFX_UITWPCOMBODIALOG_H__ED5CB90A_B88B_4B4A_8229_D001828D4DF3__INCLUDED_)
#define AFX_UITWPCOMBODIALOG_H__ED5CB90A_B88B_4B4A_8229_D001828D4DF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPComboDialog.h : header file
//

#include "UITPropertyPage.h"

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPComboDialog dialog

class UITWPComboDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPComboDialog)

private:
	AcUICombo *		m_pcsCombo;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPComboDialog();
	~UITWPComboDialog();

// Dialog Data
	//{{AFX_DATA(UITWPComboDialog)
	enum { IDD = IDD_UIT_WP_COMBO };
	CComboBox	m_csImageBottom;
	CComboBox	m_csImageClick;
	CComboBox	m_csImageMouse;
	CComboBox	m_csImageNormal;
	long	m_lLineHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPComboDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPComboDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPCOMBODIALOG_H__ED5CB90A_B88B_4B4A_8229_D001828D4DF3__INCLUDED_)
