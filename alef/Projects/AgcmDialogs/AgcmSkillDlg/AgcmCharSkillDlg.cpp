// AgcmCharSkillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmskilldlg.h"
#include "../resource.h"
#include "AgcmCharSkillDlg.h"
#include "AgcmSkillTemplateListDlg.h"
#include "AgcmFileListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmCharSkillDlg dialog


AgcmCharSkillDlg::AgcmCharSkillDlg(AgcdCharacterTemplate *pstAgcdCharacterTemplate, AgpdSkillTemplateAttachData *pcsAgpdSkillTemplateAttachData, AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData, CHAR *szFindAnimPathName1, CHAR *szFindAnimPathName2, CWnd* pParent )
	: CDialog(AgcmCharSkillDlg::IDD, pParent),
	m_bAlwaysShow( FALSE )
{
	//{{AFX_DATA_INIT(AgcmCharSkillDlg)
//	m_strInfo = _T("");
//	m_strCurAnimFrame = _T("");
//	m_strAnim = _T("");
//	m_strSkillName = _T("");
	//}}AFX_DATA_INIT

	//m_lSkillIndex						= -1;
	//m_lWeaponType						= -1;
	//m_lEffectIndex					= -1;
	//m_lPreWeaponType					= 0;

	m_pstAgcdCharacterTemplate			= pstAgcdCharacterTemplate;
	m_pcsAgpdSkillTemplateAttachData	= pcsAgpdSkillTemplateAttachData;
	m_pcsAgcdSkillAttachTemplateData	= pcsAgcdSkillAttachTemplateData;

	strcpy(m_szFindAnimPathName1, szFindAnimPathName1);
	strcpy(m_szFindAnimPathName2, szFindAnimPathName2);

	InitializeAllEventButton();
}


void AgcmCharSkillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmCharSkillDlg)
	DDX_Control(pDX, IDC_COMBO_WEAPON_TYPE, m_csWeaponType);
	DDX_Control(pDX, IDC_TREE_SKILL, m_csSkillTree);
	DDX_Text(pDX, IDC_EDIT_SKILL_INFO, m_strInfo);
	DDX_Text(pDX, IDC_EDIT_CUR_ANIM, m_strCurAnimFrame);
	DDX_Text(pDX, IDC_EDIT_ANIMATION_NAME, m_strAnim);
	DDX_Text(pDX, IDC_EDIT_SKILL_NAME, m_strSkillName);
	DDX_Check(pDX , IDC_ALWAYS_SHOW , m_bAlwaysShow );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmCharSkillDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmCharSkillDlg)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SKILL, OnSelchangedTreeSkill)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_SKILL_INFO, OnButtonChangeSkillInfo)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EFFECT, OnButtonAddEffect)
	ON_BN_CLICKED(IDC_BUTTON_COPY_EFFECT, OnButtonCopyEffect)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CONDITION_LIST, OnButtonOpenConditionList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_STATUS_LIST, OnButtonOpenStatusList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_EFFECT_LIST, OnButtonOpenEffectList)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_SET_ANIMATION, OnButtonSetAnimation)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_EFFECT, OnButtonRemoveEffect)
	ON_CBN_SELCHANGE(IDC_COMBO_WEAPON_TYPE, OnSelchangeComboWeaponType)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_PLAY, OnButtonAnimPlay)
	ON_BN_CLICKED(IDC_BUTTON_ANIM_STOP, OnButtonAnimStop)
	ON_BN_CLICKED(IDC_BUTTON_SET_ANIM_PROPERTY, OnButtonSetAnimProperty)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CUST_DATA_LIST, OnButtonOpenCustDataList)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_HIDE_CASTING_EFFECT, &AgcmCharSkillDlg::OnBnClickedCheckHideCastingEffect)
	ON_BN_CLICKED(IDC_ALWAYS_SHOW, OnButtonAlwaysShow)
END_MESSAGE_MAP()

//
// AgcmCharSkillDlg message handlers

void AgcmCharSkillDlg::OnOK()
{
	// TODO: Add extra validation here
	if (AgcmSkillDlg::GetInstance())
	{
		if (AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg())
			AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg()->CloseEffectScript();

		AgcmSkillDlg::GetInstance()->CloseCharacterSkillDlg();
	}

//	CDialog::OnOK();
}

void AgcmCharSkillDlg::OnCancel()
{
	//if (AgcmSkillDlg::GetInstance())
	//{
	//	if (AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg())
	//		AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg()->CloseEffectScript();

	//	AgcmSkillDlg::GetInstance()->CloseCharacterSkillDlg();
	//}

	////	CDialog::OnCancel();
}

void AgcmCharSkillDlg::OnDestroy() 
{
//	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

BOOL AgcmCharSkillDlg::InitializeDlg(	AgcdCharacterTemplate		*pcsAgcdCharacterTemplate,
										AgpdSkillTemplateAttachData *pcsAgpdSkillTemplateAttachData,
										AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData	)
{
	m_lSkillIndex						= -1;
	m_lWeaponType						= -1;
	m_lEffectIndex						= -1;
	m_lPreWeaponType					= 0;

	m_pstAgcdCharacterTemplate			= pcsAgcdCharacterTemplate;
	m_pcsAgpdSkillTemplateAttachData	= pcsAgpdSkillTemplateAttachData;
	m_pcsAgcdSkillAttachTemplateData	= pcsAgcdSkillAttachTemplateData;

	DestroyAllEventButton();

	InitializeWeaponType();
	InitializeSkillTree();

	m_strInfo							= _T("");
	m_strCurAnimFrame					= _T("");
	m_strAnim							= _T("");
	m_strSkillName						= _T("");

	UpdateData(FALSE);

	return TRUE;
}

BOOL AgcmCharSkillDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	InitializeDlg(
		m_pstAgcdCharacterTemplate,
		m_pcsAgpdSkillTemplateAttachData,
		m_pcsAgcdSkillAttachTemplateData	);

	return TRUE;
}

INT32 AgcmCharSkillDlg::GetNumVisualInfo()
{
	switch (m_pstAgcdCharacterTemplate->m_lAnimType2)
	{
	case AGCMCHAR_AT2_WARRIOR:
	case AGCMCHAR_AT2_ARCHER:
	case AGCMCHAR_AT2_WIZARD:
	//@{ 2006/06/07 burumal
	case AGCMCHAR_AT2_ARCHLORD:
	//@}
		{
			return AGCDSKILL_MAX_VISUAL_INFO_PC;
		}
		break;

	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		{
			return AGCDSKILL_MAX_VISUAL_INFO_BOSS_MONSTER;
		}
		break;
	}

	return 0;
}

BOOL AgcmCharSkillDlg::GetSkillIDList( ListSkillID& listSkillID )
{
	listSkillID.clear();

	HTREEITEM hRoot = m_csSkillTree.GetRootItem();
	if( !hRoot )									return FALSE;
	if( !m_csSkillTree.ItemHasChildren( hRoot ) )	return TRUE;

	HTREEITEM hSecondStep = m_csSkillTree.GetChildItem( hRoot );
	while( hSecondStep )
	{
		if( m_csSkillTree.ItemHasChildren( hSecondStep ) )
		{
			HTREEITEM hItem = m_csSkillTree.GetChildItem( hSecondStep );
			while ( hItem )
			{
				listSkillID.push_back( (INT32)m_csSkillTree.GetItemData( hItem ) );
				hItem = m_csSkillTree.GetNextSiblingItem( hItem );
			}
		}

		hSecondStep = m_csSkillTree.GetNextSiblingItem( hSecondStep );
	}

	return TRUE;;
}

BOOL AgcmCharSkillDlg::InitializeSkillTree()
{
	m_csSkillTree.DeleteAllItems();

	HTREEITEM	hRoot = InsertSkillTreeItem(AGCM_CHAR_SKILL_DLG_TREE_ROOT_NAME, TVI_ROOT, AGCM_CHAR_SKILL_DLG_TREE_ROOT_DATA);

	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo)
		return TRUE;

	CHAR		szTemp[256], szTemp2[256];
	HTREEITEM	hSkill;
	INT32		lOwnSkillCount;
	INT32		lAnimType2;
	INT32		lNumAnimType2   = AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetAgcmCharacter()->GetAnimType2Num( m_pstAgcdCharacterTemplate );
	CHAR		**ppszAnimType2	= AgcmAnimationDlg::GetInstance()->GetAnimType2Name( m_pstAgcdCharacterTemplate->m_lAnimType2 );

	INT32		nSkillVisualInfo = GetNumVisualInfo();

	for (lOwnSkillCount = 0; lOwnSkillCount < nSkillVisualInfo; ++lOwnSkillCount)
	{
		if (m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[lOwnSkillCount])
		{
			if( m_pcsAgcdSkillAttachTemplateData == NULL )
				continue;

			if( m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount] == NULL )
				continue;

			hSkill = InsertSkillTreeItem(
				m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[lOwnSkillCount],
				hRoot,
				(DWORD)(lOwnSkillCount),
				TRUE );

			for (lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
			{
				if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2])
				{
					if(	(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_pacsAnimation[lAnimType2]) &&
						(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_pacsAnimation[lAnimType2]->m_pcsAnimation) &&
						(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead) &&
						(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
					{
						//sprintf(szTemp, "%s%d", ppszAnimType2[lAnimType2], lOwnSkillCount);
						strcpy( szTemp2, ppszAnimType2[lAnimType2] + 1 );
						szTemp2[strlen(szTemp2) - 1]	= '\0';
						sprintf( szTemp, "%s%d", szTemp2, lOwnSkillCount );

						m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2] =
							(CHAR *)(malloc(strlen(szTemp) + 1));

						strcpy(	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2], szTemp	);
					}
					else if((m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_pastEffect[lAnimType2]) &&
							(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_pastEffect[lAnimType2]->m_ulConditionFlags)	)
					{
						//sprintf(szTemp, "%s%d", ppszAnimType2[lAnimType2], lOwnSkillCount);
						strcpy(szTemp2, ppszAnimType2[lAnimType2] + 1);
						szTemp2[strlen(szTemp2) - 1]	= '\0';
						sprintf(szTemp, "%s%d", szTemp2, lOwnSkillCount);

						m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2] =
							(CHAR *)(malloc(strlen(szTemp) + 1));

						strcpy(	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2], szTemp );
					}
				}

				if(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2])
				{
					InsertSkillTreeItem(
						m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lOwnSkillCount]->m_paszInfo[lAnimType2],
						hSkill,
						(DWORD)(lAnimType2) );
				}
			}
		}
	}

	return TRUE;
}

