// EffectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeffectdlg.h"
#include "EffectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectDlg dialog


CEffectDlg::CEffectDlg(AgcdUseEffectSet *pstAgcdUseEffectSet, CWnd* pParent /*=NULL*/)
	: CDialog(CEffectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pstAgcdUseEffectSet	= pstAgcdUseEffectSet;
//	m_lEffectIndex			= -1;
}


void CEffectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectDlg)
	DDX_Control(pDX, IDC_TREE_EFFECT, m_csEffectTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_EFFECT, OnSelchangedTreeEffect)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CONDITION_LIST, OnButtonOpenConditionList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CUST_DATA_LIST, OnButtonOpenCustDataList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_EFFECT_LIST, OnButtonOpenEffectList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_STATUS_LIST, OnButtonOpenStatusList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ROTATION_DLG, OnButtonOpenRotationDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectDlg message handlers

void CEffectDlg::OnOK() 
{
	// TODO: Add extra validation here
//	CloseScriptDlg();

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->CloseAllEffectDlg();
//		AgcmEffectDlg::GetInstance()->CloseUseEffectSet();

//	CDialog::OnOK();
}

BOOL CEffectDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CEffectDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CEffectDlg::InitializeDlg(AgcdUseEffectSet *pstAgcdUseEffectSet)
{
	m_pstAgcdUseEffectSet	= pstAgcdUseEffectSet;
	m_lEffectIndex			= -1;

	InitializeEffectTree();

	return TRUE;
}

BOOL CEffectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	

	return InitializeDlg(m_pstAgcdUseEffectSet);
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEffectDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	if (!AgcmEffectDlg::GetInstance()->GetAgcmEventEffect())
		return;

	HTREEITEM	hRoot	= m_csEffectTree.GetRootItem();
	if (!hRoot)
		return;

/*	for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
	{
		if (!m_pstAgcdUseEffectSet->m_astData[lCount])
		{
			m_pstAgcdUseEffectSet->m_astData[lCount] = AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->CreateEffectData();
			break;
		}

		if ((m_pstAgcdUseEffectSet->m_astData[lCount]) && (!m_pstAgcdUseEffectSet->m_astData[lCount]->m_ulConditionFlags))
			break;
	}

	if (lCount == D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM)
		return;

	if (!m_pstAgcdUseEffectSet->m_astData[lCount])
		return;

	m_pstAgcdUseEffectSet->m_astData[lCount]->m_ulConditionFlags = AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;*/

	AgcdUseEffectSetData	*pcsData	= NULL;

	INT32 lCount;

	for (lCount = 0; ; ++lCount)
	{
		pcsData = AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, lCount);
		if (!pcsData)
		{
			pcsData = AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, lCount, TRUE);
			if (!pcsData)
				return;

			break;
		}
	}

	pcsData->m_ulConditionFlags	= AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;

	CHAR	szTemp[256];
	sprintf(szTemp, "[%.*d]", D_EFFECT_DLG_ROOT_PRECISION, lCount);
	InsertEffectTreeItem(szTemp, hRoot, (DWORD)(lCount), TRUE);
}

