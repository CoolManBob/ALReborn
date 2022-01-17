// AgcmEventSpawnSpawnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmEventSpawnDlg.h"
#include "AgcmEventSpawnSpawnDlg.h"
#include "AgpmAI2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSpawnSpawnDlg dialog


AgcmEventSpawnSpawnDlg::AgcmEventSpawnSpawnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEventSpawnSpawnDlg::IDD, pParent)
{
	
	m_fPositionX		= 0.0f;
	m_fPositionY		= 0.0f;
	m_fPositionZ		= 0.0f;
	m_fRadius			= 0.0f;
	m_lMaxChar			= 0;
	m_lSpawnRate		= 0;
	m_lGroupMin			= 0;
	m_lGroupMax			= 0;
	m_lGroupInterval	= 0;
	m_strGroupName		= _T("");
	m_strSpawnName		= _T("");

	m_pstSpawnChar		=	NULL;
	m_pstEvent			=	NULL;
	m_pstSpawn			=	NULL;
	
}


void AgcmEventSpawnSpawnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEventSpawnSpawnDlg)
	DDX_Control(pDX, IDC_SelectedDropItemTemplateList, m_cSelectedDropItemTemplateList);
	DDX_Control(pDX, IDC_DropItemTemplateCombo, m_cDropItemCombo);
	DDX_Control(pDX, IDC_AddDropItemNameEdit, m_cAddDropItemNameEdit);
	DDX_Control(pDX, IDC_SPAWN_GROUP_TREE, m_csSpawnTree);
	DDX_Control(pDX, IDC_SPAWN_CHARACTER, m_csCharacter);
	DDX_Control(pDX, IDC_SPAWN_CHARACTERS, m_csCharacterList);
	DDX_Text(pDX, IDC_SPAWN_X, m_fPositionX);
	DDX_Text(pDX, IDC_SPAWN_Y, m_fPositionY);
	DDX_Text(pDX, IDC_SPAWN_Z, m_fPositionZ);
	DDX_Text(pDX, IDC_SPAWN_RADIUS, m_fRadius);
	DDX_Text(pDX, IDC_SPAWN_MAX, m_lMaxChar);
	DDX_Text(pDX, IDC_SPAWN_RATE, m_lSpawnRate);
	DDX_Text(pDX, IDC_SPAWN_GROUP_MIN, m_lGroupMin);
	DDX_Text(pDX, IDC_SPAWN_GROUP_MAX, m_lGroupMax);
	DDX_Text(pDX, IDC_SPAWN_GROUP_MAX2, m_lGroupInterval);
	DDX_Text(pDX, IDC_SPAWN_GROUP_NAME, m_strGroupName);
	DDX_Text(pDX, IDC_SPAWN_NAME, m_strSpawnName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEventSpawnSpawnDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEventSpawnSpawnDlg)
	ON_BN_CLICKED(IDC_SPAWN_ADD_CHAR, OnSpawnAddChar)
	ON_BN_CLICKED(IDC_SPAWN_DEL_CHAR, OnSpawnDelChar)
	ON_BN_CLICKED(IDC_SPAWN_UPD_CHAR, OnSpawnUpdChar)
	ON_BN_CLICKED(IDC_SPAWN_GROUP_ADD, OnSpawnGroupAdd)
	ON_BN_CLICKED(IDC_SPAWN_GROUP_DELETE, OnSpawnGroupDelete)
	ON_BN_CLICKED(IDC_SPAWN_GROUP_UPDATE, OnSpawnGroupUpdate)
	ON_BN_CLICKED(IDC_SPAWN_SET_GROUP, OnSpawnSetGroup)
	ON_BN_CLICKED(IDC_SPAWN_EDIT_CONDITION, OnSpawnEditCondition)
	ON_BN_CLICKED(IDC_AddDropItemTemplateButton, OnAddDropItemTemplateButton)
	ON_BN_CLICKED(IDC_UpdateDropItemTemplateButton, OnUpdateDropItemTemplateButton)
	ON_BN_CLICKED(IDC_RemoveDropItemTemplateButton, OnRemoveDropItemTemplateButton)
	ON_BN_CLICKED(IDC_SelectDropItemTemplateButton, OnSelectDropItemTemplateButton)
	ON_BN_CLICKED(IDC_DeleteSelectedDropItemTemplateButton, OnDeleteSelectedDropItemTemplateButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SPAWN_CHARACTERS, OnItemchangedSpawnCharacters)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SPAWN_GROUP_TREE, OnSelchangedSpawnGroupTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSpawnSpawnDlg message handlers