HTREEITEM AgcmCharSkillDlg::InsertSkillTreeItem(CHAR *szName, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible)
{
	//추가한 스킬을 트리에 삽입
	HTREEITEM hItem = m_csSkillTree.InsertItem(szName, hParent, TVI_SORT);

	if (!hItem)
		return NULL;

	if (!m_csSkillTree.SetItemData(hItem, dwData))
		return NULL;

	if (bEnsureVisible)
		m_csSkillTree.EnsureVisible(hItem);

	return hItem;
}

void AgcmCharSkillDlg::OnSelchangedTreeSkill(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult						= 0;

	HTREEITEM	hParent				= m_csSkillTree.GetParentItem(pNMTreeView->itemNew.hItem);
	if (!hParent)
	{
//		InitializeSkillControl();
		return;
	}

	if (!m_csSkillTree.GetParentItem(hParent))
	{
//		InitializeSkillControl();
		return;
	}

	m_lSkillIndex		= (INT32)(m_csSkillTree.GetItemData(hParent));
	m_lWeaponType		= (INT32)(m_csSkillTree.GetItemData(pNMTreeView->itemNew.hItem));
	m_lPreWeaponType	= m_lWeaponType;
	m_lEffectIndex		= -1;

	UpdateSkillControl(FALSE);
}

BOOL AgcmCharSkillDlg::InitializeSkillControl()
{
	m_strInfo = "";
	m_strAnim = "";

	m_csWeaponType.SetCurSel(0);

	DestroyAllEventButton();

	return TRUE;
}

BOOL AgcmCharSkillDlg::MoveToMatchButton(INT32 lMatchIndex)
{
	// AgcmEventEffect module이 필요하다.
	if ((!AgcmSkillDlg::GetInstance()) || (!AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()))
		return FALSE;

	EventButtonCallback	pafCallback[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	CEventButtonParams	*pcsParams, acsParams[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	INT32				lNum;

	INT32 lIndex;

	// 이미 추가되었는지 검사.
	for (lIndex = 0; lIndex < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lIndex)
	{
		if (!m_pacsBaseButton[lIndex])
		{
			lNum = lIndex;
			break;
		}

		// 마지막 버튼이 아니라면...
		if (m_pacsBaseButton[lIndex + 1])
		{
			pcsParams = m_pacsBaseButton[lIndex]->GetParams();

			///gcdUseEffectSetData	*pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
			//	m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[(INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX])]
			//	.m_astEffect + (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]),
			//	(INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX])															);

			INT32	lSkillIndex	= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]);
			INT32	lWeaponType	= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]);

			AgcdUseEffectSetData	*pcsData	=
				AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
					m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_pastEffect[lWeaponType],
					(INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX])								);
			if (!pcsData)
				return FALSE;

			// 이미 해당 match index를 쓰는 놈이 있다면 스킵!
			//if (	AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetCustDataIndex(
			//			m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[(INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX])]
			//			.m_astEffect[(INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE])]
			//			.m_astData[(INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX])]->m_szCustData,
			//			D_AGCD_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH															) == lMatchIndex			)
			if (	(pcsData->m_pszCustData) &&
					(AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetCustDataIndex(
						pcsData->m_pszCustData,
						D_AGCD_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH							) == lMatchIndex)	)
				return TRUE; // skip
		}
	}

	// 추가 되지 않았다면, 현재 base button의 정보를 저장하고 button을 삭제한다.
	for (lIndex = 0; lIndex < lNum; ++lIndex)
	{
		if (m_pacsBaseButton[lIndex])
		{
			pafCallback[lIndex]		= m_pacsBaseButton[lIndex]->GetCallback();
			memcpy(&acsParams[lIndex], m_pacsBaseButton[lIndex]->GetParams(), sizeof(CEventButtonParams));

			delete m_pacsBaseButton[lIndex];
			m_pacsBaseButton[lIndex] = NULL;
		}
	}

	// Match 타입과 같으면 Match button으로, 아니면 base button으로 생성한다.
	for (lIndex = 0; lIndex < lNum; ++lIndex)
	{
		if (lIndex == lMatchIndex)
		{
			// Match button에 들어갈 인덱스는 마지막 버튼 인덱스인 (lNum - 1)이 들어가야 하지만,
			// Match button으로 button이 한개 감소하므로, (lNum - 2)가 된다.
			CHAR	szTemp[256];
			sprintf(szTemp, "%d", lNum - 2);

			CreateEventButton(
				E_AGCM_CHAR_SKILL_DLG_EB_MATCH,
				(INT32)(acsParams[lIndex].m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]),
				(INT32)(acsParams[lIndex].m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]),
				lIndex,
				szTemp																			);
		}
		else
		{			
			CreateEventButton(
				E_AGCM_CHAR_SKILL_DLG_EB_BASE,
				(INT32)(acsParams[lIndex].m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]),
				(INT32)(acsParams[lIndex].m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]),
				lIndex																			);
		}
	}

	return TRUE;
}

//BOOL AgcmCharSkillDlg::UpdateBaseButton(INT32 lMatchIndex, INT32 lDisplayIndex, INT32 lMaxIndex)
//{
//	if (!AgcmSkillDlg::GetInstance())
//		return FALSE;
//
//	AgcmEventEffect *pcsAgcmEventEffect = AgcmSkillDlg::GetInstance()->GetAgcmEventEffect();
//	if (!pcsAgcmEventEffect)
//		return FALSE;
//
//	CHAR					szTemp[256];
//	INT32					lIndex;
//	AgcdUseEffectSetData	*pstData;
//
//	// Base 버튼을 모두 지운다.
//	for (lIndex = 0; lIndex < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lIndex)
//	{
//		if (m_pacsBaseButton[lIndex])
//		{
//			delete m_pacsBaseButton[lIndex];
//			m_pacsBaseButton[lIndex] = NULL;
//		}
//	}
//
//	for (lIndex = 0; lIndex < lMaxIndex; ++lIndex)
//	{
//		if (lIndex == lMatchIndex)
//		{
//			sprintf(szTemp, "%d", lDisplayIndex);
//			CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_MATCH, m_lSkillIndex, m_lWeaponType, lIndex, szTemp);
//		}
//		else
//		{
//			CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_BASE, m_lSkillIndex, m_lWeaponType, lIndex);
//		}
//
//		if (pcsAgcmEventEffect->GetCustDataIndex(pstData->m_szCustData, D_AGCD_EFFECT_CUST_DATA_INDEX_ANIM_POINT) > -1)
//		{
//			lMatchIndex = pcsAgcmEventEffect->GetCustDataIndex(pstData->m_szCustData, D_AGCD_EFFECT_CUST_DATA_INDEX_SKILL_DESTROY_MATCH);
//			alTempMatchIndex[lAnimIndex++] = lMatchIndex;
//		}
//		else
//		{
//			for (lButtonIndex = 0; lButtonIndex < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lButtonIndex)
//			{
//				bEtc = TRUE;
//				if (alTempMatchIndex[lButtonIndex] == -1)
//				{
//					break;
//				}
//				else if (alTempMatchIndex[lMatchIndex] == lIndex)
//				{
//					bEtc = FALSE;
//					break;
//				}
//			}
//
//			if (bEtc)
//				CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_ETC, m_lSkillIndex, m_lWeaponType, lIndex);
//		}
//	}
//
//	return TRUE;
//}

