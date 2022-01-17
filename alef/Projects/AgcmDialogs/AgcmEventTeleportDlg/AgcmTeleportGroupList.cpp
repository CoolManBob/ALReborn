// AgcmTeleportGroupList.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventteleportdlg.h"
#include "AgcmTeleportGroupList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportGroupList dialog

// TODO : 이거 H에만 있고 rc파일엔 없는 리소스로 만들어진 Dialog..
// 그냥 싹 지우면 돼는건가
#define IDC_LIST1 0

AgcmTeleportGroupList::AgcmTeleportGroupList(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmTeleportGroupList::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmTeleportGroupList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmTeleportGroupList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmTeleportGroupList)
	DDX_Control(pDX, IDC_LIST1, m_csListGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmTeleportGroupList, CDialog)
	//{{AFX_MSG_MAP(AgcmTeleportGroupList)
//	ON_BN_CLICKED(IDC_BUTTON_CREATE_GROUP, OnButtonCreateGroup)
//	ON_BN_CLICKED(IDC_BUTTON_DELETE_GROUP, OnButtonDeleteGroup)
//	ON_BN_CLICKED(IDC_BUTTON_EDIT_GROUP, OnButtonEditGroup)
//	ON_BN_CLICKED(IDSELECT, OnSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportGroupList message handlers

BOOL AgcmTeleportGroupList::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here

	INT32	lIndex = 0;
	AgpdTeleportGroup **ppcsTeleportGroup = (AgpdTeleportGroup **) m_pcsAgpmEventTeleport->m_csAdminGroup.GetObjectSequence(&lIndex);
	while (ppcsTeleportGroup && *ppcsTeleportGroup)
	{
		INT32	lAddIndex	= m_csListGroup.AddString((*ppcsTeleportGroup)->m_szGroupName);
		if (lAddIndex != LB_ERR)
			m_csListGroup.SetItemDataPtr(lAddIndex, *ppcsTeleportGroup);

		ppcsTeleportGroup = (AgpdTeleportGroup **) m_pcsAgpmEventTeleport->m_csAdminGroup.GetObjectSequence(&lIndex);
	}

   return TRUE;   // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL AgcmTeleportGroupList::InitData(AgpmEventTeleport *pcsAgpmEventTeleport, CHAR **pszSelectedGroupName)
{
	if (!pcsAgpmEventTeleport)
		return FALSE;

	m_pcsAgpmEventTeleport		= pcsAgpmEventTeleport;
	m_pszSelectedGroupName		= pszSelectedGroupName;

	return FALSE;
}

void AgcmTeleportGroupList::OnButtonCreateGroup() 
{
	// TODO: Add your control notification handler code here
	AgcmTeleportGroup	csGroupDialog;

	csGroupDialog.InitData(m_pcsAgpmEventTeleport, NULL);

	csGroupDialog.DoModal();

	INT32	lIndex = 0;
	AgpdTeleportGroup **ppcsTeleportGroup = (AgpdTeleportGroup **) m_pcsAgpmEventTeleport->m_csAdminGroup.GetObjectSequence(&lIndex);
	while (ppcsTeleportGroup && *ppcsTeleportGroup)
	{
		INT32	lAddIndex	= m_csListGroup.AddString((*ppcsTeleportGroup)->m_szGroupName);
		if (lAddIndex != LB_ERR)
			m_csListGroup.SetItemDataPtr(lAddIndex, *ppcsTeleportGroup);

		ppcsTeleportGroup = (AgpdTeleportGroup **) m_pcsAgpmEventTeleport->m_csAdminGroup.GetObjectSequence(&lIndex);
	}
}

void AgcmTeleportGroupList::OnButtonDeleteGroup() 
{
	// TODO: Add your control notification handler code here
	
	INT32				lIndex = m_csListGroup.GetCurSel();
	AgpdTeleportGroup*	pcsGroup;

	if (lIndex == LB_ERR)
		return;

	pcsGroup = (AgpdTeleportGroup *) m_csListGroup.GetItemDataPtr(lIndex);

	if (pcsGroup)
		m_pcsAgpmEventTeleport->RemoveTeleportGroup(pcsGroup->m_szGroupName);

	m_csListGroup.DeleteString(lIndex);
}

void AgcmTeleportGroupList::OnButtonEditGroup() 
{
	// TODO: Add your control notification handler code here
	INT32				lIndex = m_csListGroup.GetCurSel();
	AgpdTeleportGroup*	pcsGroup;

	if (lIndex == LB_ERR)
		return;

	pcsGroup = (AgpdTeleportGroup *) m_csListGroup.GetItemDataPtr(lIndex);
	if (!pcsGroup)
		return;

	AgcmTeleportGroup	csGroupDialog;

	csGroupDialog.InitData(m_pcsAgpmEventTeleport, pcsGroup);

	csGroupDialog.DoModal();

	return;
}

void AgcmTeleportGroupList::OnSelect() 
{
	// TODO: Add your control notification handler code here
	INT32				lIndex = m_csListGroup.GetCurSel();
	AgpdTeleportGroup*	pcsGroup;

	if (lIndex == LB_ERR)
		return;

	pcsGroup = (AgpdTeleportGroup *) m_csListGroup.GetItemDataPtr(lIndex);
	if (!pcsGroup)
		return;

	*m_pszSelectedGroupName	= pcsGroup->m_szGroupName;

	m_csListGroup.ResetContent();

	CDialog::OnOK();
}

void AgcmTeleportGroupList::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	*m_pszSelectedGroupName	= NULL;

	m_csListGroup.ResetContent();

	CDialog::OnCancel();
}
