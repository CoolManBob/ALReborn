#if !defined(AFX_UITWPTREEDIALOG_H__E09554EA_B3E2_4CAD_83B3_80F9F9B14BBC__INCLUDED_)
#define AFX_UITWPTREEDIALOG_H__E09554EA_B3E2_4CAD_83B3_80F9F9B14BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPTreeDialog.h : header file
//

#include "AgcmUIManager2.h"
#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPTreeDialog dialog

class UITWPTreeDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPTreeDialog)

private:
	AcUITree *		m_pcsTree;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPTreeDialog();
	~UITWPTreeDialog();

// Dialog Data
	//{{AFX_DATA(UITWPTreeDialog)
	enum { IDD = IDD_UIT_WP_TREE };
	CComboBox	m_csButtonClose;
	CComboBox	m_csButtonOpen;
	CComboBox	m_csUDDepth;
	long	m_lItemNum;
	BOOL	m_bStartAtBottom;
	UINT	m_ulItemColumn;
	long	m_lDepthMargin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPTreeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPTreeDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPTREEDIALOG_H__E09554EA_B3E2_4CAD_83B3_80F9F9B14BBC__INCLUDED_)