BOOL AgcmCharSkillDlg::UpdateSkillControl(BOOL bEnable)
{
	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0))
		return TRUE;	// skip!

	if ((!AgcmSkillDlg::GetInstance()) || (!AgcmEffectDlg::GetInstance()))
		return FALSE;

	AgcmEventEffect *pcsAgcmEventEffect = AgcmSkillDlg::GetInstance()->GetAgcmEventEffect();
	if (!pcsAgcmEventEffect)
		return FALSE;

	if (m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[m_lSkillIndex])
		m_strSkillName = m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[m_lSkillIndex];
	else
		m_strSkillName = "";
	if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszInfo[m_lWeaponType])
		m_strInfo = m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszInfo[m_lWeaponType];
	else
		m_strInfo = "";
	/*if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszInfoName[m_lWeaponType])
		m_strInfo = m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszInfoName[m_lWeaponType];
	else
		m_strInfo = "";*/
	//if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszAnimName[m_lWeaponType])
	if (	(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]) &&
			(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation) &&
			(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation->m_pcsHead) &&
			(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
		m_strAnim = m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName;
		//m_strAnim = m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszAnimName[m_lWeaponType];
	else
		m_strAnim = "";

	switch( m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_eShowOption )
	{
	case AgcmSkillVisualInfo::DEFEND_ON_OPTION	:	m_bAlwaysShow = FALSE	; break;
	case AgcmSkillVisualInfo::ALWAYS_SHOW		:	m_bAlwaysShow = TRUE	; break;
	}

	UpdateData(FALSE);

	m_csWeaponType.SetCurSel(m_lWeaponType);

	DestroyAllEventButton();

	CHAR	szTemp[256];
	INT32	lMatchIndex = 0, lMatchCount = 0, lTempMatchIndexCount = 0, lTemp = 0;
	INT32	alTempMatchIndex[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	INT32	alTempDisplayIndex[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];

	memset(szTemp, 0, 256);
	memset(alTempMatchIndex, -1, sizeof(INT32) * AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM);
	memset(alTempDisplayIndex, -1, sizeof(INT32) * AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM);

	//if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_ulConditionFlags)
	if (	(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]) &&
			(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]->m_ulConditionFlags)	)
	{
		lTempMatchIndexCount	= 0;
//		for (lEffectIndex = 0; lEffectIndex < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lEffectIndex)
		AgcdUseEffectSetList	*pcsCurrent	=
			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]->m_pcsHead;
		while (pcsCurrent)
		{
//			pstData = m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[lEffectIndex];

//			if (	(!pstData) || ((!pstData->m_ulEID) && (!pstData->m_szSoundName[0]) && (!pstData->m_ulConditionFlags))	)
			if (	((!pcsCurrent->m_csData.m_ulEID) && (!pcsCurrent->m_csData.m_pszSoundName) && (!pcsCurrent->m_csData.m_ulConditionFlags))	)
			{
				pcsCurrent	= pcsCurrent->m_pcsNext;
				continue;
			}

			for (lMatchCount = 0; lMatchCount < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lMatchCount)
			{
				// 더이상 비교할 match index가 없으면.
				if (alTempMatchIndex[lMatchCount] == -1)
				{
					// BaseButton으로 추가한다.
//					lTemp = CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_BASE, m_lSkillIndex, m_lWeaponType, lEffectIndex);
					lTemp = CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_BASE, m_lSkillIndex, m_lWeaponType, pcsCurrent->m_csData.m_ulIndex);
					// 추가실패시!
					if (-1 == lTemp)
					{
						MessageBox("ERROR!-관리자에게 문의하세요", "ERROR");
						return FALSE;
					}

					// 현재 데이터의 match index를 가져온다.
//					lMatchIndex = pcsAgcmEventEffect->GetCustDataIndex(pstData->m_szCustData, D_AGCD_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH);
					if (pcsCurrent->m_csData.m_pszCustData)
						lMatchIndex = pcsAgcmEventEffect->GetCustDataIndex(pcsCurrent->m_csData.m_pszCustData, D_AGCD_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH);
					else
						lMatchIndex	= -1;

					// match index가 있으면...
					if (lMatchIndex > -1)
					{
						// match index가 이미 effect index를 지나쳤다면.
//						if (lMatchIndex < lEffectIndex)
						if (lMatchIndex < pcsCurrent->m_csData.m_ulIndex)
						{
							// 해당 BaseButton을 MatchButton으로 옮긴다.
							MoveToMatchButton(lMatchIndex);
						} // 아직 match index가 effect index를 지나치지 않았다면.
//						else if (lMatchIndex > lEffectIndex)
						else if (lMatchIndex > pcsCurrent->m_csData.m_ulIndex)
						{
							alTempMatchIndex[lTempMatchIndexCount]		= lMatchIndex;
							alTempDisplayIndex[lTempMatchIndexCount]	= lTemp;
							++lTempMatchIndexCount;
						}
						else // error
						{
							MessageBox("ERROR!-관리자에게 문의하세요", "ERROR");
							return FALSE;
						}

						break;
					}

					break;
				} // 현재 effect index가 match index와 같다면.
//				else if (alTempMatchIndex[lMatchCount] == lEffectIndex)
				else if (alTempMatchIndex[lMatchCount] == pcsCurrent->m_csData.m_ulIndex)
				{
					// 새로 들어갈 MatchButton의 display index를 설정하고.
					sprintf(szTemp, "%d", alTempDisplayIndex[lMatchCount]);

					// MatchButton을 생성한다.
//					if (-1 == CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_MATCH, m_lSkillIndex, m_lWeaponType, lEffectIndex, szTemp))
					if (-1 == CreateEventButton(E_AGCM_CHAR_SKILL_DLG_EB_MATCH, m_lSkillIndex, m_lWeaponType, pcsCurrent->m_csData.m_ulIndex, szTemp))
					{
						MessageBox("ERROR!-관리자에게 문의하세요", "ERROR");
						return FALSE;
					}

					break;
				}
			}

			pcsCurrent	= pcsCurrent->m_pcsNext;
		}
	}

	AgcmEffectDlg::GetInstance()->EnableEffectFlagsDlg(bEnable);

	return TRUE;
}

BOOL AgcmCharSkillDlg::InitializeWeaponType()
{
	m_csWeaponType.ResetContent();

	INT32	lNumAnimType2	=
		AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetAgcmCharacter()->GetAnimType2Num(
			m_pstAgcdCharacterTemplate														);

	CHAR	**ppszAnimType2	=
		AgcmAnimationDlg::GetInstance()->GetAnimType2Name(m_pstAgcdCharacterTemplate->m_lAnimType2);

	INT32 lAnimType2;

	for (lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
	{
		m_csWeaponType.InsertString(lAnimType2, ppszAnimType2[lAnimType2]);
	}

	if (lAnimType2 > 0)
		m_csWeaponType.SetCurSel(0);

	//for (INT32 lEquipType = 0; lEquipType < AGCDITEM_EQUIP_ANIMATION_TYPE_NUM; ++lEquipType)
	//{
	//	m_csWeaponType.InsertString(lEquipType, AgcmSkillDlg::GetInstance()->GetAgcmItem()->GetItemEquipAnimationName(lEquipType));
	//}

	//if (lEquipType > 0)
	//	m_csWeaponType.SetCurSel(0);

	return TRUE;
}

INT32 AgcmCharSkillDlg::CreateEventButton(eAgcmCharSkillDlgEventButton eType, INT32 lSkillIndex, INT32 lWeaponType, INT32 lEffectIndex, CHAR *szForceText)
{
	CHAR			szTemp[256];
	CEventButton	**ppButton = NULL;
	INT32			lIndex, lDivIndex;
	INT32			lX, lY;

	for (lIndex = 0; lIndex < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lIndex)
	{
		switch (eType)
		{
		//case E_AGCM_CHAR_SKILL_DLG_EB_ANIMATION:
		//	{
		//		if (!m_pacsAnimationButton[lIndex])
		//		{
		//			ppButton	= &m_pacsAnimationButton[lIndex];
		//			lX			= AGCM_CHAR_SKILL_DLG_EBS_COMMON_X;
		//			lY			= AGCM_CHAR_SKILL_DLG_EBS_ANIM_Y;

		//			sprintf(szTemp, "%c", 'A' + lIndex);
		//		}
		//	}
		//	break;

		case E_AGCM_CHAR_SKILL_DLG_EB_MATCH:
			{
				if (!m_pacsMatchButton[lIndex])
				{
					ppButton = &m_pacsMatchButton[lIndex];
					lX			= AGCM_CHAR_SKILL_DLG_EBS_COMMON_X;
					lY			= AGCM_CHAR_SKILL_DLG_EBS_MATCH_Y;

					strcpy(szTemp, "");
				}
			}
			break;

		case E_AGCM_CHAR_SKILL_DLG_EB_BASE:
			{
				if (!m_pacsBaseButton[lIndex])
				{
					ppButton	= &m_pacsBaseButton[lIndex];
					lX			= AGCM_CHAR_SKILL_DLG_EBS_COMMON_X;
					lY			= AGCM_CHAR_SKILL_DLG_EBS_BASE_Y;

					sprintf(szTemp, "%d", lIndex);
				}
			}
			break;

		default:
			return -1;
		}

		if (ppButton)
			break;
	}

	if (!ppButton)
		return -1;

	lDivIndex		= lIndex % AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_X_NUM;	
	if (lIndex >= AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_X_NUM)
		lY			= lY + AGCM_CHAR_SKILL_DLG_EBS_COMMON_EXTENT + AGCM_CHAR_SKILL_DLG_EBS_COMMON_BLANK;

	RECT	rtRect	= {	(LONG)(lX + (AGCM_CHAR_SKILL_DLG_EBS_COMMON_EXTENT * lDivIndex) + (AGCM_CHAR_SKILL_DLG_EBS_COMMON_BLANK * lDivIndex)),
						(LONG)(lY),
						(LONG)(lX + (AGCM_CHAR_SKILL_DLG_EBS_COMMON_EXTENT * (lDivIndex + 1)) + (AGCM_CHAR_SKILL_DLG_EBS_COMMON_BLANK * lDivIndex)),
						(LONG)(lY + AGCM_CHAR_SKILL_DLG_EBS_COMMON_EXTENT)	};

	CEventButtonParams csParams;
	csParams.m_pvClass											= (PVOID)(this);
	csParams.m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]	= (PVOID)(lSkillIndex);
	csParams.m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]	= (PVOID)(lWeaponType);
	csParams.m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX]	= (PVOID)(lEffectIndex);

	*(ppButton) = new CEventButton(
		this, 
		rtRect, 
		( LPCSTR ) ( (szForceText) ?  (szForceText) : (szTemp) ), 
		( int ) TRUE, 
		AgcmCharSkillDlg::EventButtonCB, 
		&csParams);

	return lIndex;
}

