// AgcmEffectList.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmEffectList.h"
#include "AgcmEffectDlg.h"
#include "EffectListFindDlg.h"

#define AGCMEFFECT_INI_KEYNAME_EFFECTSET_INFO		"EffectSetInfo"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEffectList dialog


AgcmEffectList::AgcmEffectList(UINT32 *pulIndex, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEffectList::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEffectList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pulIndex = pulIndex;
}


void AgcmEffectList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEffectList)
	DDX_Control(pDX, IDC_EFFECT_LIST, m_csEffectList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEffectList, CDialog)
	//{{AFX_MSG_MAP(AgcmEffectList)
	ON_NOTIFY(NM_DBLCLK, IDC_EFFECT_LIST, OnDblclkEffectList)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEffectList message handlers

BOOL AgcmEffectList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_csEffectList.InsertColumn(1, "EID", LVCFMT_LEFT, 100, 0);
	m_csEffectList.InsertColumn(2, "Effect Title", LVCFMT_LEFT, 250, 0);

	if(!LoadEffectList())
	{
		OutputDebugString("AgcmEffectList::OnInitDialog() Error (1) !!!\n");
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL AgcmEffectList::LoadEffectList()
{
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
	for( nIndex = 0; bFind;
		bFind = csFinder.FindNextFile(),
		sprintf(szFileName, "./%s", csFinder.GetFileName()),
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

	return TRUE;
}

void AgcmEffectList::OnOK() 
{
	// TODO: Add extra validation here
/*	if(m_csEffectList.GetSelectedCount() > 1)
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

	if(ulEID)
	{
		*m_pulEID = ulEID;
	}*/
//	AgcmEffectDlg::GetInstance()->SetEID(lEID);

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->CloseEffectList();
	
//	CDialog::OnOK();
}

void AgcmEffectList::OnDblclkEffectList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
//	OnOK();
	
	*pResult = 0;
}

VOID AgcmEffectList::_ToUpper(CHAR *szDest, CHAR *szSrc)
{
	strcpy(szDest, szSrc);
	INT16 nMax = strlen(szSrc);
	for(INT16 nCount = 0; nCount < nMax; szDest[nCount] = toupper(szSrc[nCount]), ++nCount);
}

BOOL AgcmEffectList::IsLowEqual(CHAR *szCmp1, CHAR *szCmp2)
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

VOID AgcmEffectList::SetEffectList(CHAR *szCmp)
{
	CHAR	szCurList[256];
	INT16	nMax = m_csEffectList.GetItemCount();
	for(INT16 nCount = 0; nCount < nMax; ++nCount)
	{
		m_csEffectList.GetItemText(nCount, 1, szCurList, 256);
		if(IsLowEqual(szCurList, szCmp))
		{
			m_csEffectList.SetItemState(nCount, LVIS_SELECTED , LVIS_SELECTED);
			break;
		}
	}
}

BOOL AgcmEffectList::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void AgcmEffectList::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void AgcmEffectList::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	if (!m_pulIndex)
		return;

	if (!AgcmEffectDlg::GetInstance())
		return;

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

	*m_pulIndex					= ulEID;

	AgcmEffectDlg::GetInstance()->UpdateEffectScript();
}

VOID AgcmEffectList::EnableApplyButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(bEnable);
}

VOID AgcmEffectList::UpdateEffectIndex(UINT32 *plIndex)
{
	m_pulIndex = plIndex;
}

void AgcmEffectList::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