BOOL AgcmEventSpawnSpawnDlg::Create() 
{
	BOOL	bRet = CDialog::Create(	IDD , NULL );

	return bRet;
}

BOOL AgcmEventSpawnSpawnDlg::SetModule(AgcmEventSpawnDlg *pcsAgcmEventSpawnDlg, AgpmCharacter *pcsAgpmCharacter, AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmDropItem *pcsAgpmDropItem, AgpmAI2 *pcsAgpmAI2 ) 
{
	m_pcsAgcmEventSpawnDlg	= pcsAgcmEventSpawnDlg;
	m_pcsAgpmCharacter		= pcsAgpmCharacter;
	m_pcsAgpmItem			= pcsAgpmItem;
	m_pcsAgpmSkill			= pcsAgpmSkill;
	m_pcsAgpmDropItem		= pcsAgpmDropItem;
	m_pcsAgpmAI2			= pcsAgpmAI2;

	if( !m_pcsAgcmEventSpawnDlg	|| !m_pcsAgpmCharacter	|| !m_pcsAgpmItem || 
		!m_pcsAgpmSkill			|| !m_pcsAgpmDropItem	|| !m_pcsAgpmAI2	)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmEventSpawnSpawnDlg::SetAI2TemplateCombo()
{
	return TRUE;
}

VOID AgcmEventSpawnSpawnDlg::Init()
{
}

BOOL AgcmEventSpawnSpawnDlg::InitData(ApdEvent *pstEvent)
{
	m_pstEvent	= pstEvent;
	m_pstSpawn	= static_cast< AgpdSpawn * >( pstEvent->m_pvData );

	return TRUE;
}

BOOL AgcmEventSpawnSpawnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	INT32						lIndex		= 0;
	INT32						lListIndex	= 0;
	
	HTREEITEM					hItem;
	HTREEITEM					hItemTemp;

	AgpdSpawnGroup *			pstGroup			= NULL;
	ApdEvent *					pstEventTemp		= NULL;
	AgpdSpawn *					pstSpawn			= NULL;
	AgpdCharacterTemplate *		pcsTemplate			= NULL;
	AgpdDropItemTemplate *		pcsDropItemTemplate	= NULL;

	for (lIndex = 0, pcsTemplate = m_pcsAgcmEventSpawnDlg->m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_pcsAgcmEventSpawnDlg->m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		lListIndex = m_csCharacter.AddString(pcsTemplate->m_szTName);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csCharacter.SetItemDataPtr(lListIndex, pcsTemplate);
	}

	
	m_csCharacterList.InsertColumn(1, AGCMEVENT_SPAWN_CHARACTERLIST_COLUMN1, LVCFMT_LEFT, 200);
	m_csCharacterList.InsertColumn(2, AGCMEVENT_SPAWN_CHARACTERLIST_COLUMN2, LVCFMT_LEFT, 60);

	m_csSpawnTree.DeleteAllItems();
	for (lIndex = 0, pstGroup = m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		hItem = m_csSpawnTree.InsertItem(pstGroup->m_szName);
		if (!hItem)
			return FALSE;

		m_csSpawnTree.SetItemData(hItem, (DWORD) pstGroup);

		pstEventTemp = pstGroup->m_pstChild;

		while (pstEventTemp)
		{
			pstSpawn = (AgpdSpawn *) pstEventTemp->m_pvData;
			if( !pstSpawn	||	IsBadReadPtr( pstSpawn , sizeof(AgpdSpawn) ) )
			{
				break;
			}

			hItemTemp = m_csSpawnTree.InsertItem(pstSpawn->m_szName[0] ? pstSpawn->m_szName : "No Name", hItem);

			m_csSpawnTree.SetItemData(hItemTemp, (DWORD) pstEventTemp);

			pstEventTemp = pstSpawn->m_pstNext;
			if( pstEventTemp	== NULL )	break;

			if( IsBadReadPtr( pstEventTemp , sizeof(ApdEvent) ) ) 
			{
				return FALSE;
			}
		}
	}

	//DropItem Combo에 Data를 넣자.
	AgpaDropItemTemplate		*pcsAgpaDropItemTemplate;
	pcsAgpaDropItemTemplate = &m_pcsAgcmEventSpawnDlg->m_pcsAgpmDropItem->m_aDropItemTemplate;

	for (lIndex = 0, pcsDropItemTemplate = pcsAgpaDropItemTemplate->GetTemplateSequence(&lIndex); pcsDropItemTemplate; pcsDropItemTemplate = pcsAgpaDropItemTemplate->GetTemplateSequence(&lIndex))
	{
		m_cDropItemCombo.AddString( pcsDropItemTemplate->m_strTemplateName );
	}

	//읽어낸 AI2 정보를 디스플레이한다.
	SetAI2TemplateCombo();

	return InitSpawnData();
}