BOOL AgcmCharSkillDlg::InitializeAllEventButton()
{
	memset(m_pacsMatchButton,		0, sizeof(CEventButton *) * AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM);
	memset(m_pacsBaseButton,		0, sizeof(CEventButton *) * AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM);

	return TRUE;
}

BOOL AgcmCharSkillDlg::DestroyAllEventButton()
{
	for (INT32 lIndex = 0; lIndex < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lIndex)
	{
		if (m_pacsMatchButton[lIndex])
			delete m_pacsMatchButton[lIndex];

		if (m_pacsBaseButton[lIndex])
			delete m_pacsBaseButton[lIndex];
	}

	InitializeAllEventButton();

	return TRUE;
}

BOOL AgcmCharSkillDlg::EventButtonCB(PVOID pvData)
{
	CEventButtonParams	*params			= (CEventButtonParams *)(pvData);
	AgcmCharSkillDlg	*this_class		= (AgcmCharSkillDlg *)(params->m_pvClass);

	INT32				lSkillIndex		= (INT32)(params->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]);
	INT32				lWeaponType		= (INT32)(params->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]);
	INT32				lEffectIndex	= (INT32)(params->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX]);

	if ((lSkillIndex < 0) || (lWeaponType < 0) || (lEffectIndex < 0))
		return FALSE;

	AgcdUseEffectSetData	*pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
		this_class->m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_pastEffect[lWeaponType], lEffectIndex	);
//	if (!this_class->m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_astEffect[lWeaponType].m_astData[lEffectIndex])
	if (!pcsData)
		return FALSE;

	this_class->m_lEffectIndex			= lEffectIndex;

	return this_class->UpdateEffectDlg(
		this_class->m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_pastEffect[lWeaponType],
		lEffectIndex
		);
}

BOOL AgcmCharSkillDlg::UpdateEffectDlg(AgcdUseEffectSet *pstSet, INT32 lIndex)
{
	if (AgcmEffectDlg::GetInstance())
	{
		AgcmEffectDlg::GetInstance()->OpenEffectScript();
		AgcmEffectDlg::GetInstance()->UpdateEffectScript(pstSet, lIndex);

		AgcdUseEffectSetData	*pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(pstSet, lIndex);
//		if (pstSet->m_astData[lIndex])		
			//AgcmEffectDlg::GetInstance()->UpdateEffectFlagsDlg(pstSet->m_astData[lIndex]);
		if (pcsData)
			AgcmEffectDlg::GetInstance()->UpdateEffectFlagsDlg(pcsData);
		else
			AgcmEffectDlg::GetInstance()->CloseAllEffectDlg();
	}

	return TRUE;
}

BOOL AgcmCharSkillDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::Create(IDD, pParentWnd);
}

//BOOL AgcmCharSkillDlg::UpdateButton()//AgcdUseEffectSetData *pstData)
//{
//	if (!AgcmSkillDlg::GetInstance())
//		return FALSE;
//	
//	if (!AgcmSkillDlg::GetInstance()->GetAgcmEventEffect())
//		return FALSE;
//
//	INT32 lMatchIndex = AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetCustDataIndex(pstData->m_szCustData, D_AGCD_EFFECT_CUST_DATA_INDEX_MATCH);
//	if (lMatchIndex == -1)
//		return TRUE;
//
//	if (!pstData)
//		return FALSE;
//
//	CHAR	szText[256];
//	INT32	lEffectIndex, lSkillIndex, lWeaponType;
//
//	for (INT32 lCount = 0; lCount < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lCount)
//	{
//		CEventButtonParams *pcsParams = m_pacsAnimationButton[lCount]->GetParams();
//
//		lSkillIndex		= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]);
//		lWeaponType		= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]);
//		lEffectIndex	= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX]);
//		if ((lSkillIndex < 0) || (lWeaponType < 0) || (lEffectIndex < 0))
//			return FALSE;
//
//		if (m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_astEffect[lWeaponType].m_astData[lEffectIndex] != pstData)
//			continue;
//
//		sprintf(szText, "%c", 'A' + lCount);
//		break;
//	}
//
//	if (lCount == AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM)
//	{
//		MessageBox("ERROR!", "ERROR");
//		return FALSE;
//	}
//
//	for (lCount = 0; lCount < AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM; ++lCount)
//	{
//		if (!m_pacsMatchButton[lCount])
//			break;
//
//		CEventButtonParams *pcsParams = m_pacsMatchButton[lCount]->GetParams();
//
//		lEffectIndex	= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX]);
//		if (lEffectIndex != lMatchIndex)
//			continue;
//
//		lSkillIndex		= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX]);
//		lWeaponType		= (INT32)(pcsParams->m_pavData[E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE]);
//		
//		if ((lSkillIndex < 0) || (lWeaponType < 0) || (lEffectIndex < 0))
//			return FALSE;
//
//		m_pacsMatchButton[lCount]->SetWindowText(szText);
//	}
//
//	return TRUE;
//}

BOOL AgcmCharSkillDlg::AddButton(BOOL bCopyCurData)
{
	if (!AgcmSkillDlg::GetInstance())
		return FALSE;

	if (!AgcmSkillDlg::GetInstance()->GetAgcmEventEffect())
		return FALSE;

	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0))
		return FALSE;

	//INT32					lBtIndex;
	//AgcdUseEffectSetData	**ppstData;
	//for (lBtIndex = 0; lBtIndex < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lBtIndex)
	//{
	//	ppstData =	&(m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[lBtIndex]);
	//	if (!(*ppstData))
	//	{
	//		*ppstData = AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->CreateEffectData();
	//		if (!(*ppstData))
	//			return FALSE;

	//		m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_ulConditionFlags |= AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
	//		(*ppstData)->m_ulConditionFlags = AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;

	//		break;
	//	}

	//	if (	(!((*ppstData)->m_ulEID)) && (!((*ppstData)->m_szSoundName[0])) && (!((*ppstData)->m_ulConditionFlags))	)
	//	{
	//		m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_ulConditionFlags |= AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
	//		(*ppstData)->m_ulConditionFlags = AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
	//		break;
	//	}
	//}

	//if (!(*ppstData))
	//	return FALSE;

	//if (bCopyCurData)
	//{
	//	memcpy(*ppstData, m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex], sizeof(AgcdUseEffectSetData));
	//}

	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType])
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]	= new AgcdUseEffectSet();

	AgcdUseEffectSetList	*pcsCurrent	=
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]->m_pcsHead;
	while (pcsCurrent)
	{
		if ((!pcsCurrent->m_csData.m_ulEID) && (!pcsCurrent->m_csData.m_pszSoundName))
		{
			pcsCurrent->m_csData.m_ulConditionFlags	|= AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
			break;
		}

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	AgcdUseEffectSetData	*pcsData	= NULL;
	if (!pcsCurrent)
	{
		for (INT32 lCount = 0; ; ++lCount)
		{
			pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
				m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], lCount	);

			if (!pcsData)
			{
				pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
					m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], lCount, TRUE	);

				if (!pcsData)
					return FALSE;

				pcsData->m_ulConditionFlags = AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
				m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]->m_ulConditionFlags |= AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
				break;
			}
		}
	}
	else
	{
		pcsData	= &pcsCurrent->m_csData;
	}

	if (bCopyCurData)
	{
		AgcdUseEffectSetData	*pcsSrcData	=
			AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType],
				m_lEffectIndex																						);
		if (!pcsSrcData)
			return FALSE;

		AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->CopyData(pcsData, pcsSrcData);
	}

	UpdateSkillControl(FALSE);

	return TRUE;
}

BOOL AgcmCharSkillDlg::RemoveButton()
{
	if ((m_lSkillIndex > -1) && (m_lWeaponType > -1) && (m_lEffectIndex > -1))
	{
//		if (!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex])
//			return FALSE;

		AgcdUseEffectSetData	*pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], m_lEffectIndex	);
		if (!pcsData)
			return FALSE;

//		memset(&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex], 0, sizeof(AgcdUseEffectSetData));
		AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->Delete(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], pcsData);

		if (!AgcmSkillDlg::GetInstance())
			return FALSE;

		if (!AgcmSkillDlg::GetInstance()->GetAgcmEventEffect())
			return FALSE;

//		AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->ReSortUseEffectSet(&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType], m_lEffectIndex);

		UpdateSkillControl(FALSE);
	}

	return TRUE;
}

