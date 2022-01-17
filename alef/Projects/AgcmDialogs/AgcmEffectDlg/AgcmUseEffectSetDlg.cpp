// AgcmUseEffectSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmUseEffectSetDlg.h"
#include "AgcmEffectDlg.h"
#include "AgcmFileListDlg.h"
#include "AgcmEffectList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmUseEffectSetDlg dialog


AgcmUseEffectSetDlg::AgcmUseEffectSetDlg(AgcdUseEffectSet *pstInitData, INT16 nMaxEffectNum, CWnd* pParent/* = NULL*/)
	: CDialog(AgcmUseEffectSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmUseEffectSetDlg)
/*	m_unEID					= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_ulEID : 0;
	m_unGap					= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_ulStartGap : 0;
	m_fOffsetX				= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_v3dOffset.x : 0;
	m_fOffsetY				= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_v3dOffset.y : 0;
	m_fOffsetZ				= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_v3dOffset.z : 0;
	m_fScale				= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_fScale : 1;
	m_strSoundName			= (pstInitData->m_astData[0]) ? _T(pstInitData->m_astData[0]->m_szSoundName) : _T("");
	m_nParentNodeID			= (pstInitData->m_astData[0]) ? pstInitData->m_astData[0]->m_lParentNodeID : 0;*/
	//}}AFX_DATA_INIT

	m_pstAgcdEventEffect	= pstInitData;
	m_nMaxEffectNum			= nMaxEffectNum;
	m_nPreEIDIndex			= 0;

//	memcpy(&m_stUseEffectSet, pstInitData, sizeof(m_stUseEffectSet));
}

void AgcmUseEffectSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmUseEffectSetDlg)
	DDX_Control(pDX, IDC_COMBO_UES_INDEX, m_csIndex);
	DDX_Text(pDX, IDC_EDIT_UES_EID, m_unEID);
	DDX_Text(pDX, IDC_EDIT_UES_GAP, m_unGap);
	DDX_Text(pDX, IDC_EDIT_UES_OFFSET_X, m_fOffsetX);
	DDX_Text(pDX, IDC_EDIT_UES_OFFSET_Y, m_fOffsetY);
	DDX_Text(pDX, IDC_EDIT_UES_OFFSET_Z, m_fOffsetZ);
	DDX_Text(pDX, IDC_EDIT_UES_SCALE, m_fScale);
	DDX_Text(pDX, IDC_EDIT_SOUND_NAME, m_strSoundName);
	DDX_Text(pDX, IDC_EDIT_UES_PARENT_NODEID, m_nParentNodeID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmUseEffectSetDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmUseEffectSetDlg)
	ON_BN_CLICKED(IDC_BUTTON_EID_FIND, OnButtonEidFind)
	ON_BN_CLICKED(ID_USE_EFFECT_SET_APPLY, OnUseEffectSetApply)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_UES_INDEX, OnSelchangeComboUesIndex)
	ON_BN_CLICKED(IDC_BUTTON_EID_RESET, OnButtonEidReset)
//	ON_BN_CLICKED(IDC_BUTTON_SET_CONDITION_FLAG, OnButtonSetConditionFlag)
	ON_BN_CLICKED(IDC_BUTTON_SET_SOUND, OnButtonSetSound)
	ON_BN_CLICKED(IDC_BUTTON_SET_STATUS_FLAGS, OnButtonSetStatusFlags)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmUseEffectSetDlg message handlers

BOOL AgcmUseEffectSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);

	CHAR szIndex[32];
	INT16 nIndex;

	for(nIndex = 0; nIndex < m_nMaxEffectNum; ++nIndex)
	{
		sprintf(szIndex, "%d", nIndex);
		m_csIndex.InsertString(nIndex, szIndex);
	}

	if(nIndex)
		m_csIndex.SetCurSel(0);
	else
	{
		m_csIndex.EnableWindow(FALSE);
		::MessageBox(NULL, "AgcmUseEffectSetDlg::OnInitDialog()\n다이얼로그를 초기화할 수 없습니다.", "ERROR", MB_OK);
		//OutputDebugString("AgcmUseEffectSetDlg::OnInitDialog() Error (1) !!!\n");
		return FALSE;
	}