void CEffectDlg::OnButtonRemove() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM				hSelected		= m_csEffectTree.GetSelectedItem();
	if (!hSelected)
		return;

	HTREEITEM				hRoot			= m_csEffectTree.GetRootItem();
	if (!hRoot)
		return;

	AgcdUseEffectSetData	*pcsData		= NULL;
	INT32					lEffectIndex	= 0;
	if (hSelected == hRoot) // remove all!
	{
		HTREEITEM	hTemp;
		hSelected			= m_csEffectTree.GetChildItem(hSelected);
		while (hSelected)
		{
			lEffectIndex	= (INT32)(m_csEffectTree.GetItemData(hSelected));
//			if ((lEffectIndex < 0) || (lEffectIndex >= D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM))
			if (lEffectIndex < 0)
			{
				MessageBox("ERROR!", "ERROR!");
				return;
			}

/*			if (!m_pstAgcdUseEffectSet->m_astData[lEffectIndex])
			{
				MessageBox("ERROR!", "ERROR!");
				return;
			}*/
			pcsData			=
				AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, lEffectIndex);
			
			if (!pcsData)
			{
				MessageBox("ERROR!", "ERROR!");
				return;
			}
			
//			memset(m_pstAgcdUseEffectSet->m_astData[lEffectIndex], 0, sizeof(AgcdUseEffectSetData));
//			m_pstAgcdUseEffectSet->m_astData[lEffectIndex]->m_fScale = 1.0f;

			AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->Delete(
				m_pstAgcdUseEffectSet, pcsData);

			hTemp			= m_csEffectTree.GetNextItem(hSelected, TVGN_NEXT);
			m_csEffectTree.DeleteItem(hSelected);
			hSelected		= hTemp;
		}
	}
	else
	{
		lEffectIndex	= (INT32)(m_csEffectTree.GetItemData(hSelected));
/*		if (!m_pstAgcdUseEffectSet->m_astData[lEffectIndex])
			return;

		memset(m_pstAgcdUseEffectSet->m_astData[lEffectIndex], 0, sizeof(AgcdUseEffectSetData));
		m_pstAgcdUseEffectSet->m_astData[lEffectIndex]->m_fScale = 1.0f;*/

		pcsData			=
			AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, lEffectIndex);
		
		if (!pcsData)
		{
			MessageBox("ERROR!", "ERROR!");
			return;
		}

		AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->Delete(
			m_pstAgcdUseEffectSet, pcsData);

		m_csEffectTree.DeleteItem(hSelected);
	}

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->EnableEffectFlagsDlg(FALSE);
}

VOID CEffectDlg::InitializeEffectTree()
{
	CHAR	szTemp[256];

	m_csEffectTree.DeleteAllItems();

	HTREEITEM	hRoot	= InsertEffectTreeItem(D_EFFECT_DLG_TREE_NAME_ROOT, TVI_ROOT, D_EFFECT_DLG_TREE_ROOT_DATA);
	if (!hRoot)
		return;

//	for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
	AgcdUseEffectSetList	*pcsCurrent	= m_pstAgcdUseEffectSet->m_pcsHead;
	while (pcsCurrent)
	{
/*		if ((m_pstAgcdUseEffectSet->m_astData[lCount]) && (m_pstAgcdUseEffectSet->m_astData[lCount]->m_ulConditionFlags))
		{
			sprintf(szTemp, "[%.*d]", D_EFFECT_DLG_ROOT_PRECISION, lCount);
			InsertEffectTreeItem(szTemp, hRoot, (DWORD)(lCount), TRUE);
		}*/

		sprintf(szTemp, "[%.*d]", D_EFFECT_DLG_ROOT_PRECISION, pcsCurrent->m_csData.m_ulIndex);
		InsertEffectTreeItem(szTemp, hRoot, (DWORD)(pcsCurrent->m_csData.m_ulIndex), TRUE);

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}
}

HTREEITEM CEffectDlg::InsertEffectTreeItem(CHAR *szName, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible)
{
	HTREEITEM hItem = m_csEffectTree.InsertItem(szName, hParent, TVI_SORT);
	if (!hItem)
		return NULL;

	if (!m_csEffectTree.SetItemData(hItem, dwData))
		return NULL;

	if (bEnsureVisible)
		m_csEffectTree.EnsureVisible(hItem);

	return hItem;
}

