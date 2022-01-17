// UITUserDataDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITUserDataDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITUserDataDialog dialog


UITUserDataDialog::UITUserDataDialog(AgcdUIControl *pcsControl, CWnd* pParent /*=NULL*/)
	: CDialog(UITUserDataDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITUserDataDialog)
	m_lUserDataIndex = 0;
	//}}AFX_DATA_INIT

	m_pcsControl = pcsControl;
}


void UITUserDataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITUserDataDialog)
	DDX_Control(pDX, IDC_UIT_USER_DATA_LIST, m_csUserDataList);
	DDX_Text(pDX, IDC_UIT_USER_DATA_INDEX, m_lUserDataIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITUserDataDialog, CDialog)
	//{{AFX_MSG_MAP(UITUserDataDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITUserDataDialog message handlers

void UITUserDataDialog::OnOK() 
{
	UpdateData();

	if (m_pcsControl)
	{
		AgcdUIUserData *	pstUserData;

		pstUserData = (AgcdUIUserData *) m_csUserDataList.GetItemDataPtr(m_csUserDataList.GetCurSel());
		if (pstUserData == (PVOID) -1)
			m_pcsControl->m_pstUserData = NULL;
		else if (pstUserData == (PVOID) 1)
		{
			m_pcsControl->m_bUseParentUserData = TRUE;
			m_pcsControl->m_pstUserData = g_pcsAgcmUIManager2->GetControlUserData(m_pcsControl->m_pcsParentControl);
		}
		else
			m_pcsControl->m_pstUserData = pstUserData;

		m_pcsControl->m_lUserDataIndex = m_lUserDataIndex;
	}

	g_pcsAgcmUIManager2->CountUserData();

//	CDialog::OnOK();
}

void UITUserDataDialog::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL UITUserDataDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUIUserData *	pstUserData;
	static CHAR *		szParentString = "<Parent UserData>";
	static CHAR *		szNullString = "<NULL>";

	lListIndex = m_csUserDataList.AddString(szNullString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csUserDataList.SetItemDataPtr(lListIndex, NULL);

	lListIndex = m_csUserDataList.AddString(szParentString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csUserDataList.SetItemDataPtr(lListIndex, (PVOID) 1);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData)
		{
			lListIndex = m_csUserDataList.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csUserDataList.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	if (m_pcsControl && m_pcsControl->m_bUseParentUserData)
	{
		m_csUserDataList.SelectString(-1, szParentString);
	}
	else if (m_pcsControl && m_pcsControl->m_pstUserData)
	{
		m_csUserDataList.SelectString(-1, m_pcsControl->m_pstUserData->m_szName);
	}
	else
	{
		m_csUserDataList.SelectString(-1, szNullString);
	}

	m_lUserDataIndex = m_pcsControl->m_lUserDataIndex;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
