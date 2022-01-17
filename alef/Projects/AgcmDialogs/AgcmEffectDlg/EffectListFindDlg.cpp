// EffectListFindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeffectdlg.h"
#include "../resource.h"
#include "EffectListFindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectListFindDlg dialog


CEffectListFindDlg::CEffectListFindDlg(CHAR *pszDest, CWnd* pParent /*=NULL*/)
	: CDialog(CEffectListFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectListFindDlg)
	m_strFind = _T("");
	//}}AFX_DATA_INIT

	m_pszDest = pszDest;
}


void CEffectListFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectListFindDlg)
	DDX_Text(pDX, IDC_EDIT_EL_FIND, m_strFind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectListFindDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectListFindDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectListFindDlg message handlers

void CEffectListFindDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if(m_pszDest)
	{
		strcpy(m_pszDest, (LPSTR)(LPCSTR)(m_strFind));
	}
	
	CDialog::OnOK();
}