BOOL AgcmEventSpawnSpawnDlg::InitSpawnData()
{
	INT32					lIndex = 0;
	INT32					lListIndex;
	AgpdCharacterTemplate *	pcsTemplate;
	CHAR					szTemp[100];
	AuPOS *					pstPos;

	m_csCharacterList.DeleteAllItems();

	for (lIndex = 0; lIndex < m_pstSpawn->m_stConfig.m_lSpawnChar; ++lIndex)
	{
		pcsTemplate = m_pcsAgcmEventSpawnDlg->m_pcsAgpmCharacter->GetCharacterTemplate(m_pstSpawn->m_stConfig.m_astChar[lIndex].m_lCTID);
		if (!pcsTemplate)
			continue;

		lListIndex = m_csCharacterList.InsertItem(lIndex, pcsTemplate->m_szTName);
		if (lListIndex == -1)
			return FALSE;

		sprintf(szTemp, "%d", m_pstSpawn->m_stConfig.m_astChar[lIndex].m_lSpawnRate);
		m_csCharacterList.SetItemText(lListIndex, 1, szTemp);

		m_csCharacterList.SetItemData(lListIndex, lIndex);

		//첫번째 몬스터의 드랍 템플릿을 보여준다.
		if( lIndex == 0 )
		{
			char			*pstrItemName;

			for( int lCounter=0; lCounter<AGPDSPAWN_MAX_ITEM_NUM; lCounter++ )
			{
				pstrItemName = m_pstSpawn->m_stConfig.m_astChar[lIndex].m_astItem[lCounter].m_strDropItemTemplate;

				if( strlen( pstrItemName ) )
				{
					m_cSelectedDropItemTemplateList.AddString( pstrItemName );
				}
			}						
		}
	}

	if (m_pstEvent->m_pcsSource)
	{
		pstPos = m_pcsAgcmEventSpawnDlg->m_pcsApmEventManager->GetBasePos(m_pstEvent->m_pcsSource, NULL);
		m_fPositionX = pstPos->x;
		m_fPositionY = pstPos->y;
		m_fPositionZ = pstPos->z;
	}
	else
	{
		m_fPositionX = 0;
		m_fPositionY = 0;
		m_fPositionZ = 0;
	}

	if (m_pstEvent->m_pstCondition && m_pstEvent->m_pstCondition->m_pstArea)
	{
		if (m_pstEvent->m_pstCondition->m_pstArea->m_eType == APDEVENT_AREA_SPHERE)
			m_fRadius = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius / 100.0f;
		else if (m_pstEvent->m_pstCondition->m_pstArea->m_eType == APDEVENT_AREA_SPHERE)
			m_fRadius = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius / 100.0f;
	}

	m_strSpawnName = m_pstSpawn->m_szName;
	m_lMaxChar = m_pstSpawn->m_stConfig.m_lMaxChar;

	m_csCharacter.SetCurSel(-1);
	m_lSpawnRate = 0;

	UpdateData(FALSE);

	m_pstSpawnChar = NULL;

	return TRUE;
}