void CEffectDlg::OnSelchangedTreeEffect(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (!AgcmEffectDlg::GetInstance())
		return;

	INT32		lEffectIndex	= (INT32)(m_csEffectTree.GetItemData(pNMTreeView->itemNew.hItem));
	if (lEffectIndex == D_EFFECT_DLG_TREE_ROOT_DATA)
	{
//		AgcmEffectDlg::GetInstance()->CloseAllEffectDlg(FALSE);
		m_lEffectIndex			= -1;
		return;
	}

/*	if (!m_pstAgcdUseEffectSet->m_astData[lEffectIndex])
	{
		MessageBox("ERROR!", "ERROR!");
		return;
	}*/

	AgcdUseEffectSetData	*pcsData
		= AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, lEffectIndex);
	if  (!pcsData)
	{
		MessageBox("ERROR!", "ERROR!");
		return;
	}

	m_lEffectIndex				= lEffectIndex;

	AgcmEffectDlg::GetInstance()->OpenEffectScript();
	AgcmEffectDlg::GetInstance()->UpdateEffectScript(m_pstAgcdUseEffectSet, m_lEffectIndex);
//	AgcmEffectDlg::GetInstance()->UpdateEffectFlagsDlg(m_pstAgcdUseEffectSet->m_astData[lEffectIndex]);
	AgcmEffectDlg::GetInstance()->UpdateEffectFlagsDlg(pcsData);
}

/*
VOID CEffectDlg::OpenScriptDlg()
{
	if (m_pcsScriptDlg)
		return;

	m_pcsScriptDlg = new CScriptDlg();

	m_pcsScriptDlg->Create(NULL);
	m_pcsScriptDlg->ShowWindow(SW_SHOW);
}

VOID CEffectDlg::CloseScriptDlg()
{
	if (!m_pcsScriptDlg)
		return;

	m_pcsScriptDlg->ShowWindow(SW_HIDE);
	m_pcsScriptDlg->DestroyWindow();

	delete m_pcsScriptDlg;
	m_pcsScriptDlg = NULL;
}

VOID CEffectDlg::UpdateScriptDlg(AgcdUseEffectSetData *pstData, INT32 lIndex)
{
	if (!m_pcsScriptDlg)
		OpenScriptDlg();

	if (!m_pcsScriptDlg)
		return;

	m_pcsScriptDlg->UpdateScript(pstData, lIndex);
}
*/
void CEffectDlg::OnButtonOpenConditionList() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	if (m_lEffectIndex < 0)
		return;

	AgcdUseEffectSetData	*pcsData
		= AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, m_lEffectIndex);
	if (!pcsData)
		return;

/*	AgcmEffectDlg::GetInstance()->OpenEffectConditionList(
		&m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex]->m_ulConditionFlags,
		&m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex]->m_ulConditionFlags	);*/
	AgcmEffectDlg::GetInstance()->OpenEffectConditionList(
		&pcsData->m_ulConditionFlags,
		&pcsData->m_ulConditionFlags						);

//		&m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex]->m_ulSSConditionFlags	);
}

void CEffectDlg::OnButtonOpenCustDataList() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	AgcmEffectDlg::GetInstance()->OpenEffectHelp();
}

void CEffectDlg::OnButtonOpenEffectList() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	if (m_lEffectIndex < 0)
		return;

	AgcdUseEffectSetData	*pcsData =
		AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, m_lEffectIndex);
//	if (!m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex])
//		return;

//	AgcmEffectDlg::GetInstance()->OpenEffectList(&m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex]->m_ulEID);
	AgcmEffectDlg::GetInstance()->OpenEffectList(&pcsData->m_ulEID);
}

void CEffectDlg::OnButtonOpenStatusList() 
{
	// TODO: Add your control notification handler code here
/*	if (!AgcmEffectDlg::GetInstance())
		return;

	if (m_lEffectIndex < 0)
		return;

	AgcmEffectDlg::GetInstance()->OpenEffectStatusList(
		&m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex]->m_ulStatusFlags);*/
}

void CEffectDlg::OnButtonOpenRotationDlg() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	if (m_lEffectIndex < 0)
		return;

	AgcdUseEffectSetData	*pcsData =
		AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, m_lEffectIndex);

	AgcmEffectDlg::GetInstance()->OpenEffectRotationDlg(&pcsData->m_pcsRotation);
//		&m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex]->m_pstRotation	);
}

void CEffectDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
