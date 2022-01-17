#if !defined(AFX_UITWPSCROLL_H__F13DF65C_49A0_47AD_93CC_3A94D1F533EF__INCLUDED_)
#define AFX_UITWPSCROLL_H__F13DF65C_49A0_47AD_93CC_3A94D1F533EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPScroll.h : header file
//

#include "AgcmUIManager2.h"
#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPScroll dialog

class UITWPScroll : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPScroll)

private:
	AcUIScroll *	m_pcsScroll;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPScroll();
	~UITWPScroll();

// Dialog Data
	//{{AFX_DATA(UITWPScroll)
	enum { IDD = IDD_UIT_WP_SCROLL };
	CComboBox	m_csButtonDown;
	CComboBox	m_csButtonUp;
	CComboBox	m_csCurrentDisplay;
	CComboBox	m_csCurrentUserData;
	CComboBox	m_csMaxDisplay;
	CComboBox	m_csMaxUserData;
	BOOL	m_bVScroll;
	long	m_lMinPosition;
	long	m_lMaxPosition;
	float	m_fChangeUnit;
	BOOL	m_bMoveByUnit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPScroll)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPScroll)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeUITScrollMaxUD();
	afx_msg void OnSelchangeUITScrollCurrentUD();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPSCROLL_H__F13DF65C_49A0_47AD_93CC_3A94D1F533EF__INCLUDED_)
