#if !defined(AFX_UITWPGRIDDIALOG_H__1473E0B7_DADB_44D4_8686_83C1605441AC__INCLUDED_)
#define AFX_UITWPGRIDDIALOG_H__1473E0B7_DADB_44D4_8686_83C1605441AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPGridDialog.h : header file
//

#include "AgcmUIManager2.h"
#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPGridDialog dialog

class UITWPGridDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPGridDialog)

private:
	AcUIGrid *		m_pcsGrid;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPGridDialog();
	~UITWPGridDialog();

// Dialog Data
	//{{AFX_DATA(UITWPGridDialog)
	enum { IDD = IDD_UIT_WP_GRID };
	CComboBox	m_csGridList;
	long	m_lStartX;
	long	m_lStartY;
	long	m_lGapX;
	long	m_lGapY;
	long	m_lItemWidth;
	long	m_lItemHeight;
	BOOL	m_bMovableItem;
	BOOL	m_bGridItemBottomCount;
	BOOL	m_bMoveItemCopy;
	BOOL	m_bDrawImageForeground;
	CComboBox	m_csReusableDisplayList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPGridDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPGridDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPGRIDDIALOG_H__1473E0B7_DADB_44D4_8686_83C1605441AC__INCLUDED_)