//void AgcmCharSkillDlg::OnButtonEffectTableAnimAdd() 
//{
//	// TODO: Add your control notification handler code here
//	if (m_pacsAnimationButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
//	{
//		MessageBox("더이상 추가할 수 없습니다!", "MAX");
//		return;
//	}
//
//	AddButton(E_AGCM_CHAR_SKILL_DLG_EB_ANIMATION);
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableAnimCopy() 
//{
//	// TODO: Add your control notification handler code here
//	if (m_pacsAnimationButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
//	{
//		MessageBox("더이상 추가할 수 없습니다!", "MAX");
//		return;
//	}
//
//	if (!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex])
//	{
//		MessageBox("COPY할 데이터가 없습니다!", "WARNNING");
//		return;
//	}
//
//	AddButton(E_AGCM_CHAR_SKILL_DLG_EB_ANIMATION, TRUE);
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableAnimRemove() 
//{
//	// TODO: Add your control notification handler code here
//	RemoveButton();
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableEtcAdd() 
//{
//	// TODO: Add your control notification handler code here
//	if (m_pacsEtcButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
//	{
//		MessageBox("더이상 추가할 수 없습니다!", "MAX");
//		return;
//	}
//
//	AddButton(E_AGCM_CHAR_SKILL_DLG_EB_ETC);
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableEtcCopy() 
//{
//	// TODO: Add your control notification handler code here
//	if (m_pacsEtcButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
//	{
//		MessageBox("더이상 추가할 수 없습니다!", "MAX");
//		return;
//	}
//
//	if (!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex])
//	{
//		MessageBox("COPY할 데이터가 없습니다!", "WARNNING");
//		return;
//	}
//
//	AddButton(E_AGCM_CHAR_SKILL_DLG_EB_ETC, TRUE);
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableEtcRemove() 
//{
//	// TODO: Add your control notification handler code here
//	RemoveButton();
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableMatchAdd() 
//{
//	// TODO: Add your control notification handler code here
//	if (m_pacsMatchButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
//	{
//		MessageBox("더이상 추가할 수 없습니다!", "MAX");
//		return;
//	}
//
//	AddButton(E_AGCM_CHAR_SKILL_DLG_EB_MATCH);
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableMatchCopy() 
//{
//	// TODO: Add your control notification handler code here
//	if (m_pacsMatchButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
//	{
//		MessageBox("더이상 추가할 수 없습니다!", "MAX");
//		return;
//	}
//
//	if (!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex])
//	{
//		MessageBox("COPY할 데이터가 없습니다!", "WARNNING");
//		return;
//	}
//
//	AddButton(E_AGCM_CHAR_SKILL_DLG_EB_MATCH, TRUE);
//}
//
//void AgcmCharSkillDlg::OnButtonEffectTableMatchRemove() 
//{
//	// TODO: Add your control notification handler code here
//	RemoveButton();
//}

void AgcmCharSkillDlg::OnButtonChangeSkillInfo() 
{
	// TODO: Add your control notification handler code here
	if ((m_lSkillIndex == -1) || (m_lWeaponType == -1))
	{
		MessageBox("변경할 오브젝트가 없습니다!", "WARNNING");
		return;
	}

	UpdateData(TRUE);

	strcpy(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszInfo[m_lWeaponType], m_strInfo);

	HTREEITEM hItem = FindSkillTreeItem(m_lSkillIndex, m_lWeaponType);
	if (!hItem)
		return;

	m_csSkillTree.SetItemText(hItem, m_strInfo);
	m_csSkillTree.EnsureVisible(hItem);
}

HTREEITEM AgcmCharSkillDlg::FindSkillTreeItem(INT32 lSkillIndex, INT32 lWeaponType)
{
	HTREEITEM	hRoot	= m_csSkillTree.GetRootItem();
	HTREEITEM	hItem	= m_csSkillTree.GetChildItem(hRoot);

	while (hItem)
	{
		if (lSkillIndex == m_csSkillTree.GetItemData(hItem))
			break;

		hItem = m_csSkillTree.GetNextItem(hItem, TVGN_NEXT);
	}

	if (lWeaponType == -1)
		return hItem;

	hItem				= m_csSkillTree.GetChildItem(hItem);

	while (hItem)
	{
		if (lWeaponType == m_csSkillTree.GetItemData(hItem))
			break;

		hItem			= m_csSkillTree.GetNextItem(hItem, TVGN_NEXT);
	}

	return hItem;
}

void AgcmCharSkillDlg::OnButtonAddEffect() 
{
	// TODO: Add your control notification handler code here
	if (m_pacsBaseButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
	{
		MessageBox("더이상 추가할 수 없습니다!", "MAX");
		return;
	}

	if ((m_lSkillIndex == -1) || (m_lWeaponType == -1))
	{
		MessageBox("추가할 객체가 없습니다!", "WARNNING");
		return;
	}

	AddButton();
}

void AgcmCharSkillDlg::OnButtonCopyEffect() 
{
	// TODO: Add your control notification handler code here
	if (m_pacsBaseButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM - 1])
	{
		MessageBox("더이상 추가할 수 없습니다!", "MAX");
		return;
	}

	if (	(m_lSkillIndex == -1) ||
			(m_lWeaponType == -1) ||
			(m_lEffectIndex == -1) ||
			!AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
				m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], m_lEffectIndex	)	)
//			(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex])	)
	{
		MessageBox("COPY할 데이터가 없습니다!", "WARNNING");
		return;
	}

	AddButton(TRUE);
}

BOOL AgcmCharSkillDlg::RemoveSkillAnimation(INT32 lSkillIndex, INT32 lWeaponType)
{
	//존재하지 않는 애니메이션이라면 패스.
	if(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex])
	{
		AAD_RemoveAllAnimationParams	csRemoveAllAnimationParams;

		csRemoveAllAnimationParams.m_ppSrcAnimation	=
			&m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_pacsAnimation[lWeaponType]->m_pcsAnimation;
		csRemoveAllAnimationParams.m_bApply	= FALSE;

		AgcmSkillDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);
	}

	/*if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex].m_pastAnim[lWeaponType])
	{
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex].m_pastAnim[lWeaponType]->m_ppastAnimData[0] = NULL;
	}

	if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex].m_paszAnimName[lWeaponType])
	{
		free(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex].m_paszAnimName[lWeaponType]);
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex].m_paszAnimName[lWeaponType] = NULL;
	}*/

	return TRUE;
}

BOOL AgcmCharSkillDlg::RemoveSkillEffect(INT32 lSkillIndex, INT32 lWeaponType)
{
	//for (INT32 lEffectIndex = 0; lEffectIndex < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lEffectIndex)
	//{
	//	if (m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_astEffect[lWeaponType].m_astData[lEffectIndex])
	//	{
	//		memset(	m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_astEffect[lWeaponType].m_astData[lEffectIndex],
	//				0,
	//				sizeof(AgcdUseEffectSetData)																							);

	//		m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_astEffect[lWeaponType].m_astData[lEffectIndex]->m_fScale = 1.0f;
	//	}
	//}

	//.존재하지 않는 Effect라면 패스.
	if(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex])
	{
		AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->DeleteAll(
			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_pastEffect[lWeaponType]	);
	}

	return TRUE;
}

BOOL AgcmCharSkillDlg::RemoveSkillInfo(INT32 lSkillIndex, INT32 lWeaponType)
{
	if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex])
	{
		if(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lWeaponType])
		{
			free(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lWeaponType]);
			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lWeaponType] = NULL;
		}
	}

	return TRUE;
}

//. 2005. 8. 30 Nonstopdj
//. pSkillName을 가진 모든 트리아이템을 삭제
void AgcmCharSkillDlg::RemoveSameNameSkillTreeItem(CHAR* pSkillName)
{
	HTREEITEM hCurrent = m_csSkillTree.GetRootItem();

	//. root에서 다음번 Item부터 시작한다.
	//. root는 [HIERARCHY]
	hCurrent = m_csSkillTree.GetNextItem( hCurrent, TVGN_CHILD );

	while (hCurrent != NULL)
	{
		// Get the text for the item. Notice we use TVIF_TEXT because
		// we want to retrieve only the text, but also specify TVIF_HANDLE
		// because we're getting the item by its handle.
		TVITEM item;
		CHAR szText[512];
		item.hItem = hCurrent;
		item.mask = TVIF_TEXT | TVIF_HANDLE;
		item.pszText = szText;
		item.cchTextMax = 512;

		BOOL bWorked = m_csSkillTree.GetItem(&item);

		// Try to get the next item
		hCurrent = m_csSkillTree.GetNextItem(hCurrent, TVGN_NEXT);

		if (bWorked && !strcmp(item.pszText, pSkillName))
			m_csSkillTree.DeleteItem(item.hItem);
	}
}

BOOL AgcmCharSkillDlg::RemoveSkillTreeItem(HTREEITEM hSkill)
{
	INT32	lSkillIndex		= (INT32)(m_csSkillTree.GetItemData(hSkill));
	
	if (lSkillIndex < 0)
		return FALSE;

	//. 2005. 8. 30 Nonstopdj
	//. ISkillIndex로 해당 스킬의 이름을 얻어온후 같은 이름으로 등록된 모든 스킬들을 삭제한다.
	//. 총소유할 수 있는 스킬은 AGPMSKILL_MAX_SKILL_OWN = 60
	CHAR* pDeleteSkillName = m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[lSkillIndex];

	for(int nCount = 0; nCount < AGPMSKILL_MAX_SKILL_OWN;nCount++)
	{
		//등록이 안되어 있는 비어있는 스킬이름 slot일 경우 패스.
		if(m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[nCount] == NULL)
			continue;

		if (!strcmp(m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[nCount], pDeleteSkillName))
		{
			m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[nCount] = NULL;

			INT32	lNumAnimType2	=
				AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetAgcmCharacter()->GetAnimType2Num(m_pstAgcdCharacterTemplate);

			//for (INT32 lWeaponType = 0; lWeaponType < AGCDITEM_EQUIP_ANIMATION_TYPE_NUM; ++lWeaponType)
			for (INT32 lWeaponType = 0; lWeaponType < lNumAnimType2; ++lWeaponType)
			{
				if (!RemoveSkillAnimation(nCount, lWeaponType))
					return FALSE;

				if (!RemoveSkillEffect(nCount, lWeaponType))
					return FALSE;

				if (!RemoveSkillInfo(nCount, lWeaponType))
					return FALSE;
			}
		}
	}

	RemoveSameNameSkillTreeItem(pDeleteSkillName);

	return TRUE;
}

BOOL AgcmCharSkillDlg::RemoveAllSkillTreeItem()
{
	HTREEITEM	hRoot	= m_csSkillTree.GetRootItem();
	if (!hRoot)
		return FALSE;

	HTREEITEM	hSkill	= m_csSkillTree.GetChildItem(hRoot);
	if (!hSkill)
		return TRUE; // skip

	HTREEITEM	hTemp;
	while (hSkill)
	{
		hTemp			= m_csSkillTree.GetNextItem(hSkill, TVGN_NEXT);
		if (!RemoveSkillTreeItem(hSkill))
			return FALSE;

		hSkill			= hTemp;
	}

	return TRUE;
}