void AgcmEventSpawnSpawnDlg::OnSpawnAddChar()
{
	INT32					lListIndex;
	INT32					lIndex;
	INT32					lAI2Index;
	INT32					lAI2TID		=	0;
	AgpdCharacterTemplate *	pcsTemplate;
	CHAR					szTemp[100];

	lListIndex = m_csCharacter.GetCurSel();
	if (lListIndex == CB_ERR)
		return;

	pcsTemplate = (AgpdCharacterTemplate *) m_csCharacter.GetItemDataPtr(lListIndex);
	if (!pcsTemplate)
		return;

	UpdateData();

	//lAI2Index = m_cAI2TemplateCombo.GetCurSel();
	//if( lAI2Index == -1 )
	//{
	//	MessageBox( "AI를 설정해주십시요." );
	//	return;
	//}
	//else
	//{
	//	lAI2TID = m_cAI2TemplateCombo.GetItemData( lAI2Index );
	//}

	lIndex = m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->AddSpawnChar(m_pstSpawn, pcsTemplate->m_lID, lAI2TID, m_lSpawnRate);
	if (lIndex == -1)
		return;

	lListIndex = m_csCharacterList.InsertItem(lIndex, pcsTemplate->m_szTName);
	if (lListIndex == -1)
		return;

	sprintf(szTemp, "%d", m_lSpawnRate);
	m_csCharacterList.SetItemText(lListIndex, 1, szTemp);

	m_csCharacterList.SetItemData(lListIndex, lIndex);
}

void AgcmEventSpawnSpawnDlg::OnSpawnDelChar() 
{
	INT32		lListIndex;
	INT32		lIndex;
	POSITION	pos = m_csCharacterList.GetFirstSelectedItemPosition();

	while (pos)
	{
		lListIndex = m_csCharacterList.GetNextSelectedItem(pos);
		if (lListIndex < 0)
			continue;

		lIndex = m_csCharacterList.GetItemData(lListIndex);
		if (lIndex < 0)
			continue;

		if (!m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->RemoveSpawnChar(m_pstSpawn, lIndex))
			return;

		m_csCharacterList.DeleteItem(lIndex);
		
		//Drop Item 초기화
		m_cDropItemCombo.SetCurSel( CB_ERR );
		m_cAddDropItemNameEdit.SetWindowText( "" );
		m_cSelectedDropItemTemplateList.ResetContent();
	}
}

void AgcmEventSpawnSpawnDlg::OnSpawnUpdChar() 
{
	INT32					lListIndex;
	INT32					lIndex;
	AgpdCharacterTemplate *	pcsTemplate;
	POSITION				pos = m_csCharacterList.GetFirstSelectedItemPosition();
	CHAR					szTemp[100];
	AgpdSpawnConfigChar *	pstSpawnChar;

	UpdateData();

	if (pos)
	{
		lListIndex = m_csCharacter.GetCurSel();
		pcsTemplate = (AgpdCharacterTemplate *) m_csCharacter.GetItemDataPtr(lListIndex);

		lListIndex = m_csCharacterList.GetNextSelectedItem(pos);
		lIndex = m_csCharacterList.GetItemData(lListIndex);

		pstSpawnChar = m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->GetSpawnChar(m_pstSpawn, lIndex);

		pstSpawnChar->m_lCTID = pcsTemplate->m_lID;
		pstSpawnChar->m_lSpawnRate = m_lSpawnRate;
		pstSpawnChar->m_lAITID = 0;

		if (m_lSpawnRate > 100)
			m_lSpawnRate = 100;

		sprintf(szTemp, "%d", m_lSpawnRate);
		m_csCharacterList.SetItemText(lListIndex, 0, pcsTemplate->m_szTName);
		m_csCharacterList.SetItemText(lListIndex, 1, szTemp);
	}
}

