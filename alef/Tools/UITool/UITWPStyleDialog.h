#if !defined(AFX_UITWPSTYLEDIALOG_H__51E27669_D5C0_46C0_B78D_F514CFE7E7D0__INCLUDED_)
#define AFX_UITWPSTYLEDIALOG_H__51E27669_D5C0_46C0_B78D_F514CFE7E7D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPStyleDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPStyleDialog dialog

class UITWPStyleDialog : public CPropertyPage
{
	DECLARE_DYNCREATE(UITWPStyleDialog)

private:
	AcUIBase *			m_pcsBase			;
	AgcdUI *			m_pcsUI				;
	AgcdUIControl *		m_pcsControl		;

	AgcWindowProperty *	m_pstWinProperty	;
	AgcdUIBaseProperty *m_pstBaseProperty	;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPStyleDialog();
	~UITWPStyleDialog();

// Dialog Data
	//{{AFX_DATA(UITWPStyleDialog)
	enum { IDD = IDD_UIT_WP_STYLE };
	CEdit	m_csTooltip;
	CComboBox	m_csShowUD;
	CComboBox	m_csShowBoolean;
	CButton	m_csAutoAlign;
	BOOL	m_bUseInput;
	BOOL	m_bTopMost;
	BOOL	m_bMovable;
	BOOL	m_bVisible;
	BOOL	m_bModal;
	BOOL	m_bShrink;
	BOOL	m_bAutoAlign;
	CString	m_strTooltip;
	BOOL	m_bUseCondensation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPStyleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPStyleDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnUITApply();
	afx_msg void OnUITCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bImageClipping;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPSTYLEDIALOG_H__51E27669_D5C0_46C0_B78D_F514CFE7E7D0__INCLUDED_)