void AgcmCharSkillDlg::OnButtonRemove()
{
	// TODO: Add your control notification handler code here
	HTREEITEM	hSelected	= m_csSkillTree.GetSelectedItem();
	if (!hSelected)
		return;

	HTREEITEM	hRoot		= m_csSkillTree.GetRootItem();
	if (!hRoot)
		return;

	if (hSelected == hRoot) // remove all!
	{
		RemoveAllSkillTreeItem();
	}
	else
	{
		HTREEITEM hParent	= m_csSkillTree.GetParentItem(hSelected);
		if (!hParent)
			return;

		if (hParent == hRoot) // remove skill!
		{
			RemoveSkillTreeItem(hSelected);
		}
		else // remove skill! (weapon type)
		{
			INT32	lSkillIndex	= (INT32)(m_csSkillTree.GetItemData(hParent));
			INT32	lWeaponType	= (INT32)(m_csSkillTree.GetItemData(hSelected));

			if ((lSkillIndex < 0) || (lWeaponType < 0))
				return;

			RemoveSkillAnimation(lSkillIndex, lWeaponType);
			RemoveSkillEffect(lSkillIndex, lWeaponType);
			RemoveSkillInfo(lSkillIndex, lWeaponType);

			m_csSkillTree.DeleteItem(hSelected);
		}
	}

	UpdateSkillControl(FALSE);

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->EnableEffectFlagsDlg(FALSE);
}

void AgcmCharSkillDlg::OnButtonOpenConditionList() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0) || (m_lEffectIndex < 0))
		return;

	AgcdUseEffectSetData	*pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], m_lEffectIndex	);
	if (!pcsData)
		return;

	AgcmEffectDlg::GetInstance()->OpenEffectConditionList(
//		&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex]->m_ulConditionFlags,
//		&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex]->m_ulConditionFlags	);
		&pcsData->m_ulConditionFlags,
		&pcsData->m_ulConditionFlags						);
//		&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex]->m_ulSSConditionFlags	);
}

void AgcmCharSkillDlg::OnButtonOpenStatusList() 
{
	// TODO: Add your control notification handler code here
	//if (!AgcmEffectDlg::GetInstance())
	//	return;

	//if ((m_lSkillIndex < 0) || (m_lWeaponType < 0) || (m_lEffectIndex < 0))
	//	return;

	//AgcmEffectDlg::GetInstance()->OpenEffectStatusList(&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex]->m_ulStatusFlags);
}

void AgcmCharSkillDlg::OnButtonOpenEffectList()
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0) || (m_lEffectIndex < 0))
		return;

	AgcdUseEffectSetData	*pcsData	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType], m_lEffectIndex	);
	if (!pcsData)
		return;

//	AgcmEffectDlg::GetInstance()->OpenEffectList(&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[m_lEffectIndex]->m_ulEID);
	AgcmEffectDlg::GetInstance()->OpenEffectList(&pcsData->m_ulEID);
}

//BOOL AgcmCharSkillDlg::UpdateSearchCondition()
//{
//	if ((m_lSkillIndex == -1) || (m_lWeaponType == -1))
//		return FALSE;
//
//	m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_ulConditionFlags = 0;
//
//	for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
//	{
//		if (m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[lCount])
//		{
//			m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_ulConditionFlags |= m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[lCount]->m_ulConditionFlags;
//		}
//	}
//
//	return TRUE;
//}


//. 2005. 08. 30 Nonstopdj
BOOL	AgcmCharSkillDlg::CheckSkillExist(const CHAR* szName)
{
	HTREEITEM hCurrent = m_csSkillTree.GetRootItem();

	//. root에서 다음번 Item부터 시작한다.
	//. root는 [HIERARCHY]
	hCurrent = m_csSkillTree.GetNextItem( hCurrent, TVGN_CHILD );

	while (hCurrent != NULL)
	{
		// Get the text for the item. Notice we use TVIF_TEXT because
		// we want to retrieve only the text, but also specify TVIF_HANDLE
		// because we're getting the item by its handle.
		TVITEM item;
		CHAR szText[512];
		item.hItem = hCurrent;
		item.mask = TVIF_TEXT | TVIF_HANDLE;
		item.pszText = szText;
		item.cchTextMax = 512;

		BOOL bWorked = m_csSkillTree.GetItem(&item);

		if (bWorked && !strcmp(item.pszText, szName))
			return TRUE;

		// Try to get the next item
		hCurrent = m_csSkillTree.GetNextItem(hCurrent, TVGN_NEXT);
	}

	return FALSE;
}


void AgcmCharSkillDlg::OnButtonAdd()
{
	// TODO: Add your control notification handler code here
	if ((!AgcmSkillDlg::GetInstance()) || (!AgcmSkillDlg::GetInstance()->GetAgcmItem()))
		return;

	INT32		lCount;
	HTREEITEM	hSelected	= m_csSkillTree.GetSelectedItem();
	if (!hSelected)
		return;

	/*
	if (!m_pstAgcdCharacterTemplate->m_lAnimType2)
	{
		MessageBox("기본 캐릭터는 스킬을 설정 할 수 없습니다.");
		return;	// skip
	}
	*/

	HTREEITEM	hRoot		= m_csSkillTree.GetRootItem();
	if (hSelected == hRoot) // insert skill
	{
		CHAR	*pszTemp = NULL;
		AgcmSkillTemplateListDlg dlg(&pszTemp);
		if ((dlg.DoModal() != IDOK) || (!pszTemp))
			return;

		if(CheckSkillExist(pszTemp)) //존재하는 이름의 스킬이면 추가하지 않는다.
			return;

		for (lCount = 0; lCount < AGPMSKILL_MAX_SKILL_OWN; ++lCount)
		{
			if (!m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[lCount])
			{
				//. 2005 08. 30 Nonstopdj
				//. 같은 이름의 스킬은 등록하지 못한다.

				m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[lCount] = pszTemp;
				InsertSkillTreeItem(pszTemp, hSelected, (DWORD)(lCount), TRUE);
					
				
				break;
			}
		}
	}
	else
	{
		HTREEITEM	hParent		= m_csSkillTree.GetParentItem(hSelected);
		if (!hParent)
			return;

		if (hParent == hRoot) // insert skill(weapon type)
		{
			INT32	lSkillIndex		= m_csSkillTree.GetItemData(hSelected);

			AgcmSkillDlg::GetInstance()->GetAgcmSkill()->CheckVisualInfo(
				m_pstAgcdCharacterTemplate,
				m_pcsAgcdSkillAttachTemplateData,
				lSkillIndex													);

			INT32	lNumAnimType2	=
				AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetAgcmCharacter()->GetAnimType2Num(
					m_pstAgcdCharacterTemplate														);

			for (lCount = 0; lCount < lNumAnimType2; ++lCount)
			{
						//(!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_pastEffect[lCount]) &&
						//(!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex].m_paszAnimName[lCount]) &&
						//(!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount])						)
				if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount])
				{
					/*if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount])
						m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount] =
							(CHAR *)(malloc(AGCDSKILL_INFO_NAME_LENGTH));

					if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount])
						return;*/

					CHAR	**ppAnimType2	=
						AgcmAnimationDlg::GetInstance()->GetAnimType2Name(
							m_pstAgcdCharacterTemplate->m_lAnimType2		);
					CHAR	szTemp[256];
					CHAR	szTemp2[256];
					//sprintf(szTemp, "%s%d", ppAnimType2[lCount], lSkillIndex);
					strcpy(szTemp2, ppAnimType2[lCount] + 2);
					szTemp2[strlen(szTemp2) - 2]	= '\0';
					sprintf(szTemp, "%s%d", szTemp2, lSkillIndex);

					m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount]	=
						(CHAR *)(malloc(strlen(szTemp) + 1));

					strcpy(
						m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lSkillIndex]->m_paszInfo[lCount],
						szTemp																						);

					InsertSkillTreeItem(szTemp, hSelected, lCount, TRUE);

					break;
				}
			}

			//for (lCount = 0; lCount < AGCDITEM_EQUIP_ANIMATION_TYPE_NUM; ++lCount)
			//{
			//	if (	(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_astEffect[lCount].m_ulConditionFlags) &&
			//			(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszAnimName[lCount]) &&
			//			(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lCount])						)
			//	{
			//		if (!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lCount])
			//			m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lCount] =
			//				(CHAR *)(malloc(AGCDSKILL_INFO_NAME_LENGTH));

			//		if (!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lCount])
			//			return;

			//		//sprintf(m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lCount],
			//		//		"%s%d",
			//		//		AgcmSkillDlg::GetInstance()->GetAgcmItem()->GetItemEquipAnimationName(lCount), lSkillIndex		);
			//		//InsertSkillTreeItem(
			//		//	m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lCount],
			//		//	hSelected,
			//		//	lCount,
			//		//	TRUE																						);

			//		break;
			//	}
			//}
		}
	}
}