void AgcmEventSpawnSpawnDlg::OnItemchangedSpawnCharacters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	INT32					lListIndex;
	INT32					lIndex;
	AgpdCharacterTemplate *	pcsTemplate;
	POSITION				pos = m_csCharacterList.GetFirstSelectedItemPosition();

	UpdateData();

	if (pos)
	{
		AgpdAI2Template			*pcsAgpdAI2Template;
		char					*pstrItemName;

		lListIndex = m_csCharacterList.GetNextSelectedItem(pos);
		lIndex = m_csCharacterList.GetItemData(lListIndex);

		pcsTemplate = m_pcsAgcmEventSpawnDlg->m_pcsAgpmCharacter->GetCharacterTemplate(m_pstSpawn->m_stConfig.m_astChar[lIndex].m_lCTID);
		if (!pcsTemplate)
			return;

		m_csCharacter.SetCurSel(m_csCharacter.FindString(-1, pcsTemplate->m_szTName));
		m_lSpawnRate = m_pstSpawn->m_stConfig.m_astChar[lIndex].m_lSpawnRate;

		m_lSpawnCharIndex = lIndex;
		m_pstSpawnChar = &m_pstSpawn->m_stConfig.m_astChar[ lIndex ];

		//현재 선택된 캐릭터가 떨구는 아이템 리스트를본다.
		m_cSelectedDropItemTemplateList.ResetContent();

		for( int lCounter=0; lCounter<AGPDSPAWN_MAX_ITEM_NUM; lCounter++ )
		{
			pstrItemName = m_pstSpawnChar->m_astItem[lCounter].m_strDropItemTemplate;

			if( strlen( pstrItemName ) )
			{
				m_cSelectedDropItemTemplateList.AddString( pstrItemName );
			}
		}

		UpdateData(FALSE);
	}
}

void AgcmEventSpawnSpawnDlg::OnSpawnGroupAdd() 
{
	HTREEITEM			hItem;
	AgpdSpawnGroup	*	pstGroup	=	NULL;

	UpdateData();

	pstGroup = m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->AddSpawnGroup((LPSTR) (LPCTSTR) m_strGroupName);

	if (!pstGroup)
	{
		CString	str;
		str.Format( "스폰 그룹 갯수제한 ?.. ( 현 %d 개/총 %d 개 )" , 
			m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->GetGroupCount(),
			m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->GetMaxGroupCount() );

		MessageBox( str );
		return;
	}

	pstGroup->m_lMinChar = m_lGroupMin;
	pstGroup->m_lMaxChar = m_lGroupMax;
	pstGroup->m_lInterval = m_lGroupInterval;

	hItem = m_csSpawnTree.InsertItem(pstGroup->m_szName);
	if (!hItem)
		return;

	m_csSpawnTree.SetItemData(hItem, (DWORD) pstGroup);
}

void AgcmEventSpawnSpawnDlg::OnSpawnGroupDelete() 
{
	HTREEITEM			hItem = m_csSpawnTree.GetSelectedItem();
	AgpdSpawnGroup *	pstGroup;

	if (!hItem)
		return;

	if (m_csSpawnTree.GetParentItem(hItem))
		return;

	pstGroup = (AgpdSpawnGroup *) m_csSpawnTree.GetItemData(hItem);

	m_csSpawnTree.DeleteItem(hItem);

	m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->RemoveGroup(pstGroup);
}

void AgcmEventSpawnSpawnDlg::OnSpawnGroupUpdate() 
{
	HTREEITEM			hItem = m_csSpawnTree.GetSelectedItem();
	AgpdSpawnGroup *	pstGroup;

	UpdateData();

	if (!hItem)
		return;

	if (m_csSpawnTree.GetParentItem(hItem))
		return;

	pstGroup = (AgpdSpawnGroup *) m_csSpawnTree.GetItemData(hItem);

	if (strcmp(pstGroup->m_szName, m_strGroupName) && !m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->ChangeGroupName(pstGroup, (LPSTR) (LPCTSTR) m_strGroupName))
		return;

	pstGroup->m_lMinChar = m_lGroupMin;
	pstGroup->m_lMaxChar = m_lGroupMax;
	pstGroup->m_lInterval = m_lGroupInterval;

	m_csSpawnTree.SetItemText(hItem, pstGroup->m_szName);
}

void AgcmEventSpawnSpawnDlg::OnSelchangedSpawnGroupTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM			hItem = m_csSpawnTree.GetSelectedItem();
	AgpdSpawnGroup *	pstGroup;

	if (!hItem)
		return;

	if (!m_csSpawnTree.GetParentItem(hItem))
	{
		pstGroup = (AgpdSpawnGroup *) m_csSpawnTree.GetItemData(hItem);

		m_strGroupName = pstGroup->m_szName;
		m_lGroupMin = pstGroup->m_lMinChar;
		m_lGroupMax = pstGroup->m_lMaxChar;
		m_lGroupInterval = pstGroup->m_lInterval;

		UpdateData(FALSE);
	}
	else
	{
		m_pstEvent = (ApdEvent *) m_csSpawnTree.GetItemData(hItem);
		m_pstSpawn = (AgpdSpawn *) m_pstEvent->m_pvData;

		InitSpawnData();
	}

	*pResult = 0;
}

