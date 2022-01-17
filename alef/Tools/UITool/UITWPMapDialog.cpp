// UITWPMapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPMapDialog.h"

#include "UITMessageMapDialog.h"
#include "UITDisplayMapDialog.h"
#include "UITStatusDialog.h"
#include "UITUserDataDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPMapDialog property page

IMPLEMENT_DYNCREATE(UITWPMapDialog, CPropertyPage)

UITWPMapDialog::UITWPMapDialog() : CPropertyPage(UITWPMapDialog::IDD)
{
	//{{AFX_DATA_INIT(UITWPMapDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pcsBase		= NULL	;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPMapDialog::~UITWPMapDialog()
{
}

void UITWPMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPMapDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPMapDialog, CPropertyPage)
	//{{AFX_MSG_MAP(UITWPMapDialog)
	ON_BN_CLICKED(IDC_UIT_EDIT_MESSAGEMAP, OnUITEditMessagemap)
	ON_BN_CLICKED(IDC_UIT_EDIT_DISPLAY, OnUITEditDisplay)
	ON_BN_CLICKED(IDC_UIT_SET_HANDLER, OnUITSetHandler)
	ON_BN_CLICKED(IDC_UIT_EDIT_STATUS, OnUITEditStatus)
	ON_BN_CLICKED(IDC_UIT_EDIT_USER_DATA, OnUITEditUserData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPMapDialog message handlers

BOOL UITWPMapDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsBase, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPMapDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsBase		= pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd())
		return TRUE;

	if (pcsBase)
	{
		if (g_pcsAgcmUIManager2->GetControl(m_pcsUI, pcsBase))
		{
			GetDlgItem(IDC_UIT_EDIT_MESSAGEMAP)->EnableWindow(TRUE);
			GetDlgItem(IDC_UIT_EDIT_DISPLAY)->EnableWindow(TRUE);
			GetDlgItem(IDC_UIT_EDIT_USER_DATA)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_UIT_EDIT_MESSAGEMAP)->EnableWindow(FALSE);
			GetDlgItem(IDC_UIT_EDIT_DISPLAY)->EnableWindow(FALSE);
			GetDlgItem(IDC_UIT_EDIT_USER_DATA)->EnableWindow(FALSE);
		}

		GetDlgItem(IDC_UIT_EDIT_STATUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_UIT_SET_HANDLER)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_UIT_EDIT_MESSAGEMAP)->EnableWindow(FALSE);
		GetDlgItem(IDC_UIT_EDIT_DISPLAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_UIT_EDIT_USER_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_UIT_EDIT_STATUS)->EnableWindow(TRUE);
		GetDlgItem(IDC_UIT_SET_HANDLER)->EnableWindow(FALSE);
	}

	return TRUE;
}

BOOL UITWPMapDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	return TRUE;
}

void UITWPMapDialog::OnUITEditMessagemap() 
{
	if (!m_pcsBase || !m_pcsControl)
		return;

	UITMessageMapDialog	dlgMessageMap(m_pcsUI, m_pcsControl);

	if (dlgMessageMap.DoModal() == IDOK)
	{
	}
}

void UITWPMapDialog::OnUITEditDisplay() 
{
	if (!m_pcsBase || !m_pcsControl)
		return;

	UITDisplayMapDialog	dlgDisplayMap(m_pcsControl);

	if (dlgDisplayMap.DoModal() == IDOK)
	{
	}
}

void UITWPMapDialog::OnUITSetHandler() 
{
	if (g_MainWindow.m_eMode != UIT_MODE_AREA || !g_MainWindow.m_bDrawArea)
	{
		AfxMessageBox("Set Area at Area Mode First !!!");
		return;
	}

	if (!m_pcsUI)
	{
		AfxMessageBox("Select Window(Control) First !!!");
		return;
	}

	INT32	lStartX	= (INT32) g_MainWindow.m_stAreaStart.x;
	INT32	lStartY	= (INT32) g_MainWindow.m_stAreaStart.y;
	INT32	lEndX	= (INT32) g_MainWindow.m_stAreaEnd.x;
	INT32	lEndY	= (INT32) g_MainWindow.m_stAreaEnd.y;

	m_pcsUI->m_pcsUIWindow->ScreenToClient(&lStartX, &lStartY);
	m_pcsUI->m_pcsUIWindow->ScreenToClient(&lEndX, &lEndY);

	if (lStartX < 0)
		lStartX = 0;
	if (lStartY < 0)
		lStartY = 0;
	if (lEndX > m_pcsUI->m_pcsUIWindow->w)
		lEndX = m_pcsUI->m_pcsUIWindow->w;
	if (lEndY > m_pcsUI->m_pcsUIWindow->h)
		lEndY = m_pcsUI->m_pcsUIWindow->h;

	m_pcsUI->m_pcsUIWindow->SetHandler(lStartX, lStartY, lEndX - lStartX, lEndY - lStartY);
}

void UITWPMapDialog::OnUITEditStatus() 
{
	if (!m_pcsBase)
		return;

	UITStatusDialog dlgStatus(m_pcsBase, m_pcsControl);

	if (dlgStatus.DoModal() == IDOK)
	{
	}
}

void UITWPMapDialog::OnUITEditUserData() 
{
	if (!m_pcsBase || !m_pcsControl)
		return;

	UITUserDataDialog	dlgUserData(m_pcsControl);

	if (dlgUserData.DoModal() == IDOK)
	{
	}
}
