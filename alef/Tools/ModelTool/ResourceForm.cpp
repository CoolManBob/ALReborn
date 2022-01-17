// ResourceForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "ResourceForm.h"
#include "CharRideDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceForm

IMPLEMENT_DYNCREATE(CResourceForm, CFormView)

CResourceForm::CResourceForm()
	: CFormView(CResourceForm::IDD)
{
	//{{AFX_DATA_INIT(CResourceForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CResourceForm::CResourceForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor)
	: CFormView(CResourceForm::IDD)
{
	InitializeMember();
	m_hBackBrush = hBackColor;

	Create(NULL, NULL, ( WS_CHILD | WS_VISIBLE | WS_SIZEBOX ), stInitRect, pcsParent, NULL, NULL);
	CreateMember();
}

CResourceForm::~CResourceForm()
{
}

void CResourceForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResourceForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResourceForm, CFormView)
	//{{AFX_MSG_MAP(CResourceForm)
	ON_WM_DESTROY()
	ON_COMMAND( ID_AMT_RESOURCE_TOOLBAR_DATA, OnCommandPressDataButton )
	ON_COMMAND( ID_AMT_RESOURCE_TOOLBAR_CHARACTER, OnCommandPressCharacterButton )
	ON_COMMAND( ID_AMT_RESOURCE_TOOLBAR_ITEM, OnCommandPressItemButton )
	ON_COMMAND( ID_AMT_RESOURCE_TOOLBAR_OBJECT, OnCommandPressObjectButton )
	ON_COMMAND( ID_AMT_RESOURCE_TOOLBAR_SKILL, 	OnCommandPressSkillButton )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceForm diagnostics

#ifdef _DEBUG
void CResourceForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CResourceForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CResourceForm message handlers

VOID CResourceForm::InitializeMember()
{
	m_hBackBrush				= NULL;

	m_pcsResourceToolBar		= NULL;
	m_pcsResourceToolBarBitmap	= NULL;
	m_pcsResourceToolTip		= NULL;

	m_pcsTreeCurrent			= NULL;
	m_pcsTreeData				= NULL;
	m_pcsTreeCharacter			= NULL;
	m_pcsTreeItem				= NULL;
	m_pcsTreeObject				= NULL;
	m_pcsTreeSkill				= NULL;

	m_hData1					= NULL;
	m_hData2					= NULL;
	m_hCharacter				= NULL;

	m_hItem						= NULL;

	ZeroMemory( m_ahItemChar, sizeof(m_ahItemChar) );
	ZeroMemory( m_ahEquip,	  sizeof(m_ahEquip) );
	ZeroMemory( m_ahArmour,   sizeof(m_ahArmour) );
	ZeroMemory( m_ahBody,     sizeof(m_ahBody) );
	ZeroMemory( m_ahHead,     sizeof(m_ahHead) );
	ZeroMemory( m_ahArms,     sizeof(m_ahArms) );
	ZeroMemory( m_ahHands,    sizeof(m_ahHands) );
	ZeroMemory( m_ahLegs,     sizeof(m_ahLegs) );
	ZeroMemory( m_ahFoot,     sizeof(m_ahFoot) );
	ZeroMemory( m_ahArms2,    sizeof(m_ahArms2) );

	m_hUsable					= NULL;
	m_hOther					= NULL;
	m_hWeaponParent				= NULL;
	m_hPcWeaponParent			= NULL;
	m_hMonsterWeaponParent		= NULL;

	ZeroMemory( m_ahWeapon,			sizeof(m_ahWeapon) );
	ZeroMemory( m_ahMonsterWeapon,	sizeof(m_ahMonsterWeapon) );
	m_hShield					= NULL;
	m_hRing						= NULL;
	m_hNecklace					= NULL;
	m_hRide						= NULL;
	m_hObject					= NULL;
	m_hSkill					= NULL;
}

