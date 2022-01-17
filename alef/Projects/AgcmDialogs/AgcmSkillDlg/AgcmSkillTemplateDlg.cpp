// AgcmSkillTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"

#include "AgcmSkillDlg.h"
#include "AgcmSkillTemplateDlg.h"

#include "AgcmFileListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmSkillTemplateDlg dialog


AgcmSkillTemplateDlg::AgcmSkillTemplateDlg(AgpdSkillTemplate *pcsAgpdSkillTemplate, AgcdSkillTemplate *pcsAgcdSkillTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmSkillTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmSkillTemplateDlg)
	m_strName				= _T(pcsAgpdSkillTemplate->m_szName);
	m_strSmallSkillTexture	=
		pcsAgcdSkillTemplate->m_pszSmallTextureName ?
		_T(pcsAgcdSkillTemplate->m_pszSmallTextureName) :
		_T("");
	m_strSkillTexture		= 
		pcsAgcdSkillTemplate->m_pszTextureName ?
		_T(pcsAgcdSkillTemplate->m_pszTextureName) :
		_T("");
	m_strUnableSkillTexture =
		pcsAgcdSkillTemplate->m_pszUnableTextureName ?
		_T(pcsAgcdSkillTemplate->m_pszUnableTextureName) :
		_T("");
	//}}AFX_DATA_INIT

	m_pcsAgpdSkillTemplate	= pcsAgpdSkillTemplate;
	m_pcsAgcdSkillTemplate	= pcsAgcdSkillTemplate;
//	m_strTemp				= m_strName;
}


void AgcmSkillTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmSkillTemplateDlg)
	DDX_Text(pDX, IDC_EDIT_SMALL_SKILL_TEXTURE, m_strSmallSkillTexture);
	DDX_Text(pDX, IDC_EDIT_SKILL_TEXTURE, m_strSkillTexture);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_UNABLE_SKILL_TEXTURE, m_strUnableSkillTexture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmSkillTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmSkillTemplateDlg)
	ON_BN_CLICKED(IDC_BUTTON_SET_SKILL_TEXTURE, OnButtonSetSkillTexture)
	ON_BN_CLICKED(IDC_BUTTON_SET_SMALL_SKILL_TEXTURE, OnButtonSetSmallSkillTexture)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SKILL_TEXTURE, OnButtonDeleteSkillTexture)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_SMALL_SKILL_TEXTURE, OnButtonDeleteSmallSkillTexture)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_UNABLE_SKILL_TEXTURE, OnButtonDeleteUnableSkillTexture)
	ON_BN_CLICKED(IDC_BUTTON_SET_UNABLE_SKILL_TEXTURE, OnButtonSetUnableSkillTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmSkillTemplateDlg message handlers

void AgcmSkillTemplateDlg::OnButtonSetSkillTexture() 
{
	// TODO: Add your control notification handler code here
	if(!AgcmSkillDlg::GetInstance())
		return;

	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp = dlg.OpenFileList(	AgcmSkillDlg::GetInstance()->GetTexturePathName1(),
								AgcmSkillDlg::GetInstance()->GetTexturePathName2(),
								AgcmSkillDlg::GetInstance()->GetTexturePathName3()		);
	if(pszTemp)
	{
		m_strSkillTexture = pszTemp;
		UpdateData(FALSE);
	}
}

void AgcmSkillTemplateDlg::OnButtonSetSmallSkillTexture() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp = dlg.OpenFileList(	AgcmSkillDlg::GetInstance()->GetTexturePathName1(),
								AgcmSkillDlg::GetInstance()->GetTexturePathName2(),
								AgcmSkillDlg::GetInstance()->GetTexturePathName3()		);
	if(pszTemp)
	{
		m_strSmallSkillTexture = pszTemp;
		UpdateData(FALSE);
	}
}

void AgcmSkillTemplateDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	ReallocCopyString( &m_pcsAgcdSkillTemplate->m_pszTextureName, (char*)m_strSkillTexture.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdSkillTemplate->m_pszSmallTextureName, (char*)m_strSmallSkillTexture.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdSkillTemplate->m_pszUnableTextureName, (char*)m_strUnableSkillTexture.GetBuffer() );

	char* szName = (char*)m_strName.GetBuffer();
	if( strcmp( m_pcsAgpdSkillTemplate->m_szName, szName ) )
	{
		AgcmSkillDlg::GetInstance()->GetAgpmSkill()->ChangeSkillTemplateName( m_pcsAgpdSkillTemplate->m_szName, szName );
		strcpy( m_pcsAgpdSkillTemplate->m_szName, szName );
	}

	CDialog::OnOK();
}

void AgcmSkillTemplateDlg::OnButtonDeleteSkillTexture() 
{
	// TODO: Add your control notification handler code here
	m_strSkillTexture = "";
	UpdateData(FALSE);
}

void AgcmSkillTemplateDlg::OnButtonDeleteSmallSkillTexture() 
{
	// TODO: Add your control notification handler code here
	m_strSmallSkillTexture = "";
	UpdateData(FALSE);
}

void AgcmSkillTemplateDlg::OnButtonDeleteUnableSkillTexture() 
{
	// TODO: Add your control notification handler code here
	m_strUnableSkillTexture = "";
	UpdateData(FALSE);
}

void AgcmSkillTemplateDlg::OnButtonSetUnableSkillTexture() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp = dlg.OpenFileList(	AgcmSkillDlg::GetInstance()->GetTexturePathName1(),
								AgcmSkillDlg::GetInstance()->GetTexturePathName2(),
								AgcmSkillDlg::GetInstance()->GetTexturePathName3()		);
	if(pszTemp)
	{
		m_strUnableSkillTexture = pszTemp;
		UpdateData(FALSE);
	}	
}