void AgcmEventSpawnSpawnDlg::OnSpawnSetGroup() 
{
	HTREEITEM			hItem = m_csSpawnTree.GetSelectedItem();
	AgpdSpawnGroup *	pstGroup;

	UpdateData();

	strncpy(m_pstSpawn->m_szName, (LPSTR) (LPCTSTR) m_strSpawnName, AGPDSPAWN_MAX_CHAR_NUM);
	m_pstSpawn->m_szName[AGPDSPAWN_MAX_CHAR_NUM - 1] = 0;
	m_pstSpawn->m_stConfig.m_lMaxChar = m_lMaxChar;

	if (!hItem)
		return;

	if (m_csSpawnTree.GetParentItem(hItem))
		return;

	pstGroup = (AgpdSpawnGroup *) m_csSpawnTree.GetItemData(hItem);

	m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->AddSpawnToGroup(pstGroup, m_pstEvent);

	OnInitDialog();
}

void AgcmEventSpawnSpawnDlg::OnOK() 
{
	UpdateData();

	strncpy(m_pstSpawn->m_szName, (LPSTR) (LPCTSTR) m_strSpawnName, AGPDSPAWN_MAX_CHAR_NUM);
	m_pstSpawn->m_szName[AGPDSPAWN_MAX_CHAR_NUM - 1] = 0;
	m_pstSpawn->m_stConfig.m_lMaxChar = m_lMaxChar;
	
	CDialog::OnOK();
}

void AgcmEventSpawnSpawnDlg::OnSpawnEditCondition() 
{
	if (m_pcsAgcmEventSpawnDlg->m_pcsApmEventManagerDlg->OpenCondition(m_pstEvent))
	{
		m_fRadius = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius / 100.0f;
		UpdateData(FALSE);
	}
}

//void AgcmEventSpawnSpawnDlg::OnSpawnEvent() 
//{
//	if (!m_pstSpawnChar)
//	{
//		AfxMessageBox("Select character first !!!");
//		return;
//	}
//
//	m_pcsAgcmEventSpawnDlg->m_pcsApmEventManagerDlg->Open(NULL, &m_pstSpawnChar->m_stEvent);
//}

void AgcmEventSpawnSpawnDlg::OnAddDropItemTemplateButton() 
{
	// TODO: Add your control notification handler code here

	//Excel에서 받아오므로 Add는 필요없음.
	return;

	AgpdDropItemTemplate		*pcsAgpdDropItemTemplate;

	pcsAgpdDropItemTemplate = new AgpdDropItemTemplate;

	if( m_cAddDropItemNameEdit.GetWindowText( pcsAgpdDropItemTemplate->m_strTemplateName, sizeof(pcsAgpdDropItemTemplate->m_strTemplateName) ) )
	{
		if( m_pcsAgpmDropItem->m_aDropItemTemplate.AddDropItemTemplate( pcsAgpdDropItemTemplate ) )
		{
			m_cDropItemCombo.AddString( pcsAgpdDropItemTemplate->m_strTemplateName );

//			m_csDropItemDlg.SetDlgData( m_pcsAgpmItem, pcsAgpdDropItemTemplate );
//			m_csDropItemDlg.DoModal();	

			m_csDropTemplateDlg.SetDlgData( m_pcsAgpmItem, m_pcsAgpmDropItem, pcsAgpdDropItemTemplate );
			m_csDropTemplateDlg.DoModal();
		}
		else
		{
			MessageBox( "이미 존재하는 템플릿 이름입니다." );
		}
	}
	else
	{
		//Template이름을 넣으라고 알린다.
		MessageBox( "템플릿 이름을 넣어주세요~" );
	}
}