BOOL CResourceForm::CreateMember()
{
	//////////////////////////////////////////
	// ToolBar
	m_pcsResourceToolBar		= new CToolBarCtrl();
	m_pcsResourceToolBarBitmap	= new CBitmap();
	m_pcsResourceToolTip		= new CToolTipCtrl();

	if(!m_pcsResourceToolBar->Create(WS_CHILD|WS_VISIBLE|TBSTYLE_TRANSPARENT|TBSTYLE_FLAT, CRect(0, 0, 100, 100), this, NULL))
		return FALSE;
	if(!m_pcsResourceToolTip->Create(m_pcsResourceToolBar))
		return FALSE;

	TBBUTTON button = {NULL, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, NULL};
	m_pcsResourceToolBar->InsertButton(0, &button);

	button.fsStyle = TBSTYLE_CHECK;
	button.iBitmap = 0;
	m_pcsResourceToolBar->InsertButton(1, &button);
	button.iBitmap = 1;
	m_pcsResourceToolBar->InsertButton(2, &button);
	button.iBitmap = 2;
	m_pcsResourceToolBar->InsertButton(3, &button);
	button.iBitmap = 3;
	m_pcsResourceToolBar->InsertButton(4, &button);
	button.iBitmap = 4;
	m_pcsResourceToolBar->InsertButton(5, &button);

	button.fsStyle = TBSTYLE_SEP;
	m_pcsResourceToolBar->InsertButton(6, &button);

	m_pcsResourceToolBar->SetCmdID( 1, ID_AMT_RESOURCE_TOOLBAR_DATA );
	m_pcsResourceToolBar->SetCmdID( 2, ID_AMT_RESOURCE_TOOLBAR_CHARACTER );
	m_pcsResourceToolBar->SetCmdID( 3, ID_AMT_RESOURCE_TOOLBAR_ITEM );
	m_pcsResourceToolBar->SetCmdID( 4, ID_AMT_RESOURCE_TOOLBAR_OBJECT );
	m_pcsResourceToolBar->SetCmdID( 5, ID_AMT_RESOURCE_TOOLBAR_SKILL );

	m_pcsResourceToolBarBitmap->LoadBitmap( IDB_RESOURCE_TOOLBAR );
	m_pcsResourceToolBar->AddBitmap( 8, m_pcsResourceToolBarBitmap );

	m_pcsResourceToolTip->AddTool (m_pcsResourceToolBar, AMT_RESOURCE_TOOL_TIP_TEXT_STATIC, CRect(0, 0, 16, 16), ID_AMT_RESOURCE_TOOLBAR_DATA );
	m_pcsResourceToolTip->AddTool (m_pcsResourceToolBar, AMT_RESOURCE_TOOL_TIP_TEXT_CHARACTER, CRect(0, 0, 16, 16), ID_AMT_RESOURCE_TOOLBAR_CHARACTER );
	m_pcsResourceToolTip->AddTool (m_pcsResourceToolBar, AMT_RESOURCE_TOOL_TIP_TEXT_ITME, CRect(0, 0, 16, 16), ID_AMT_RESOURCE_TOOLBAR_ITEM );
	m_pcsResourceToolTip->AddTool (m_pcsResourceToolBar, AMT_RESOURCE_TOOL_TIP_TEXT_OBJECT, CRect(0, 0, 16, 16), ID_AMT_RESOURCE_TOOLBAR_OBJECT );
	m_pcsResourceToolTip->AddTool (m_pcsResourceToolBar, AMT_RESOURCE_TOOL_TIP_TEXT_SKILL, CRect(0, 0, 16, 16), ID_AMT_RESOURCE_TOOLBAR_SKILL );

	m_pcsResourceToolBar->SetToolTips(m_pcsResourceToolTip);
	//////////////////////////////////////////

	//////////////////////////////////////////
	// TreeCtrl
	CRect rtTemp;
	GetClientRect(rtTemp);

	RECT rtTreeRect = {5, 30, rtTemp.Width() - 5, rtTemp.Height() - 5};

	m_pcsTreeData		= new CResourceTree(this, rtTreeRect);
	m_pcsTreeCharacter	= new CResourceTree(this, rtTreeRect);
	m_pcsTreeItem		= new CResourceTree(this, rtTreeRect);
	m_pcsTreeObject		= new CResourceTree(this, rtTreeRect);
	m_pcsTreeSkill		= new CResourceTree(this, rtTreeRect);
	//////////////////////////////////////////

	FocusDataControl();

	return TRUE;
}