void AgcmCharSkillDlg::OnButtonSetAnimation()
{
	// TODO: Add your control notification handler code here
	if ((!AgcmSkillDlg::GetInstance()) || (!AgcmSkillDlg::GetInstance()->GetAgcmSkill()))
		return;

	CHAR			*pszFileName;
	AgcmFileListDlg	dlg;

	pszFileName = dlg.OpenFileList(m_szFindAnimPathName1, m_szFindAnimPathName2, NULL);
	if (pszFileName)
	{
		UpdateData(TRUE);

		AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetCharacterTemplate(m_pcsAgcdSkillAttachTemplateData);
		if (!pcsAgpdCharacterTemplate)
			return;

		//@{ 2006/06/07 burumal
		if ( m_lSkillIndex < 0 || m_lWeaponType < 0 )
			return;
		//@}

		if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType])
			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]	= new AgcdCharacterAnimation();

		if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation)
		{
			INT32			lSkillIndex = m_lSkillIndex;
			INT32			lWeaponType = m_lWeaponType;

			AAD_RemoveAllAnimationParams	csRemoveAllAnimationParams;
			csRemoveAllAnimationParams.m_ppSrcAnimation	=
				&m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation;

			AgcmSkillDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);

			if (!csRemoveAllAnimationParams.m_bRemovedAnimation)
			{
				MessageBox("Animation을 설정할 수 없습니다!", "ERROR");
				return;
			}

			m_lSkillIndex = lSkillIndex;
			m_lWeaponType = lWeaponType;
		}

		AAD_AddAnimationParams	csAddAnimationParams;

		csAddAnimationParams.m_ppSrcAnimation	=
			&m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation;

		csAddAnimationParams.m_pszSrcAnimation	= pszFileName;

		AgcmSkillDlg::GetInstance()->AddAnimation(&csAddAnimationParams);

		if (!csAddAnimationParams.m_pDestData)
		{
			MessageBox("Animation을 설정할 수 없습니다!", "ERROR");
			return;
		}

		AAD_ReadRtAnimParams	csReadRtAnimParams;
		csReadRtAnimParams.m_pcsSrcAnimData	= csAddAnimationParams.m_pDestData;

		AgcmSkillDlg::GetInstance()->ReadRtAnim(&csReadRtAnimParams);

		if (!csReadRtAnimParams.m_bRead)
			return;

		//AgcmSkillDlgGetAnimationSet csSet;
		//strcpy(csSet.m_szAnimName, pszFileName);
		//csSet.m_ppcsAgcdAnimation	= &(m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType]);
		//csSet.m_lTID				= pcsAgpdCharacterTemplate->m_lID;
		//csSet.m_lDataIndex			= m_lSkillIndex;
		//csSet.m_lEquipType			= m_lWeaponType;

		//// callback
		//AgcmSkillDlg::GetInstance()->EnumCallbackGetAnimation(&csSet);

		//if(	(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType]) ||
		//	(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType]->m_ppastAnimData[0])	)
		//{
		//	MessageBox("Animation을 설정할 수 없습니다!", "ERROR");
		//	return;
		//}

		//if(!m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType])
		//	m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType] = (CHAR *)(malloc(AGCDSKILL_ANIMATION_NAME_LENGTH));

		//strcpy(m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType], pszFileName);
		m_strAnim = pszFileName;

		UpdateData(FALSE);
	}
}

void AgcmCharSkillDlg::OnButtonRemoveEffect() 
{
	// TODO: Add your control notification handler code here
	if ((m_lSkillIndex == -1) || (m_lWeaponType == -1) || (m_lEffectIndex == -1))
	{
		MessageBox("Remove할 객체가 없습니다!", "WARNNING");
		return;
	}

	RemoveButton();

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->EnableEffectFlagsDlg(FALSE);
}

BOOL AgcmCharSkillDlg::CheckIdentityWeaponType()
{
	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0))
		return FALSE;

	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType])
		return TRUE;	// SKIP

//	for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
	AgcdUseEffectSetList	*pcsCurrent	= m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastEffect[m_lWeaponType]->m_pcsHead;
	while (pcsCurrent)
	{
		//if (m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[lCount])
		//{
		//	if (m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType].m_astData[lCount]->m_ulConditionFlags)
		//		return FALSE;
		//}

		if (pcsCurrent->m_csData.m_ulConditionFlags)
			return FALSE;

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	//if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType])
		//return FALSE;

	if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_paszInfo[m_lWeaponType])
		return FALSE;

	return TRUE;
}

void AgcmCharSkillDlg::OnSelchangeComboWeaponType() 
{
	// TODO: Add your control notification handler code here
	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0))
		return;

	if (	(!AgcmSkillDlg::GetInstance()) ||
			(!AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()) ||
			(!AgcmSkillDlg::GetInstance()->GetAgcmSkill())	||
			(!AgcmEffectDlg::GetInstance())		)
		return;

	m_lWeaponType = m_csWeaponType.GetCurSel();

	if (!CheckIdentityWeaponType())
	{
		MessageBox("해당 타입이 이미 있습니다!", "WARNNING!");
		m_csWeaponType.SetCurSel(m_lPreWeaponType);
		m_lWeaponType = m_lPreWeaponType;
		return;
	}

//	for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
	AgcmSkillVisualInfo		*pcsInfo			= m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex];
	AgcaEffectData2			*pcsAgcaEffectData2	= AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2();
	AgcdUseEffectSetData	*pcsPrevData		= NULL, *pcsCurrentData = NULL;
	for (INT32 lCount = 0; ; ++lCount)
	{
		pcsPrevData	=
			pcsAgcaEffectData2->GetData(
				pcsInfo->m_pastEffect[m_lPreWeaponType],
				lCount									);

		if (!pcsPrevData)
			break;

		pcsCurrentData	=
			pcsAgcaEffectData2->GetData(
				pcsInfo->m_pastEffect[m_lWeaponType],
				lCount										);

		if (!pcsCurrentData)
		{
			pcsCurrentData	=
				pcsAgcaEffectData2->GetData(
					pcsInfo->m_pastEffect[m_lWeaponType],
					lCount,
					TRUE									);

			if (!pcsCurrentData)
				return;
		}

		pcsAgcaEffectData2->CopyData(pcsCurrentData, pcsPrevData);

		pcsInfo->m_pastEffect[m_lWeaponType]->m_ulConditionFlags |=
			pcsCurrentData->m_ulConditionFlags;

		pcsAgcaEffectData2->Delete(
			pcsInfo->m_pastEffect[m_lPreWeaponType],
			pcsPrevData									);
	}

	if (	( m_pcsAgcdSkillAttachTemplateData
				->m_pacsSkillVisualInfo[m_lSkillIndex]
				->m_pacsAnimation[m_lPreWeaponType]		)	&&
			( m_pcsAgcdSkillAttachTemplateData
				->m_pacsSkillVisualInfo[m_lSkillIndex]
				->m_pacsAnimation[m_lPreWeaponType]
				->m_pcsAnimation						)			)
	{
		AAD_AddAnimationParams	csAddAnimationParams;
		csAddAnimationParams.m_ppSrcAnimation	=
			&m_pcsAgcdSkillAttachTemplateData
				->m_pacsSkillVisualInfo[m_lSkillIndex]
				->m_pacsAnimation[m_lWeaponType]
				->m_pcsAnimation;

		csAddAnimationParams.m_pszSrcAnimation	=
			m_pcsAgcdSkillAttachTemplateData
				->m_pacsSkillVisualInfo[m_lSkillIndex]
				->m_pacsAnimation[m_lPreWeaponType]
				->m_pcsAnimation
				->m_pcsHead
				->m_pszRtAnimName;

		AgcmSkillDlg::GetInstance()->AddAnimation(&csAddAnimationParams);

		if (!csAddAnimationParams.m_pDestData)
			return;


		AAD_ReadRtAnimParams	csReadRtAnimParams;
		csReadRtAnimParams.m_pcsSrcAnimData	= csAddAnimationParams.m_pDestData;

		AgcmSkillDlg::GetInstance()->ReadRtAnim(&csReadRtAnimParams);

		if (!csReadRtAnimParams.m_bRead)
			return;
	}

	// 전 인덱스의 애니메이션이 있으면 바뀐 인덱스의 애니메이션으로 복사한다.
	if (	(pcsInfo->m_pacsAnimation[m_lPreWeaponType]) &&
			(pcsInfo->m_pacsAnimation[m_lPreWeaponType]->m_pcsAnimation) &&
			(pcsInfo->m_pacsAnimation[m_lPreWeaponType]->m_pcsAnimation->m_pcsHead) &&
			(pcsInfo->m_pacsAnimation[m_lPreWeaponType]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
	{
		AAD_RemoveAllAnimationParams	csRemoveAllAnimationParams;

		if (pcsInfo->m_pacsAnimation[m_lWeaponType])
		{			
			csRemoveAllAnimationParams.m_ppSrcAnimation	= &pcsInfo->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation;
			AgcmSkillDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);
		}
		else
		{
			pcsInfo->m_pacsAnimation[m_lWeaponType]	=
				new AgcdCharacterAnimation();
		}

		AAD_AddAnimationParams	csAddAnimationParams;
		csAddAnimationParams.m_ppSrcAnimation	= &pcsInfo->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation;
		csAddAnimationParams.m_pszSrcAnimation	= pcsInfo->m_pacsAnimation[m_lPreWeaponType]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName;

		AgcmSkillDlg::GetInstance()->AddAnimation(&csAddAnimationParams);

		AAD_ReadRtAnimParams	csReadRtAnimParams;
		csReadRtAnimParams.m_pcsSrcAnimData	= csAddAnimationParams.m_pDestData;

		AgcmSkillDlg::GetInstance()->ReadRtAnim(&csReadRtAnimParams);

		if (!csReadRtAnimParams.m_bRead)
			return;



		csRemoveAllAnimationParams.m_ppSrcAnimation	= &pcsInfo->m_pacsAnimation[m_lPreWeaponType]->m_pcsAnimation;
		AgcmSkillDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);
	}

	//if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lPreWeaponType])
	//{
	//	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType])
	//	{
	//		AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	=
	//			AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetCharacterTemplate(m_pcsAgcdSkillAttachTemplateData);

	//		AgcmSkillDlgGetAnimationSet csSet;
	//		strcpy(csSet.m_szAnimName, m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lPreWeaponType]);
	//		csSet.m_ppcsAgcdAnimation	= &m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType];
	//		csSet.m_lTID				= pcsAgpdCharacterTemplate->m_lID;
	//		csSet.m_lDataIndex			= m_lSkillIndex;
	//		csSet.m_lEquipType			= m_lWeaponType;

	//		// callback
	//		AgcmSkillDlg::GetInstance()->EnumCallbackGetAnimation(&csSet);

	//		if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType])
	//			return;
	//	}
	//	else
	//	{
	//		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType]->m_ppastAnimData[0] =
	//			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lPreWeaponType]->m_ppastAnimData[0];
	//	}

	//	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lPreWeaponType]->m_ppastAnimData[0] = NULL;

	//	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType])
	//	{
	//		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType] =
	//			(CHAR *)(malloc(AGCDSKILL_ANIMATION_NAME_LENGTH));
	//		if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType])
	//			return;
	//	}

	//	strcpy(	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lWeaponType],
	//			m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lPreWeaponType]	);

	//	free(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lPreWeaponType]);
	//	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszAnimName[m_lPreWeaponType] = NULL;
	//}

	if (pcsInfo->m_paszInfo[m_lPreWeaponType])
	{
		if (pcsInfo->m_paszInfo[m_lWeaponType])
			free(pcsInfo->m_paszInfo[m_lWeaponType]);

		pcsInfo->m_paszInfo[m_lWeaponType]	=
			(CHAR *)(malloc(strlen(pcsInfo->m_paszInfo[m_lPreWeaponType]) + 1));

		strcpy(
			pcsInfo->m_paszInfo[m_lWeaponType],
			pcsInfo->m_paszInfo[m_lPreWeaponType]	);

		delete[] pcsInfo->m_paszInfo[m_lPreWeaponType];
		//free(pcsInfo->m_paszInfo[m_lPreWeaponType]);
		pcsInfo->m_paszInfo[m_lPreWeaponType]	= NULL;
	}

	//if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lPreWeaponType])
	//{
	//	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lWeaponType])
	//	{
	//		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lWeaponType] = (CHAR *)(malloc(AGCDSKILL_INFO_NAME_LENGTH));
	//		if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lWeaponType])
	//			return;
	//	}

	//	free(m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lPreWeaponType]);
	//	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lPreWeaponType] = NULL;

	//	sprintf(	m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_paszInfoName[m_lWeaponType],
	//				"%s%d",
	//				AgcmSkillDlg::GetInstance()->GetAgcmItem()->GetItemEquipAnimationName(m_lWeaponType), m_lSkillIndex		);
	//}

	AgcmEffectDlg::GetInstance()->EnableEffectFlagsDlg(FALSE);

	HTREEITEM	hItem = FindSkillTreeItem(m_lSkillIndex, m_lPreWeaponType);
	if (!hItem)
	{
		MessageBox("ERROR!", "ERROR!");
		return;
	}

	m_csSkillTree.SetItemData(hItem, (DWORD)(m_lWeaponType));
	m_csSkillTree.SetItemText(hItem, pcsInfo->m_paszInfo[m_lWeaponType]);

	m_lPreWeaponType = m_lWeaponType;

	UpdateSkillControl(FALSE);
}