/*	AgcmEffectDlgConditionSet *pSet = &(AgcmEffectDlg::GetInstance()->m_stCondition);

	for(nIndex = 0; nIndex < pSet->m_nItemNum; ++nIndex)
	{
		m_csCondition.InsertString(nIndex, pSet->m_aszCondition[nIndex]);
	}

	if(nIndex > m_pstAgcdEventEffect->m_nCondition)
	{
		m_csCondition.SetCurSel(m_pstAgcdEventEffect->m_nCondition);
	}
	else
	{
		m_csCondition.EnableWindow(FALSE);
		::MessageBox(NULL, "AgcmUseEffectSetDlg::OnInitDialog()\n다이얼로그를 초기화할 수 없습니다.", "ERROR", MB_OK);
		//OutputDebugString("AgcmUseEffectSetDlg::OnInitDialog() Error (2) !!!\n");
		return FALSE;
	}*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmUseEffectSetDlg::OnOK() 
{
	// TODO: Add extra validation here
/*	UpdateData();

	INT16 nIndex = m_csIndex.GetCurSel();

	if(m_pstAgcdEventEffect->m_astData[nIndex])
	{
		m_pstAgcdEventEffect->m_astData[nIndex]->m_ulEID			= m_unEID;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_ulStartGap		= m_unGap;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.x		= m_fOffsetX;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.y		= m_fOffsetY;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.z		= m_fOffsetZ;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_lParentNodeID		= m_nParentNodeID;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_fScale			= m_fScale;	
//		m_pstAgcdEventEffect->m_astData[nIndex]->m_lCustData			= m_nCustData;
		if(m_strSoundName != "")
			strcpy(m_pstAgcdEventEffect->m_astData[nIndex]->m_szSoundName, (LPSTR)(LPCSTR)(m_strSoundName));
	}

//	(m_pstAgcdEventEffect + (nIndex * m_nDataSize))->m_nConditionFlags	= m_csCondition.GetCurSel();
//	m_pstAgcdEventEffect->m_pstTargetFrame	= NULL;
*/
	AgcmEffectDlg::GetInstance()->CloseUseEffectSet();

//	CDialog::OnOK();
}

void AgcmUseEffectSetDlg::OnButtonEidFind() 
{
	// TODO: Add your control notification handler code here
/*	INT32 lEID = AgcmEffectDlg::GetInstance()->OpenEffectList();

	if(lEID)
	{
		m_unEID = lEID;
		UpdateData(FALSE);
	}*/

/*	UINT32 ulEID;
	AgcmEffectList dlg(&ulEID);
	if(dlg.DoModal() == IDOK)
	{
		if(ulEID)
		{
			m_unEID = ulEID;
			UpdateData(FALSE);

			INT16 nIndex = m_csIndex.GetCurSel();
			if(!m_pstAgcdEventEffect->m_astData[nIndex])
			{
				m_pstAgcdEventEffect->m_astData[nIndex] = AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->CreateEffectData();
				if(!m_pstAgcdEventEffect->m_astData[nIndex])
				{
					OutputDebugString("AgcmUseEffectSetDlg::OnButtonEidFind() Error (77) !!!\n");
					return;
				}
			}
		}
	}*/
}

BOOL AgcmUseEffectSetDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void AgcmUseEffectSetDlg::OnUseEffectSetApply() 
{
	// TODO: Add your control notification handler code here
/*	UpdateData();

	INT16 nIndex = m_csIndex.GetCurSel();

	if(m_pstAgcdEventEffect->m_astData[nIndex])
	{
		m_pstAgcdEventEffect->m_astData[nIndex]->m_ulEID			= m_unEID;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_ulStartGap		= m_unGap;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.x		= m_fOffsetX;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.y		= m_fOffsetY;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.z		= m_fOffsetZ;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_lParentNodeID	= m_nParentNodeID;
		m_pstAgcdEventEffect->m_astData[nIndex]->m_fScale			= m_fScale;
//		m_pstAgcdEventEffect->m_astData[nIndex]->m_lCustData		= m_nCustData;
		if(m_strSoundName != "")
			strcpy(m_pstAgcdEventEffect->m_astData[nIndex]->m_szSoundName, (LPSTR)(LPCSTR)(m_strSoundName));
	}*/
//	(m_pstAgcdEventEffect + (nIndex * m_nDataSize))->m_nConditionFlag	= m_csCondition.GetCurSel();
}

void AgcmUseEffectSetDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	AgcmEffectDlg::GetInstance()->CloseUseEffectSet();
	
//	CDialog::OnClose();
}