void CResourceForm::OnDestroy() 
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	SAFE_DELETE( m_pcsResourceToolBarBitmap );
	SAFE_DELETE( m_pcsResourceToolBar );
	SAFE_DELETE( m_pcsResourceToolTip );

	SAFE_DELETE( m_pcsTreeData );
	SAFE_DELETE( m_pcsTreeCharacter );
	SAFE_DELETE( m_pcsTreeItem );
	SAFE_DELETE( m_pcsTreeObject );
	SAFE_DELETE( m_pcsTreeSkill );
}

void CResourceForm::OnCommandPressDataButton()
{
	FocusDataControl();
}

void CResourceForm::OnCommandPressCharacterButton()
{
	FocusCharacterControl();
}

void CResourceForm::OnCommandPressItemButton()
{
	FocusItemControl();
}

void CResourceForm::OnCommandPressObjectButton()
{
	FocusObjectControl();
}

void CResourceForm::OnCommandPressSkillButton()
{
	FocusSkillControl();
}

VOID CResourceForm::FocusDataControl()
{
	CModelToolApp::GetInstance()->m_lResourceMode = ID_AMT_RESOURCE_TOOLBAR_DATA;

	// ToolBar
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_DATA);

	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_CHARACTER, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_ITEM, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_OBJECT, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_SKILL, FALSE);

	// TreeCtrl
	m_pcsTreeData->_ShowWindow();

	m_pcsTreeCharacter->_HideWindow();
	m_pcsTreeItem->_HideWindow();
	m_pcsTreeObject->_HideWindow();
	m_pcsTreeSkill->_HideWindow();

	// event-button
	/*	m_pcsBtDelete->_HideButton();
	m_pcsBtObjectCategory->_HideButton();
	m_pcsBtCopy->_HideButton();*/

	m_pcsTreeCurrent = m_pcsTreeData;
}

VOID CResourceForm::FocusCharacterControl()
{
	CModelToolApp::GetInstance()->m_lResourceMode = ID_AMT_RESOURCE_TOOLBAR_CHARACTER;

	// ToolBar
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_CHARACTER);

	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_DATA, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_ITEM, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_OBJECT, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_SKILL, FALSE);

	// TreeCtrl
	m_pcsTreeCharacter->_ShowWindow();

	m_pcsTreeData->_HideWindow();
	m_pcsTreeItem->_HideWindow();
	m_pcsTreeObject->_HideWindow();
	m_pcsTreeSkill->_HideWindow();

	// event-button
	/*	m_pcsBtDelete->_ShowButton();
	m_pcsBtObjectCategory->_HideButton();
	m_pcsBtCopy->_ShowButton();*/

	m_pcsTreeCurrent = m_pcsTreeCharacter;
}

VOID CResourceForm::FocusItemControl()
{
	CModelToolApp::GetInstance()->m_lResourceMode = ID_AMT_RESOURCE_TOOLBAR_ITEM;

	// ToolBar
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_ITEM);

	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_CHARACTER, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_DATA, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_OBJECT, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_SKILL, FALSE);

	// TreeCtrl
	m_pcsTreeItem->_ShowWindow();

	m_pcsTreeData->_HideWindow();
	m_pcsTreeCharacter->_HideWindow();
	m_pcsTreeObject->_HideWindow();
	m_pcsTreeSkill->_HideWindow();

	// event-button
	/*	m_pcsBtDelete->_ShowButton();
	m_pcsBtObjectCategory->_HideButton();
	m_pcsBtCopy->_ShowButton();*/

	m_pcsTreeCurrent = m_pcsTreeItem;
}

