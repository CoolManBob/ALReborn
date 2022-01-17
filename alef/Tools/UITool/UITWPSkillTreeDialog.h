#if !defined(AFX_UITWPSKILLTREEDIALOG_H__4A54F1F8_87E0_4880_A2AD_A8FFD15B1036__INCLUDED_)
#define AFX_UITWPSKILLTREEDIALOG_H__4A54F1F8_87E0_4880_A2AD_A8FFD15B1036__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPSkillTreeDialog.h : header file
//

#include "UITPropertyPage.h"
#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPSkillTreeDialog dialog

class UITWPSkillTreeDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPSkillTreeDialog)

private:
	AcUISkillTree *	m_pcsSkillTree;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPSkillTreeDialog();
	~UITWPSkillTreeDialog();

// Dialog Data
	//{{AFX_DATA(UITWPSkillTreeDialog)
	enum { IDD = IDD_UIT_WP_SKILLTREE };
	CComboBox	m_csImage_2_2;
	CComboBox	m_csImage_1_2;
	CComboBox	m_csImage_1_10;
	CComboBox	m_csImage_3_3;
	CComboBox	m_csImage_2_3;
	CComboBox	m_csImage_1_3;
	CComboBox	m_csGridList;
	long	m_lStartX;
	long	m_lStartY;
	long	m_lGapX;
	long	m_lGapY;
	long	m_lItemWidth;
	long	m_lItemHeight;
	BOOL	m_bMovableItem;
	BOOL	m_bGridItemBottomCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPSkillTreeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPSkillTreeDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPSKILLTREEDIALOG_H__4A54F1F8_87E0_4880_A2AD_A8FFD15B1036__INCLUDED_)
