// EffectListModalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeffectdlg.h"
#include "EffectListModalDlg.h"

#define AGCMEFFECT_INI_KEYNAME_EFFECTSET_INFO		"EffectSetInfo"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectListModalDlg dialog


CEffectListModalDlg::CEffectListModalDlg(UINT32 *pulEID, CWnd* pParent /*=NULL*/)
	: CDialog(CEffectListModalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectListModalDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pulEID = pulEID;
}


void CEffectListModalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectListModalDlg)
	DDX_Control(pDX, IDC_EFFECT_LIST, m_csEffectList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectListModalDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectListModalDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_EFFECT_LIST, OnDblclkEffectList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectListModalDlg message handlers

void CEffectListModalDlg::OnDblclkEffectList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

BOOL CEffectListModalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_csEffectList.InsertColumn(1, "EID", LVCFMT_LEFT, 100, 0);
	m_csEffectList.InsertColumn(2, "Effect Title", LVCFMT_LEFT, 250, 0);
	
	CFileFind	csFinder;
	CHAR		szFileName[256];
	CHAR		szTemp[256];
	CHAR		szOldPath[256];

	GetCurrentDirectory(256, szOldPath);
	if(!SetCurrentDirectory("./effect/ini/new/"))
	{
		OutputDebugString("AgcmEffectList::LoadEffectList() Error (1) !!!\n");
		return FALSE;
	}

	BOOL bFind = csFinder.FindFile("*.ini");
	if(!bFind)
	{
		SetCurrentDirectory(szOldPath);
		OutputDebugString("AgcmEffectList::LoadEffectList() Error (2) !!!\n");
		return FALSE;
	}

/*	for(INT16 nIndex = 0; bFind = csFinder.FindNextFile(); ++nIndex)
	{
		sprintf(szFileName, "./%s", csFinder.GetFileName());

		GetPrivateProfileString("EFFECTSET", "lEID", "",  szTemp, 256, szFileName);
		m_csEffectList.InsertItem(nIndex, szTemp);

		GetPrivateProfileString("EFFECTSET", "szTitle", "",  szTemp, 256, szFileName);
		m_csEffectList.SetItemText(nIndex, 1, szTemp);
	}*/

/*	for(bFind = csFinder.FindNextFile(); bFind; bFind = csFinder.FindNextFile())
	{
	}*/

	UINT32	ulEID, lTemp		;
	float	fTemp			;

	INT16 nIndex;

	for(nIndex = 0; 
		bFind; 
		bFind = csFinder.FindNextFile(), sprintf(szFileName, "./%s", csFinder.GetFileName()),
	/*
	GetPrivateProfileString("EFFECTSET", AGCMEFFECT_INI_KEYNAME_EFFECTSET_INFO, "",  szTemp, 256, szFileName),
	sscanf( szTemp, "%f:%d:%d:%d:%d:%d",&fTemp,	&ulEID,	&lTemp,	&lTemp,	&lTemp,	&lTemp	),
	sprintf( szTemp, "%d", ulEID ),
	m_csEffectList.InsertItem(nIndex, szTemp),
	GetPrivateProfileString("EFFECTSET", "szTitle", "",  szTemp, 256, szFileName),
	m_csEffectList.SetItemText(nIndex, 1, szTemp));
	*/
		GetPrivateProfileString("EffSet", "id", "",  szTemp, 256, szFileName),
		m_csEffectList.InsertItem(nIndex, szTemp),
		GetPrivateProfileString("EffSet", "title", "",  szTemp, 256, szFileName),
		m_csEffectList.SetItemText(nIndex, 1, szTemp));
	
	SetCurrentDirectory(szOldPath);

	if(nIndex)
	{
		m_csEffectList.SetItemState(0, LVIS_SELECTED , LVIS_SELECTED);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEffectListModalDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(m_csEffectList.GetSelectedCount() > 1)
	{
		OutputDebugString("AgcmEffectList::GetEID() Error (1) !!!\n");
		return;
	}

	POSITION pos = m_csEffectList.GetFirstSelectedItemPosition();
	if(!pos)
	{
		OutputDebugString("AgcmEffectList::GetEID() Error (2) !!!\n");
		return;
	}

	CString strSelectedText		= m_csEffectList.GetItemText(m_csEffectList.GetNextSelectedItem(pos), 0);
	UINT32	ulEID				= atoi(strSelectedText);

	*m_pulEID					= ulEID;

	CDialog::OnOK();
}