VOID CResourceForm::FocusObjectControl()
{
	CModelToolApp::GetInstance()->m_lResourceMode = ID_AMT_RESOURCE_TOOLBAR_OBJECT;

	// ToolBar
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_OBJECT);

	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_CHARACTER, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_ITEM, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_DATA, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_SKILL, FALSE);

	// TreeCtrl
	m_pcsTreeObject->_ShowWindow();

	m_pcsTreeData->_HideWindow();
	m_pcsTreeItem->_HideWindow();
	m_pcsTreeCharacter->_HideWindow();
	m_pcsTreeSkill->_HideWindow();

	// event-button
	/*	m_pcsBtDelete->_ShowButton();
	m_pcsBtObjectCategory->_ShowButton();
	m_pcsBtCopy->_HideButton();*/

	m_pcsTreeCurrent = m_pcsTreeObject;
}

VOID CResourceForm::FocusSkillControl()
{
	CModelToolApp::GetInstance()->m_lResourceMode = ID_AMT_RESOURCE_TOOLBAR_SKILL;

	// ToolBar
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_SKILL);

	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_CHARACTER, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_ITEM, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_DATA, FALSE);
	m_pcsResourceToolBar->CheckButton(ID_AMT_RESOURCE_TOOLBAR_OBJECT, FALSE);

	// TreeCtrl
	m_pcsTreeSkill->_ShowWindow();

	m_pcsTreeData->_HideWindow();
	m_pcsTreeItem->_HideWindow();
	m_pcsTreeCharacter->_HideWindow();
	m_pcsTreeObject->_HideWindow();

	// event-button
	/*	m_pcsBtCopy->_ShowButton();
	m_pcsBtDelete->_ShowButton();
	m_pcsBtObjectCategory->_HideButton();*/

	m_pcsTreeCurrent = m_pcsTreeSkill;
}

VOID CResourceForm::InsertItemEx(CResourceTree *pcsCurTree, CHAR *szFindPathName, HTREEITEM hParent, DWORD dwData, int nImage, int nSelectedImage)
{
	CFileFind	csFind;
	BOOL		bFind;
	HTREEITEM	hThis;
	
	for(bFind = csFind.FindFile(szFindPathName); 
		bFind; 
		bFind = csFind.FindNextFile(), hThis = pcsCurTree->_InsertItem(csFind.GetFileName(), hParent, dwData, FALSE, nImage, nSelectedImage));	

	csFind.Close();
}

