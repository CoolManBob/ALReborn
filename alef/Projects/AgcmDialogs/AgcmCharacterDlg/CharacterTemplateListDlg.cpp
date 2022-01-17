#include "stdafx.h"
#include "agcmcharacterdlg.h"
#include "CharacterTemplateListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCharacterTemplateListDlg::CCharacterTemplateListDlg(INT32	*plIndex, CWnd* pParent /*=NULL*/) : CDialog(CCharacterTemplateListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharacterTemplateListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_plIndex	= plIndex;
}


void CCharacterTemplateListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterTemplateListDlg)
	DDX_Control(pDX, IDC_LIST_CHAR_TEMPLATE, m_csCharTemplateList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharacterTemplateListDlg, CDialog)
	//{{AFX_MSG_MAP(CCharacterTemplateListDlg)
	ON_LBN_DBLCLK(IDC_LIST_CHAR_TEMPLATE, OnDblclkListCharTemplate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterTemplateListDlg message handlers

BOOL CCharacterTemplateListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!AgcmCharacterDlg::GetInstance())
		return FALSE;

	AgpmCharacter	*pcsAgpmCharacter	= AgcmCharacterDlg::GetInstance()->GetAgpmCharacter();
	if(!pcsAgpmCharacter)
		return FALSE;

	INT32			lCount				= -1;
	INT32			lIndex				= 0;
	for(AgpdCharacterTemplate *pcsAgpdCharacterTemplate = pcsAgpmCharacter->GetTemplateSequence(&lIndex);
		pcsAgpdCharacterTemplate;
		pcsAgpdCharacterTemplate = pcsAgpmCharacter->GetTemplateSequence(&lIndex)							)
	{
		m_csCharTemplateList.SetItemData(m_csCharTemplateList.InsertString(++lCount, pcsAgpdCharacterTemplate->m_szTName), pcsAgpdCharacterTemplate->m_lID);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCharacterTemplateListDlg::OnDblclkListCharTemplate() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CCharacterTemplateListDlg::OnOK() 
{
	// TODO: Add extra validation here
	INT32	lCurIndex = m_csCharTemplateList.GetCurSel();
	if(lCurIndex > -1)
	{
		if(m_plIndex)
		{
			*(m_plIndex)	= m_csCharTemplateList.GetItemData(lCurIndex);
		}
	}
	
	CDialog::OnOK();
}