void AgcmEventSpawnSpawnDlg::OnUpdateDropItemTemplateButton() 
{
	// TODO: Add your control notification handler code here
	INT32				lSelectCur;

	lSelectCur = m_cDropItemCombo.GetCurSel();

	if( lSelectCur != CB_ERR )
	{
		char		strTemplateName[80];

		if( m_cDropItemCombo.GetLBText( lSelectCur, strTemplateName ) != CB_ERR )
		{
			AgpdDropItemTemplate		*pcsAgpdDropItemTemplate;

			pcsAgpdDropItemTemplate = m_pcsAgpmDropItem->m_aDropItemTemplate.GetDropItemTemplate( strTemplateName );

			if( pcsAgpdDropItemTemplate != NULL )
			{
//				m_csDropItemDlg.SetDlgData( m_pcsAgpmItem, pcsAgpdDropItemTemplate );
//				m_csDropItemDlg.DoModal();	

				m_csDropTemplateDlg.SetDlgData( m_pcsAgpmItem, m_pcsAgpmDropItem, pcsAgpdDropItemTemplate );
				m_csDropTemplateDlg.DoModal();
			}
		}
	}
}

void AgcmEventSpawnSpawnDlg::OnRemoveDropItemTemplateButton() 
{
	// TODO: Add your control notification handler code here
	INT32				lResult;

	lResult = MessageBox( "정말 삭제하시겠습니까?", "확인", MB_YESNO );

	if( lResult == IDYES )
	{
		INT32				lSelectCur;

		lSelectCur = m_cDropItemCombo.GetCurSel();

		if( lSelectCur != CB_ERR )
		{
			char		strTemplateName[80];

			if( m_cDropItemCombo.GetLBText( lSelectCur, strTemplateName ) != CB_ERR )
			{
				if( m_cDropItemCombo.DeleteString( lSelectCur ) != CB_ERR )
				{
					m_pcsAgpmDropItem->m_aDropItemTemplate.RemoveDropItemTemplate( strTemplateName );
				}
			}
		}
	}
}

void AgcmEventSpawnSpawnDlg::OnSelectDropItemTemplateButton() 
{
	// TODO: Add your control notification handler code here
	INT32			lSelectedCur;
	INT32			lSelectedCharacter;

	lSelectedCur = m_cDropItemCombo.GetCurSel();
	lSelectedCharacter = m_csCharacterList.GetNextItem( -1, LVNI_SELECTED );

	if( lSelectedCharacter != -1 )
	{
		if( lSelectedCur != CB_ERR )
		{
			char			strBuffer[80];

			m_cDropItemCombo.GetLBText( lSelectedCur, strBuffer );

			//존재하지않는 템플릿입니다.
			if( m_pcsAgpmDropItem->m_aDropItemTemplate.GetDropItemTemplate( strBuffer ) )
			{
				//이미 있는놈인지 검사한다.
				if( m_cSelectedDropItemTemplateList.FindStringExact( -1, strBuffer ) == LB_ERR )
				{
					m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->AddSpawnItem( m_pstSpawnChar, strBuffer );
					m_cSelectedDropItemTemplateList.AddString( strBuffer );
				}
				else
				{
					MessageBox( "이미 존재하는 템플릿입니다." );
				}
			}
		}
		else
		{
			//드랍 아이템템플릿을 설정하라고 에러메시지를 낸다.
			MessageBox( "DropItemTemplate를 설정해주십시요~" );
		}
	}
	else
	{
		//몹을 찍으라고 알려준다.
		MessageBox( "드랍아이템을 템플릿과 연결한 몬스터를 설정해주십시요~" );
	}
}

void AgcmEventSpawnSpawnDlg::OnDeleteSelectedDropItemTemplateButton() 
{
	// TODO: Add your control notification handler code here
	INT32				lResult;

	lResult = MessageBox( "정말 삭제하시겠습니까?", "확인", MB_YESNO );

	if( lResult == IDYES )
	{
		INT32			lSelectedCur;

		lSelectedCur = m_cSelectedDropItemTemplateList.GetCurSel();

		if( lSelectedCur != LB_ERR )
		{
			m_cSelectedDropItemTemplateList.DeleteString( lSelectedCur );
			m_pcsAgcmEventSpawnDlg->m_pcsAgpmEventSpawn->RemoveSpawnItem( m_pstSpawnChar, lSelectedCur );
		}
	}
}