HTREEITEM CResourceForm::GetTreeHandleItemParent( AgpdItemTemplate* pAgpdItemTemplate, AgcdItemTemplate* pAgcdItemTemplate )
{
	if( !pAgcdItemTemplate )					return m_hItem;
	if( !pAgcdItemTemplate->m_pcsPreData )		return m_hItem;

	char szFileName[256];
	strcpy( szFileName, pAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
	strupr( szFileName );

	int nItemCharacter = CHAR_COMMON;
	if( !strncmp( AMT_RESOURCE_TREE_NAME_MOONELF_HUN, szFileName, strlen(AMT_RESOURCE_TREE_NAME_MOONELF_HUN) ) )
		nItemCharacter = CHAR_MOONELF_HUN;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_MOONELF_SHA, szFileName, strlen(AMT_RESOURCE_TREE_NAME_MOONELF_SHA) ) )
		nItemCharacter = CHAR_MOONELF_SHA;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_MOONELF_SB, szFileName, strlen(AMT_RESOURCE_TREE_NAME_MOONELF_SB) ) )
		nItemCharacter = CHAR_MOONELF_SB;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_ORC_SHA, szFileName, strlen(AMT_RESOURCE_TREE_NAME_ORC_SHA) ) )
		nItemCharacter = CHAR_ORC_SHA;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_ORC_WAR, szFileName, strlen(AMT_RESOURCE_TREE_NAME_ORC_WAR) ) )
		nItemCharacter = CHAR_ORC_WAR;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_ORC_HUN, szFileName, strlen(AMT_RESOURCE_TREE_NAME_ORC_HUN) ) )
		nItemCharacter = CHAR_ORC_HUN;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_HUMAN_ARC, szFileName, strlen(AMT_RESOURCE_TREE_NAME_HUMAN_ARC) ) )
		nItemCharacter = CHAR_HUMAN_ARC;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_HUMAN_KNI, szFileName, strlen(AMT_RESOURCE_TREE_NAME_HUMAN_KNI) ) )
		nItemCharacter = CHAR_HUMAN_KNI;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_HUMAN_WIZ, szFileName, strlen(AMT_RESOURCE_TREE_NAME_HUMAN_WIZ) ) )
		nItemCharacter = CHAR_HUMAN_WIZ;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_DRAGONSCION_SCION, szFileName, strlen(AMT_RESOURCE_TREE_NAME_DRAGONSCION_SCION) ) )
		nItemCharacter = CHAR_DRAGONSCION_SCION;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_DRAGONSCION_SLA, szFileName, strlen(AMT_RESOURCE_TREE_NAME_DRAGONSCION_SLA) ) )
		nItemCharacter = CHAR_DRAGONSCION_SLA;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_DRAGONSCION_OBI, szFileName, strlen(AMT_RESOURCE_TREE_NAME_DRAGONSCION_OBI) ) )
		nItemCharacter = CHAR_DRAGONSCION_OBI;
	else if( !strncmp( AMT_RESOURCE_TREE_NAME_DRAGONSCION_SUM, szFileName, strlen(AMT_RESOURCE_TREE_NAME_DRAGONSCION_SUM) ) )
		nItemCharacter = CHAR_DRAGONSCION_SUM;

	HTREEITEM hParent = m_hItem;
	switch( pAgpdItemTemplate->m_nType )
	{
	case AGPMITEM_TYPE_EQUIP:
		{
			AgpdItemTemplateEquip* pAgpdItemTemplateEquip = (AgpdItemTemplateEquip*)pAgpdItemTemplate;
			AgpmItemEquipKind nKind = pAgpdItemTemplateEquip->m_nKind;
			switch( nKind )
			{
			case AGPMITEM_EQUIP_KIND_ARMOUR:
				{
					AgpmItemPart nPart = pAgpdItemTemplateEquip->GetPartIndex();
					switch( nPart )
					{
					case AGPMITEM_PART_BODY:	hParent = GetTreeHandleBody( nItemCharacter );	break;
					case AGPMITEM_PART_HEAD:	hParent = GetTreeHandleHead( nItemCharacter );	break;
					case AGPMITEM_PART_ARMS:	hParent = GetTreeHandleArms( nItemCharacter );	break;
					case AGPMITEM_PART_HANDS:	hParent = GetTreeHandleHands( nItemCharacter );	break;
					case AGPMITEM_PART_LEGS:	hParent = GetTreeHandleLegs( nItemCharacter );	break;
					case AGPMITEM_PART_FOOT:	hParent = GetTreeHandleFoot( nItemCharacter );	break;
					case AGPMITEM_PART_ARMS2:	hParent = GetTreeHandleArms2( nItemCharacter );	break;
					}
				}
				break;
			case AGPMITEM_EQUIP_KIND_SHIELD:
				{
					hParent = GetTreeHandleShield();
				}
				break;
			case AGPMITEM_EQUIP_KIND_WEAPON:
				{
					AgpmItem* pAgpmItem = CModelToolApp::GetInstance()->GetEngine()->GetAgpmItemModule();
					AgpdItemTemplateEquipWeapon* pcsWeapon = (AgpdItemTemplateEquipWeapon *)(pAgpdItemTemplateEquip);
					int nRace = 0;
					pAgpmItem->m_pagpmFactors->GetValue( &pcsWeapon->m_csRestrictFactor, &nRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE );

					INT32 lGender = pAgpmItem->m_pagpmFactors->GetGender(&pcsWeapon->m_csRestrictFactor);
					hParent = lGender == 99 ? GetTreeHandleMonsterWeapon( pcsWeapon->m_nWeaponType ) : GetTreeHandlePcWeapon( pcsWeapon->m_nWeaponType );
				}
				break;
			case AGPMITEM_EQUIP_KIND_RING:
				{
					hParent = GetTreeHandleRing();
				}
				break;
			case AGPMITEM_EQUIP_KIND_NECKLACE:
				{
					hParent = GetTreeHandleNecklace();
				}
				break;
			case AGPMITEM_EQUIP_KIND_RIDE:
				{
					hParent = GetTreeHandleRide();
				}
				break;
			}
		}
		break;

	case AGPMITEM_TYPE_USABLE:
		{
			hParent = GetTreeHandleUsable();
		}
		break;

	case AGPMITEM_TYPE_OTHER:
		{
			hParent = GetTreeHandleOther();
		}
		break;
	}

	return hParent;
}

void CResourceForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(CModelToolApp::GetInstance())
	{
		if(CModelToolApp::GetInstance()->m_pMainWnd)
		{
			CRect rtTemp;
			GetClientRect(rtTemp);

			RECT rtTreeRect = {5, 30, rtTemp.Width() - 5, rtTemp.Height() - 5};

			m_pcsTreeData->MoveWindow(&rtTreeRect);
			m_pcsTreeCharacter->MoveWindow(&rtTreeRect);
			m_pcsTreeItem->MoveWindow(&rtTreeRect);
			m_pcsTreeObject->MoveWindow(&rtTreeRect);
			m_pcsTreeSkill->MoveWindow(&rtTreeRect);
		}
	}
}

VOID CResourceForm::UpdateItemResourceTree()
{
	m_pcsTreeItem->UpdateWindow();
}

VOID CResourceForm::UpdateObjectResourceTree()
{
	m_pcsTreeObject->UpdateWindow();
}

VOID CResourceForm::UpdateCharacterResourceTree()
{
	m_pcsTreeCharacter->UpdateWindow();
}

VOID CResourceForm::UpdateSkillResourceTree()
{
	m_pcsTreeSkill->UpdateWindow();
}

BOOL CResourceForm::IsEquipments(HTREEITEM hItem)
{
	HTREEITEM hParent = m_pcsTreeCurrent->GetParentItem( hItem );
	if (!hParent)		return FALSE;

	switch( m_pcsTreeCurrent->GetItemData( hParent ) )
	{
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER:
		return TRUE;
	}

	return FALSE;
}

BOOL CResourceForm::ReadCharacterLabel( CHAR *szPathName )
{
	FILE* pInFile = fopen( szPathName, "rt" );
	if( !pInFile )		return FALSE;

	INT32			lIndex;
	CString			strCurrent, strParent;
	CHAR			szCurrent[256], szParent[256];
	HTREEITEM		hParent;

	while( !feof( pInFile ) )
	{
		if( 1 > fscanf( pInFile, "%d : %s %s", &lIndex, szCurrent, szParent ) )
			break;

		strCurrent	= szCurrent;
		strParent	= szParent;

		strCurrent.Insert( 0, "[" );
		strCurrent += "]";

		if( strParent == AMT_SAVE_TREE_NAME_ROOT ) {
			m_pcsTreeCharacter->_InsertItem( strCurrent, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER );
		}
		else {
			strParent.Insert( 0, "[" );
			strParent += "]";

			hParent	= m_pcsTreeCharacter->_FindItemEx( strParent );
			if( !hParent )
				return FALSE;

			m_pcsTreeCharacter->_InsertItem( strCurrent, hParent, E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER );
		}
	}

	return TRUE;
}

