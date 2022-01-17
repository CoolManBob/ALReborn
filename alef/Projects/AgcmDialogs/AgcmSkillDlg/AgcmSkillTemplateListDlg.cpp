// AgcmSkillTemplateListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmSkillDlg.h"
#include "AgcmSkillTemplateListDlg.h"
#include "../EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmSkillTemplateListDlg dialog


AgcmSkillTemplateListDlg::AgcmSkillTemplateListDlg(CHAR **ppszDest, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmSkillTemplateListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmSkillTemplateListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ppszDest	= ppszDest;
	m_plTID		= NULL;
}

AgcmSkillTemplateListDlg::AgcmSkillTemplateListDlg(INT32 *plTID, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmSkillTemplateListDlg::IDD, pParent)
{
	m_ppszDest	= NULL;
	m_plTID		= plTID;
}

void AgcmSkillTemplateListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmSkillTemplateListDlg)
	DDX_Control(pDX, IDC_LIST_SKILL_TEMPLATE, m_csSkillTemplateList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmSkillTemplateListDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmSkillTemplateListDlg)
	ON_LBN_DBLCLK(IDC_LIST_SKILL_TEMPLATE, OnDblclkListSkillTemplate)
	ON_BN_CLICKED(IDC_BUTTON_STL_FIND, OnButtonStlFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmSkillTemplateListDlg message handlers

void AgcmSkillTemplateListDlg::OnDblclkListSkillTemplate() 
{
	// TODO: Add your control notification handler code here

	OnOK();
}

BOOL AgcmSkillTemplateListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AgpmSkill *pcsAgpmSkill = AgcmSkillDlg::GetInstance()->GetAgpmSkill();
	if(!pcsAgpmSkill)
		return FALSE;

	INT32 lIndex = 0;
	INT32 lCount = -1;

	if(m_ppszDest)
	{
/*		for(	AgpdSkillTemplate *pcsTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex);
				pcsTemplate;
				pcsTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex)						)
		{
			m_csSkillTemplateList.AddString(pcsTemplate->m_szName);
		}*/
		for(	AgpdSkillTemplate *pcsTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex);
				pcsTemplate;
				pcsTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex)						)
		{
			m_csSkillTemplateList.SetItemData(m_csSkillTemplateList.InsertString(++lCount, pcsTemplate->m_szName), (DWORD)(pcsTemplate->m_szName));
		}
	}
	else
	{
		for(	AgpdSkillTemplate *pcsTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex);
				pcsTemplate;
				pcsTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex)						)
		{
			m_csSkillTemplateList.SetItemData(m_csSkillTemplateList.InsertString(++lCount, pcsTemplate->m_szName), pcsTemplate->m_lID);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID AgcmSkillTemplateListDlg::_ToUpper(CHAR *szDest, CHAR *szSrc)
{
	strcpy(szDest, szSrc);
	INT16 nMax = strlen(szSrc);
	for(INT16 nCount = 0; nCount < nMax; szDest[nCount] = toupper(szSrc[nCount]), ++nCount);
}

BOOL AgcmSkillTemplateListDlg::IsLowEqual(CHAR *szCmp1, CHAR *szCmp2)
{
	CHAR szTemp1[256], szTemp2[256];

	_ToUpper(szTemp1, szCmp1);
	_ToUpper(szTemp2, szCmp2);

	INT16 nMax = strlen(szTemp2);
	if(nMax > (INT16)(strlen(szTemp1)))
		return FALSE;

	for(INT16 nCount = 0; nCount < nMax; ++nCount)
	{
		if(szTemp1[nCount] != szTemp2[nCount])
			return FALSE;
	}

	return TRUE;
}

void AgcmSkillTemplateListDlg::OnButtonStlFind() 
{
	// TODO: Add your control notification handler code here
	CHAR szSelected[256];

	CEditDlg dlg(szSelected);
	if(dlg.DoModal() == IDOK)
	{
		CHAR	szCurList[256];
		INT16	nMax = m_csSkillTemplateList.GetCount();
		for(INT16 nCount = 0; nCount < nMax; ++nCount)
		{
			m_csSkillTemplateList.GetText(nCount, szCurList);

			if(IsLowEqual(szCurList, szSelected))
			{
				m_csSkillTemplateList.SetCurSel(nCount);
				break;
			}
		}
	}
}

void AgcmSkillTemplateListDlg::OnOK() 
{
	// TODO: Add extra validation here

	if(m_ppszDest)
	{
//		m_csSkillTemplateList.GetText(m_csSkillTemplateList.GetCurSel(), m_pszDest);
		*(m_ppszDest) = (CHAR *)(m_csSkillTemplateList.GetItemData(m_csSkillTemplateList.GetCurSel()));
	}
	else
	{
		*(m_plTID) = (INT32)(m_csSkillTemplateList.GetItemData(m_csSkillTemplateList.GetCurSel()));
	}
	
	CDialog::OnOK();
}