void AgcmCharSkillDlg::OnButtonAnimPlay() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmSkillDlg::GetInstance())
		return;

	UpdateData(TRUE);

	if (	(m_lSkillIndex < 0)	||
			(m_lWeaponType < 0)		)
		return;

	/*if (m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pastAnim[m_lWeaponType])
	{
		AgcmSkillDlgStartAnimationSet csSet;
		csSet.m_pstAgcdAnimation	= m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType];
		csSet.m_lSkillUsableIndex	= m_lSkillIndex;
		csSet.m_lEquipType			= m_lWeaponType;

		ApBase	*pcsSkillTemplate	= (ApBase *)(AgcmSkillDlg::GetInstance()->GetAgpmSkill()->GetSkillTemplate(m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[m_lSkillIndex]));
		if (!pcsSkillTemplate)
			return;

		csSet.m_lSkillID			= pcsSkillTemplate->m_lID;

		AgcmSkillDlg::GetInstance()->EnumCallbackStartSkillAnimation(&csSet);
	}*/
	AgcdCharacterAnimation *pcsAnim	= m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType];
	if (	(pcsAnim) &&
			(pcsAnim->m_pcsAnimation) &&
			(pcsAnim->m_pcsAnimation->m_pcsHead)	)
	{
		ApBase	*pcsSkillTemplate	= (ApBase *)(AgcmSkillDlg::GetInstance()->GetAgpmSkill()->GetSkillTemplate(m_pcsAgpdSkillTemplateAttachData->m_aszUsableSkillTName[m_lSkillIndex]));
		if (!pcsSkillTemplate)
			return;

		AAD_StartAnimationParams	csStartAnimationParams;
		csStartAnimationParams.m_lAnimType		= AGCMCHAR_ANIM_TYPE_SKILL;
		csStartAnimationParams.m_lAnimType2		= m_lWeaponType;
		csStartAnimationParams.m_pcsAnimData	= pcsAnim->m_pcsAnimation->m_pcsHead;
		csStartAnimationParams.m_lSkillIndex	= m_lSkillIndex;

		//. 2005. 9. 1. Nonstopdj
		//. 스킬다이얼로그에서 수행되는Start Animation CB는 StartPureAnimCB가 아니라 
		//. StartAnimCB이다. (Effect와 Animation을 동시에 Start하는..)

		//AgcmAnimationDlg::GetInstance()->StartAnimation(&csStartAnimationParams);
		AgcmSkillDlg::GetInstance()->StartAnim(&csStartAnimationParams);
	}
	else
	{
		AgcmSkillDlg::GetInstance()->EnumCallbackStartSkillEffect(m_lSkillIndex, m_lWeaponType);
	}
}

void AgcmCharSkillDlg::OnButtonAnimStop() 
{
}

//
//BOOL AgcmCharSkillDlg::UpdateScriptDlg()
//{
//	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0) || (m_lEffectIndex < 0))
//	{
//		MessageBox("적용할 객체가 없습니다!", "ERROR!");
//		return FALSE;
//	}
//
//	if (!AgcmSkillDlg::GetInstance())
//		return FALSE;
//
//	if (!AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg())
//		return FALSE;
//
//	AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg()->UpdateEffectScriptDlg(
//		&m_pcsAgcdSkillAttachTemplateData->m_astSkillVisualInfo[m_lSkillIndex].m_astEffect[m_lWeaponType],
//		m_lEffectIndex																						);
//
//	return TRUE;
//}

void AgcmCharSkillDlg::OnButtonSetAnimProperty() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmAnimationDlg::GetInstance())
		return;

	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0))
		return;

	/*if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType])
		return;

	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType]->m_ppastAnimData[0])
		return;

	AgcdCharacterAnimationAttachedData	*pcsData	= (AgcdCharacterAnimationAttachedData *)(
	AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetAgcmCharacter()->GetAnimationAttachedData(
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex].m_pastAnim[m_lWeaponType],
		AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY															)	);

	AgcmAnimationDlg::GetInstance()->OpenAnimProperty(
		pcsData,
		0																								);*/

	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType])
		return;

	if (!m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pstAnimFlags)
	{
		AgcmSkillDlg::GetInstance()->GetAgcmCharacter()->GetAgcaAnimation2()->AddFlags(
			&m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pstAnimFlags	);
	}

	ACA_AttachedData* pcsACA = NULL;
	AgcaAnimation2 *pcsAgcaAnimation2 = AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule()->GetAgcaAnimation2();
	if( pcsAgcaAnimation2 ) {
		AgcdAnimData2* pcsCurAnimData = m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pcsAnimation->m_pcsHead;
		pcsACA = (ACA_AttachedData *)(pcsAgcaAnimation2->GetAttachedData(	AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,	pcsCurAnimData ));
	}

	AgcmAnimationDlg::GetInstance()->OpenAnimFlags(
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_pacsAnimation[m_lWeaponType]->m_pstAnimFlags, pcsACA );
}

void AgcmCharSkillDlg::OnButtonOpenCustDataList() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	AgcmEffectDlg::GetInstance()->OpenEffectHelp();
}

//@{ 2007/01/03 burumal
void AgcmCharSkillDlg::OnBnClickedCheckHideCastingEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !AgcmSkillDlg::GetInstance()->GetAgcmEventEffect() )
		return;


	if ( AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->CastingEffectDisabled() )
		AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->DisableCastingEffect(FALSE);
	else
		AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->DisableCastingEffect(TRUE);
}
//@}
void AgcmCharSkillDlg::OnButtonAlwaysShow()
{		
	if ((m_lSkillIndex < 0) || (m_lWeaponType < 0))
		return;
	UpdateData( TRUE );

	//m_bAlwaysShow = !m_bAlwaysShow;
	//UpdateData( FALSE );

	if( m_bAlwaysShow )
	{
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_eShowOption = AgcmSkillVisualInfo::ALWAYS_SHOW;
	}
	else
	{
		m_pcsAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[m_lSkillIndex]->m_eShowOption = AgcmSkillVisualInfo::DEFEND_ON_OPTION;
	}
}