BOOL CResourceForm::SaveCharacterLabel( CHAR *szPathName, VOID (*pMsgFuncCB) (const CHAR* pMsg, VOID*), CHAR* pParam1 )
{
	FILE *pOutFile = fopen( szPathName, "wt" );
	if( !pOutFile )		return FALSE;

	HTREEITEM		hItem = m_pcsTreeCharacter->GetRootItem();

	INT32			lCount = 0;
	HTREEITEM		hTemp;
	CString			strCurrent, strParent;

	char			pMsgBuff[256];

	while( hItem )
	{
		while( hItem )
		{
			if( E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER == m_pcsTreeCharacter->GetItemData( hItem ) ) {
				strCurrent	= m_pcsTreeCharacter->GetItemText(hItem);
				strCurrent	= strCurrent.Mid(1, strCurrent.GetLength() - 2);

				hTemp	= m_pcsTreeCharacter->GetParentItem(hItem);
				if( hTemp )	{
					strParent	= m_pcsTreeCharacter->GetItemText(hTemp);
					strParent	= strParent.Mid(1, strParent.GetLength() - 2);
				}
				else {
					strParent = AMT_SAVE_TREE_NAME_ROOT;
				}
				fprintf(pOutFile, "%d : %s %s\n", lCount, strCurrent, strParent);
				++lCount;

				if ( pMsgFuncCB )
				{
					sprintf(pMsgBuff, "%s\n%s", pParam1, strCurrent);
					pMsgFuncCB(pMsgBuff, NULL);
				}
			}

			hTemp = m_pcsTreeCharacter->GetNextItem( hItem, TVGN_CHILD );
			if( !hTemp ) { // 모두 돌았다.
				break;
			}

			hItem = hTemp;
		}

		while( hItem )
		{
			hTemp = m_pcsTreeCharacter->GetNextItem(hItem, TVGN_NEXT);
			if( hTemp ) {
				hItem = hTemp;
				break;
			}

			hItem = m_pcsTreeCharacter->GetParentItem(hItem);
			if( !hItem ) {  // 모두 돌았다.
				break;
			}
		}
	}
	fclose( pOutFile );

	return TRUE;
}

BOOL CResourceForm::ChangeLabel( HTREEITEM hParent )
{
	if( !CModelToolApp::GetInstance() )		return FALSE;

	AgcEngineChild *pEngine	= CModelToolApp::GetInstance()->GetEngine();
	if( !pEngine )							return FALSE;
		

	DWORD				dwData;
	HTREEITEM			hItem;
	CString				strLabel;
	if( m_pcsTreeCurrent == m_pcsTreeCharacter )
	{
		AgpmCharacter	*pcsAgpmCharacter	= pEngine->GetAgpmCharacterModule();
		AgcmCharacter	*pcsAgcmCharacter	= pEngine->GetAgcmCharacterModule();
		if( !pcsAgpmCharacter || !pcsAgcmCharacter )		return FALSE;

		AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;
		AgcdCharacterTemplate	*pcsAgcdCharacterTemplate;

		strLabel = m_pcsTreeCharacter->GetItemText(hParent);
		strLabel = strLabel.Mid(1, strLabel.GetLength() - 2);

		hItem = m_pcsTreeCharacter->GetChildItem(hParent);
		while( hItem )
		{
			dwData = m_pcsTreeCharacter->GetItemData(hItem);
			if( dwData != E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER ) {
				pcsAgpdCharacterTemplate = pcsAgpmCharacter->GetCharacterTemplate((INT32)(dwData));
				if( !pcsAgpdCharacterTemplate )					return FALSE;

				pcsAgcdCharacterTemplate = pcsAgcmCharacter->GetTemplateData(pcsAgpdCharacterTemplate);
				if( !pcsAgcdCharacterTemplate )					return FALSE;
				if( !pcsAgcdCharacterTemplate->m_pcsPreData )	return FALSE;
					

				if( !pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel )
					pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel = new CHAR [strlen((LPSTR)(LPCSTR)(strLabel)) + 1];

				strcpy( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel, (LPSTR)(LPCSTR)(strLabel) );
			}

			hItem = m_pcsTreeCharacter->GetNextItem(hItem, TVGN_NEXT);
		}
	}
	else if( m_pcsTreeCurrent == m_pcsTreeObject )
	{
		ApmObject  *pcsApmObject = pEngine->GetApmObjectModule();
		AgcmObject *pcsAgcmObject= pEngine->GetAgcmObjectModule();
		if( !pcsApmObject || !pcsAgcmObject )		return FALSE;

		ApdObjectTemplate  *pcsApdObjectTemplate;
		AgcdObjectTemplate *pcsAgcdObjectTemplate;

		strLabel = m_pcsTreeObject->GetItemText(hParent);
		strLabel = strLabel.Mid(1, strLabel.GetLength() - 2);

		hItem	= m_pcsTreeObject->GetChildItem(hParent);
		while( hItem )
		{
			dwData = m_pcsTreeObject->GetItemData(hItem);

			if (dwData != E_AMT_ID_RESOURCE_TREE_DATA_OBJECT)
			{
				pcsApdObjectTemplate		= pcsApmObject->GetObjectTemplate((INT32)(dwData));
				if( !pcsApdObjectTemplate )		return FALSE;
				pcsAgcdObjectTemplate		= pcsAgcmObject->GetTemplateData(pcsApdObjectTemplate);
				if( !pcsAgcdObjectTemplate )	return FALSE;

				strcpy( pcsAgcdObjectTemplate->m_szCategory, strLabel );
			}

			hItem = m_pcsTreeObject->GetNextItem(hItem, TVGN_NEXT);
		}
	}
	else
		return FALSE;

	return TRUE;
}