void AgcmUseEffectSetDlg::OnSelchangeComboUesIndex() 
{
	// TODO: Add your control notification handler code here

	// 마지막에 입력한 값들을 가지고 온 후...
/*	UpdateData(TRUE);
	// EID가 SET되어 있으면 저장한다.
	if(m_unEID)
	{
		if(m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex])
		{
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_ulEID				= m_unEID;
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_ulStartGap		= m_unGap;
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_v3dOffset.x		= m_fOffsetX;
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_v3dOffset.y		= m_fOffsetY;
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_v3dOffset.z		= m_fOffsetZ;
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_lParentNodeID		= m_nParentNodeID;
			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_fScale			= m_fScale;
//			m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_lCustData			= m_nCustData;
			if(m_strSoundName != "")
				strcpy(m_pstAgcdEventEffect->m_astData[m_nPreEIDIndex]->m_szSoundName, (LPSTR)(LPCSTR)(m_strSoundName));
//			(m_pstAgcdEventEffect + (m_nPreEIDIndex * m_nDataSize))->m_nConditionFlag	= m_csCondition.GetCurSel();
		}
	}

	// 현재 인덱스를 가지고 온다.
	INT16 nIndex = m_csIndex.GetCurSel();

	// 현재 인덱스에 저장되어 있는 값들을 가지고 온 후...
	if(m_pstAgcdEventEffect->m_astData[nIndex])
	{
		m_unEID					= m_pstAgcdEventEffect->m_astData[nIndex]->m_ulEID;
		m_unGap					= m_pstAgcdEventEffect->m_astData[nIndex]->m_ulStartGap;
		m_fOffsetX				= m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.x;
		m_fOffsetY				= m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.y;
		m_fOffsetZ				= m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.z;
		m_nParentNodeID			= m_pstAgcdEventEffect->m_astData[nIndex]->m_lParentNodeID;
		m_fScale				= m_pstAgcdEventEffect->m_astData[nIndex]->m_fScale;
//		m_nCustData				= m_pstAgcdEventEffect->m_astData[nIndex]->m_lCustData;
		m_strSoundName			= m_pstAgcdEventEffect->m_astData[nIndex]->m_szSoundName;
	//	m_csCondition.SetCurSel((m_pstAgcdEventEffect + (nIndex * m_nDataSize))->m_nConditionFlag);
	}
	else
	{
		m_unEID					= 0;
		m_unGap					= 0;
		m_fOffsetX				= 0;
		m_fOffsetY				= 0;
		m_fOffsetZ				= 0;
		m_nParentNodeID			= 0;
		m_fScale				= 1;
//		m_nCustData				= -1;
		m_strSoundName			= _T("");
	}
	// dialog에 적용시킨다.
	UpdateData(FALSE);

	// 현재 인덱스를 저장한다.
	m_nPreEIDIndex = nIndex;*/
}

void AgcmUseEffectSetDlg::OnButtonEidReset() 
{
	// TODO: Add your control notification handler code here
/*	INT16 nIndex = m_csIndex.GetCurSel();

	if(m_pstAgcdEventEffect->m_astData[nIndex])
	{
		memset(m_pstAgcdEventEffect->m_astData[nIndex], 0, sizeof(AgcdUseEffectSetData));
		m_pstAgcdEventEffect->m_astData[nIndex]->m_fScale = 1.0f;

		m_unEID					= m_pstAgcdEventEffect->m_astData[nIndex]->m_ulEID;
		m_unGap					= m_pstAgcdEventEffect->m_astData[nIndex]->m_ulStartGap;
		m_fOffsetX				= m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.x;
		m_fOffsetY				= m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.y;
		m_fOffsetZ				= m_pstAgcdEventEffect->m_astData[nIndex]->m_v3dOffset.z;
		m_nParentNodeID			= m_pstAgcdEventEffect->m_astData[nIndex]->m_lParentNodeID;
		m_fScale				= m_pstAgcdEventEffect->m_astData[nIndex]->m_fScale;
//		m_nCustData				= m_pstAgcdEventEffect->m_astData[nIndex]->m_lCustData;
		m_strSoundName			= m_pstAgcdEventEffect->m_astData[nIndex]->m_szSoundName;
	//	m_csCondition.SetCurSel((m_pstAgcdEventEffect + (nIndex * m_nDataSize))->m_nConditionFlag);
		UpdateData(FALSE);
	}*/
}

void AgcmUseEffectSetDlg::OnButtonSetConditionFlag() 
{
	// TODO: Add your control notification handler code here
	if(AgcmEffectDlg::GetInstance())
	{
		INT16 nIndex = m_csIndex.GetCurSel();

		AgcmEffectDlg::GetInstance()->OpenEffectCondition(m_pstAgcdEventEffect, nIndex);
	}
}

void AgcmUseEffectSetDlg::OnButtonSetSound() 
{
	// TODO: Add your control notification handler code here
/*	AgcmFileListDlg dlg;
	m_strSoundName = dlg.OpenFileList(".\\sound\\effect\\*.wav", NULL , NULL );

	if(m_strSoundName != "")
	{
		INT16 nIndex = m_csIndex.GetCurSel();
		if(!m_pstAgcdEventEffect->m_astData[nIndex])
		{
			m_pstAgcdEventEffect->m_astData[nIndex] = AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->CreateEffectData();
			if(!m_pstAgcdEventEffect->m_astData[nIndex])
			{
				OutputDebugString("AgcmUseEffectSetDlg::OnButtonSetSound() Error (78) !!!\n");
				return;
			}
		}
	}

	UpdateData(FALSE);*/
}

void AgcmUseEffectSetDlg::OnButtonSetStatusFlags() 
{
	// TODO: Add your control notification handler code here
	if(AgcmEffectDlg::GetInstance())
	{
		INT16 nIndex = m_csIndex.GetCurSel();

		AgcmEffectDlg::GetInstance()->OpenEffectStatus(m_pstAgcdEventEffect, nIndex);
	}
}
