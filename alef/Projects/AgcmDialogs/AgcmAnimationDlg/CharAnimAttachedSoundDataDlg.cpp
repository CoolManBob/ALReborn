// CharAnimAttachedSoundDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAnimationDlg.h"
#include "../resource.h"
#include "CharAnimAttachedSoundDataDlg.h"
#include "AgcmFileListDlg.h"


// CCharAnimAttachedSoundDataDlg dialog
static CHAR *g_aszSoundConditionName[E_AEE_CAASD_MAX_CONDITION] = 
{
	"NONE",
	"ATTACK_SUCCESS",
	"ATTACK_MISS"
};

IMPLEMENT_DYNAMIC(CCharAnimAttachedSoundDataDlg, CDialog)
CCharAnimAttachedSoundDataDlg::CCharAnimAttachedSoundDataDlg(AEE_CharAnimAttachedData *pcsAEE, CWnd* pParent /*=NULL*/)
	: CDialog(CCharAnimAttachedSoundDataDlg::IDD, pParent)
{
	m_pcsAEE	= pcsAEE;
}

CCharAnimAttachedSoundDataDlg::~CCharAnimAttachedSoundDataDlg()
{
}

void CCharAnimAttachedSoundDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SOUND, m_csListSound);
	DDX_Control(pDX, IDC_COMBO_CONDITION, m_csComboCondition);
}

BOOL CCharAnimAttachedSoundDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	INT32	lIndex = 0;

	for (lIndex = 0; lIndex < E_AEE_CAASD_MAX_CONDITION; ++lIndex)
		m_csComboCondition.InsertString(lIndex, g_aszSoundConditionName[lIndex]);

	if (lIndex > 0)
		m_csComboCondition.SetCurSel(0);

	lIndex	= 0;

	for( AEE_CharAnimAttachedData::SoundList::iterator	iter = m_pcsAEE->GetList().begin();
		iter != m_pcsAEE->GetList().end();
		iter ++ )
	{
		AEE_CharAnimAttachedSoundData	*pcsCurrent	= &*iter;
		if (	!pcsCurrent->m_strSoundName.empty()	)
		{
			m_csListSound.InsertString(lIndex, pcsCurrent->m_strSoundName.c_str());
			m_csListSound.SetItemData(lIndex, (DWORD_PTR)(pcsCurrent));
		}
		++lIndex;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CCharAnimAttachedSoundDataDlg, CDialog)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST_SOUND, OnLbnSelchangeListSound)
	ON_CBN_SELCHANGE(IDC_COMBO_CONDITION, OnCbnSelchangeComboCondition)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CCharAnimAttachedSoundDataDlg)
END_DHTML_EVENT_MAP()



// CCharAnimAttachedSoundDataDlg message handlers

void CCharAnimAttachedSoundDataDlg::OnClose()
{
	CDialog::OnClose();
}

void CCharAnimAttachedSoundDataDlg::OnLbnSelchangeListSound()
{
	AEE_CharAnimAttachedSoundData	*pcsData	=
		(AEE_CharAnimAttachedSoundData *)(
			m_csListSound.GetItemData(
				m_csListSound.GetCurSel()	)	);
	if (pcsData)
	{
		m_csComboCondition.SetCurSel(pcsData->m_unConditions);
	}
}

void CCharAnimAttachedSoundDataDlg::OnCbnSelchangeComboCondition()
{
	INT32 lSel	= m_csListSound.GetCurSel();
	if (lSel < 0)
		return;

	AEE_CharAnimAttachedSoundData	*pcsData	=
		(AEE_CharAnimAttachedSoundData *)(
			m_csListSound.GetItemData(
				lSel					)	);
	if (!pcsData)
		return;

	pcsData->m_unConditions	= m_csComboCondition.GetCurSel();;
}

void CCharAnimAttachedSoundDataDlg::OnBnClickedButtonAdd()
{
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp = dlg.OpenFileList(AgcmAnimationDlg::GetInstance()->GetFindCharAnimSoundPath(), NULL, NULL);
	if (!pszTemp)
		return;

	m_pcsAEE->GetList().push_back( AEE_CharAnimAttachedSoundData( pszTemp) );

	m_csListSound.InsertString(0, pszTemp);
	m_csListSound.SetItemData(0, (DWORD_PTR)( &*m_pcsAEE->GetList().rbegin() ));
}

void CCharAnimAttachedSoundDataDlg::OnBnClickedButtonRemove()
{
	INT32	lSel	= m_csListSound.GetCurSel();
	if (lSel < 0)
		return;

	AEE_CharAnimAttachedSoundData	*pcsRemove		=
		(AEE_CharAnimAttachedSoundData *)(m_csListSound.GetItemData(lSel));

	for( AEE_CharAnimAttachedData::SoundList::iterator	iter = m_pcsAEE->GetList().begin();
		iter != m_pcsAEE->GetList().end();
		iter ++ )
	{
		AEE_CharAnimAttachedSoundData	*pcsCurrent	= &*iter;
		if( pcsCurrent == pcsRemove )
		{
			m_pcsAEE->GetList().erase( iter );
			m_csListSound.DeleteString(lSel);
			break;
		}
	}
}