BOOL CResourceForm::ReadObjectLabel( CHAR *szPathName )
{
	FILE *pInFile = fopen(szPathName, "rt");
	if( !pInFile )		return FALSE;

	INT32		lLen, lIndex;
	CHAR		szLabel[256], szLabel2[256];
	HTREEITEM	hParent[10];

	memset( hParent, 0, sizeof(HTREEITEM) * 10 );

	while( !feof(pInFile) )
	{
		if( fscanf(pInFile, "%d|", &lIndex) < 1 )
			break;

		fgets(szLabel, 256, pInFile);
		lLen = strlen(szLabel) - 1;
		if( 10 == szLabel[lLen] )
			szLabel[lLen] = '\0';

		sprintf(szLabel2, "[%s]", szLabel);

		if( lIndex == 0 ) {
			hParent[lIndex] = m_pcsTreeObject->_InsertItem( szLabel2, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_OBJECT );
		}
		else {
			if( !hParent[lIndex - 1] ) {
				fclose(pInFile);
				return FALSE;
			}

			hParent[lIndex] = m_pcsTreeObject->_InsertItem( szLabel2, hParent[lIndex - 1], E_AMT_ID_RESOURCE_TREE_DATA_OBJECT );
		}
	}
	fclose(pInFile);

	return TRUE;
}

BOOL CResourceForm::SaveObjectLabel( CHAR *szPathName, VOID (*pMsgFuncCB) (const CHAR* pMsg, VOID*), CHAR* pParam1 )
{
	FILE *pOutFile = fopen( szPathName, "wt" );
	if( !pOutFile )		return FALSE;

	HTREEITEM hItem	= m_pcsTreeObject->GetRootItem();

	INT32			lIndex		= 0;
	HTREEITEM		hTemp;
	CString			strText;

	char pMsgBuff[256];

	while (hItem)
	{
		while (hItem)
		{
			if( E_AMT_ID_RESOURCE_TREE_DATA_OBJECT == m_pcsTreeObject->GetItemData(hItem) )	{
				strText	= m_pcsTreeObject->GetItemText(hItem);
				strText	= strText.Mid(1, strText.GetLength() - 2);
				fprintf(pOutFile, "%d|%s\n", lIndex, strText);

				if ( pMsgFuncCB )
				{
					sprintf(pMsgBuff, "%s\n%s", pParam1, strText);
					pMsgFuncCB(pMsgBuff, NULL);
				}
			}

			hTemp = m_pcsTreeObject->GetNextItem(hItem, TVGN_CHILD);
			if( hTemp == NULL ) break;
			else				++lIndex;

			hItem = hTemp;
		}

		while( hItem )
		{
			hTemp = m_pcsTreeObject->GetNextItem(hItem, TVGN_NEXT);
			if( hTemp )	{
				hItem = hTemp;
				break;
			}

			hItem = m_pcsTreeObject->GetParentItem(hItem);
			if( !hItem ) {
				fclose(pOutFile);
				return TRUE;
			}
			else {
				--lIndex;
			}
		}
	}

	fclose(pOutFile);

	return TRUE;
}