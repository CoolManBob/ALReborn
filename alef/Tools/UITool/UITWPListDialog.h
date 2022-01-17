#if !defined(AFX_UITWPLISTDIALOG_H__828518E9_C8D3_4A7F_B209_F53FA4C90E2E__INCLUDED_)
#define AFX_UITWPLISTDIALOG_H__828518E9_C8D3_4A7F_B209_F53FA4C90E2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPListDialog.h : header file
//

#include "AgcmUIManager2.h"
#include "UITPropertyPage.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPListDialog dialog

class UITWPListDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPListDialog)

private:
	AcUIList *		m_pcsList;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPListDialog();
	~UITWPListDialog();

// Dialog Data
	//{{AFX_DATA(UITWPListDialog)
	enum { IDD = IDD_UIT_WP_LIST };
	long	m_lItemNum;
	BOOL	m_bStartAtBottom;
	UINT	m_ulItemColumn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPListDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPListDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_csImageSelection;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPLISTDIALOG_H__828518E9_C8D3_4A7F_B209_F53FA4C90E2E__INCLUDED_)
