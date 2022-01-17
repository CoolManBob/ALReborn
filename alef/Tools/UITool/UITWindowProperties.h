#if !defined(AFX_UITWINDOWPROPERTIES_H__8CE188EB_B33F_40EE_BAAB_8E7D4BD949A4__INCLUDED_)
#define AFX_UITWINDOWPROPERTIES_H__8CE188EB_B33F_40EE_BAAB_8E7D4BD949A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWindowProperties.h : header file
//

#include "AgcmUIManager2.h"

#include "UITWinPropertyDialog.h"
#include "UITWPMapDialog.h"
#include "UITWPStyleDialog.h"
#include "UITWPAnimationDialog.h"

#include "UITPropertyPage.h"

#include "UITWPEditDialog.h"
#include "UITWPButtonDialog.h"
#include "UITWPGridDialog.h"
#include "UITWPBarDialog.h"
#include "UITWPListDialog.h"
#include "UITWPSkillTreeDialog.h"
#include "UITWPScroll.h"
#include "UITWPComboDialog.h"
#include "UITWPTreeDialog.h"
#include "UITWPClockDialog.h"

#include "UITWPUIDialog.h"

/////////////////////////////////////////////////////////////////////////////
// UITWindowProperties

class UITWindowProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(UITWindowProperties)
private:
	AcUIBase *				m_pcsBase;
	AgcdUI *				m_pcsUI;
	AgcdUIControl *			m_pcsControl;

public:
	UITWinPropertyDialog	m_dlgWinProperty;
	UITWPMapDialog			m_dlgMap;
	UITWPStyleDialog		m_dlgStyle;
	UITWPAnimationDialog	m_dlgAnimation;

	UITPropertyPage *		m_apcsDialogs[AcUIBase::TYPE_COUNT];

	UITWPEditDialog			m_dlgEdit;
	UITWPButtonDialog		m_dlgButton;
	UITWPGridDialog			m_dlgGrid;
	UITWPBarDialog			m_dlgBar;
	UITWPListDialog			m_dlgList;
	UITWPSkillTreeDialog	m_dlgSkillTree;
	UITWPScroll				m_dlgScroll;
	UITWPComboDialog		m_dlgCombo;
	UITWPTreeDialog			m_dlgTree;
	UITWPClockDialog		m_dlgClock;

	UITWPUIDialog			m_dlgUI;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI = NULL, AgcdUIControl *pcsControl = NULL);
	BOOL UpdateDataFromDialog();

	VOID InitProperty();

	UITWindowProperties(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	UITWindowProperties(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITWindowProperties)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~UITWindowProperties();

	// Generated message map functions
protected:
	//{{AFX_MSG(UITWindowProperties)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWINDOWPROPERTIES_H__8CE188EB_B33F_40EE_BAAB_8E7D4BD949A4__INCLUDED_)
