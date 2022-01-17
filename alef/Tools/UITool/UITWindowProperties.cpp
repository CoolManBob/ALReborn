// UITWindowProperties.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWindowProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWindowProperties

IMPLEMENT_DYNAMIC(UITWindowProperties, CPropertySheet)

UITWindowProperties::UITWindowProperties(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	InitProperty();
}

UITWindowProperties::UITWindowProperties(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	InitProperty();
}

UITWindowProperties::~UITWindowProperties()
{
}


BEGIN_MESSAGE_MAP(UITWindowProperties, CPropertySheet)
	//{{AFX_MSG_MAP(UITWindowProperties)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWindowProperties message handlers

VOID UITWindowProperties::InitProperty()
{
	memset(m_apcsDialogs, 0, sizeof(CPropertyPage *) * AcUIBase::TYPE_COUNT);

	m_apcsDialogs[AcUIBase::TYPE_EDIT]			= &m_dlgEdit;
	m_apcsDialogs[AcUIBase::TYPE_BUTTON]		= &m_dlgButton;
	m_apcsDialogs[AcUIBase::TYPE_GRID]			= &m_dlgGrid;
	m_apcsDialogs[AcUIBase::TYPE_SKILL_TREE]	= &m_dlgSkillTree;
	m_apcsDialogs[AcUIBase::TYPE_BAR]			= &m_dlgBar;
	m_apcsDialogs[AcUIBase::TYPE_LIST]			= &m_dlgList;
	m_apcsDialogs[AcUIBase::TYPE_SCROLL]		= &m_dlgScroll;
	m_apcsDialogs[AcUIBase::TYPE_SCROLL_BUTTON]	= &m_dlgButton;
	m_apcsDialogs[AcUIBase::TYPE_COMBO]			= &m_dlgCombo;
	m_apcsDialogs[AcUIBase::TYPE_TREE]			= &m_dlgTree;
	m_apcsDialogs[AcUIBase::TYPE_CLOCK]			= &m_dlgClock;

	AddPage(&m_dlgWinProperty);
	AddPage(&m_dlgMap);
	AddPage(&m_dlgStyle);
	AddPage(&m_dlgAnimation);

	m_pcsBase = NULL;
	m_pcsUI = NULL;
	m_pcsControl = NULL;
}

BOOL UITWindowProperties::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	if (pcsControl)
		m_dlgWinProperty.UpdateDataToDialog(pcsControl->m_szName, pcsBase, pcsUI, pcsControl);
	else if (pcsUI)
		m_dlgWinProperty.UpdateDataToDialog(pcsUI->m_szUIName, pcsBase, pcsUI, pcsControl);

	m_dlgMap.UpdateDataToDialog(pcsBase, pcsUI, pcsControl);
	m_dlgStyle.UpdateDataToDialog(pcsBase, pcsUI, pcsControl);
	m_dlgAnimation.UpdateDataToDialog(pcsBase, pcsUI, pcsControl);

	if (m_pcsBase && m_apcsDialogs[m_pcsBase->m_nType])
	{
		RemovePage(m_apcsDialogs[m_pcsBase->m_nType]);
	}

	if (pcsBase && m_apcsDialogs[pcsBase->m_nType])
	{
		AddPage(m_apcsDialogs[pcsBase->m_nType]);

		m_apcsDialogs[pcsBase->m_nType]->UpdateDataToDialog(pcsBase, pcsUI, pcsControl);
	}

	if (pcsUI && !pcsControl)
	{
		if (!m_pcsUI || m_pcsControl)
			AddPage(&m_dlgUI);

		m_dlgUI.UpdateDataToDialog(NULL, pcsUI, NULL);
	}
	else if (m_pcsUI && !m_pcsControl)
		RemovePage(&m_dlgUI);


	m_pcsBase = pcsBase;
	m_pcsUI = pcsUI;
	m_pcsControl = pcsControl;

	return TRUE;
}

BOOL UITWindowProperties::UpdateDataFromDialog()
{
	m_dlgWinProperty.UpdateDataFromDialog();
	m_dlgMap.UpdateDataFromDialog();
	m_dlgStyle.UpdateDataFromDialog();
	m_dlgAnimation.UpdateDataFromDialog();

	if (m_pcsBase && m_apcsDialogs[m_pcsBase->m_nType])
	{
		m_apcsDialogs[m_pcsBase->m_nType]->UpdateDataFromDialog();
	}

	return TRUE;
}
