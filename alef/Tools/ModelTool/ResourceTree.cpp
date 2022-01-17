// ResourceTree.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "EditDlg.h"
#include "ResourceTree.h"
#include "TreeDlg.h"
#include ".\resourcetree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceTree
CResourceTree::CResourceTree(CWnd *pcsParent, RECT &stInitRect)
{
	_Create(pcsParent, stInitRect, WS_DLGFRAME);

	m_StateImageLIst.Create(IDB_TRUNK_ONLY, 16, 1, RGB(255, 255, 255));
	SetImageList(&m_StateImageLIst, TVSIL_STATE);

	ASSERT( CModelToolApp::GetInstance() );
	ASSERT( CModelToolDlg::GetInstance() );
}

CResourceTree::~CResourceTree()
{
}

BEGIN_MESSAGE_MAP(CResourceTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CResourceTree)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_EDIT_LABEL, OnCommandResourceTreeContextMenuEditLabel)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_LABEL, OnCommandResourceTreeContextMenuDeleteLabel)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_INSERT_LABEL, OnCommandResourceTreeContextMenuInsertLabel)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM, OnCommandResourceTreeContextMenuCopyItem)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM, OnCommandResourceTreeContextMenuDeleteItem)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_MOVE_ITEM, OnCommandResourceTreeContextMenuMoveItem)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE, OnCommandResourceTreeContextMenuUpdateResource)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_TRUNK_ONLY, OnCommandResourceTreeContextMenuTrunkOnly)
	ON_COMMAND(ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_SAVE, OnCommandResourceTreeContextMenuSave)
	//}}AFX_MSG_MAP
	//ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceTree message handlers

void CResourceTree::OnBegindrag( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = 0;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelected = pNMTreeView->itemNew.hItem;
	if( !hSelected )	return;

	BOOL bSetDrag;

	switch( GetItemData(hSelected) )
	{
	case E_AMT_ID_RESOURCE_TREE_DATA_STATIC_CHARACTER:
	case E_AMT_ID_RESOURCE_TREE_DATA_STATIC_OBJECT:
	case E_AMT_ID_RESOURCE_TREE_DATA_OBJECT:
	case E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_USABLE:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_OTHER:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER:
	case E_AMT_ID_RESOURCE_TREE_DATA_SKILL:
		bSetDrag = FALSE;
		break;
	default:
		bSetDrag = TRUE;
		break;
	}

	if( bSetDrag )
		SetDrag( pNMTreeView->itemNew.hItem );
}

void CResourceTree::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( m_bDrag == FALSE ) {
		CTreeCtrl::OnLButtonUp(nFlags, point);
		return;
	}

	ReleaseCapture();
	m_bDrag = FALSE;

	if( CModelToolApp::GetInstance()->CursorInRenderView() == FALSE ) {
		CTreeCtrl::OnLButtonUp(nFlags, point);
		return;
	}

	HTREEITEM hSelected	= GetSelectedItem();
	if( hSelected == NULL ) {
		CTreeCtrl::OnLButtonUp(nFlags, point);
		return;
	}


	DWORD dwData;
	CResourceTree* pCur = ( CResourceTree * )( this );

	if( pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER;
	}
	else if( pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_OBJECT;
	}
	else {
		dwData = 0;
	}

	HTREEITEM hParent = GetParentItem( hSelected );
	if( hParent != NULL ) {
		dwData = GetItemData( hParent );
	}
	else if (dwData == 0) {
		CTreeCtrl::OnLButtonUp(nFlags, point);
		return;
	}

	switch( dwData )
	{
	case E_AMT_ID_RESOURCE_TREE_DATA_STATIC_CHARACTER:
		{
			if( !CModelToolApp::GetInstance()->SetStaticModel( GetItemText( hSelected ), FALSE ) ) {
				OutputDebugString("!\n!\nERROR <CResourceTree::OnLButtonUp() - 데이터를 로드할 수 없습니다.>\n!\n!\n");
				MessageBox("데이터를 로드할 수 없습니다.", "ERROR", MB_OK);
			}
		}
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_STATIC_OBJECT:
		{
			if( !CModelToolApp::GetInstance()->SetStaticModel( GetItemText( hSelected ), TRUE ) ) {
				OutputDebugString("!\n!\nERROR <CResourceTree::OnLButtonUp() - 데이터를 로드할 수 없습니다.>\n!\n!\n");
				MessageBox("데이터를 로드할 수 없습니다.", "ERROR", MB_OK);
			}
		}
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER:
		{
			CModelToolApp::GetInstance()->MakeTargetCharacter();

			if( !CModelToolApp::GetInstance()->SetCharacter( GetItemData( hSelected ), hSelected) ) {
				OutputDebugString("!\n!\nERROR <CResourceTree::OnLButtonUp() - 캐릭터를 로드할 수 없습니다.>\n!\n!\n");
				MessageBox("데이터를 로드할 수 없습니다.", "ERROR", MB_OK);
			}
		}
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_USABLE:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_OTHER:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR:
	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER:
		{
			if( !CModelToolApp::GetInstance()->SetItem( GetItemData(hSelected), hSelected ) ) {
				OutputDebugString("!\n!\nERROR <CResourceTree::OnLButtonUp() - 아이템을 로드할 수 없습니다.>\n!\n!\n");
				MessageBox("데이터를 로드할 수 없습니다.", "ERROR", MB_OK);
			}
		}
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_OBJECT:
		{
			if( !CModelToolApp::GetInstance()->SetObject( GetItemData(hSelected), hSelected ) )	{
				OutputDebugString("!\n!\nERROR <CResourceTree::OnLButtonUp() - 오브젝트를 로드할 수 없습니다.>\n!\n!\n");
				MessageBox("데이터를 로드할 수 없습니다.", "ERROR", MB_OK);
			}
		}
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_SKILL:
		{
			if( !CModelToolApp::GetInstance()->SetSkill( GetItemData(hSelected), hSelected ) ) {
				OutputDebugString("!\n!\nERROR <CResourceTree::OnLButtonUp() - 스킬을 로드할 수 없습니다.>\n!\n!\n");
				MessageBox("데이터를 로드할 수 없습니다.", "ERROR", MB_OK);
			}
		}
		break;
	};

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CResourceTree::OnMouseMove( UINT nFlags, CPoint point )
{
	if( !m_bDrag )		return;

	int nID = CModelToolApp::GetInstance()->CursorInRenderView() ? IDC_POINTER_COPY : IDC_NODROP;
	SetCursor( AfxGetApp()->LoadCursor( nID ) );
	CTreeCtrl::OnMouseMove( nFlags, point );
}

BOOL CResourceTree::_AppendMenu( CMenu *pMenu, UINT32 ulIDCommand )
{
	CHAR	szTextItem[AMT_MAX_STR];

	switch( ulIDCommand )
	{
	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_EDIT_LABEL:
		strcpy(szTextItem, "Edit");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_LABEL:
	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM:
		strcpy(szTextItem, "Delete");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_INSERT_LABEL:
		strcpy(szTextItem, "Insert");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM:
		strcpy(szTextItem, "Copy");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_MOVE_ITEM:
		strcpy(szTextItem, "Move");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE:
		strcpy(szTextItem, "Update");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_TRUNK_ONLY :
		strcpy(szTextItem, "Trunk Only");
		break;

	case ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_SAVE:
		strcpy(szTextItem, "Save");
		break;

	default:
		return FALSE;
	}

	pMenu->AppendMenu( MF_STRING, ulIDCommand, szTextItem );

	return TRUE;
}

BOOL CResourceTree::_TrackPopupMenu(CMenu *pMenu)
{
	POINT	_point;
	GetCursorPos(&_point);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, _point.x, _point.y, this);
	pMenu->DestroyMenu();

	return TRUE;
}

BOOL CResourceTree::_AddSeparator(CMenu *pMenu)
{
	return pMenu->AppendMenu(MF_SEPARATOR);
}

BOOL CResourceTree::TrackDefaultPopupMenu()
{
	CMenu	menu;
	menu.CreatePopupMenu();
	_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
	_TrackPopupMenu(&menu);

	return TRUE;
}

BOOL CResourceTree::TrackCharacterPopupMenu(HTREEITEM hSelectedItem)
{
	SelectItem(hSelectedItem);

	{
		CMenu	menu;
		menu.CreatePopupMenu();

		if (E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER == GetItemData(hSelectedItem))
		{
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_INSERT_LABEL);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_LABEL);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_EDIT_LABEL);
			_AddSeparator(&menu);			
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
		}
		else
		{
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_MOVE_ITEM);
			_AddSeparator(&menu);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_TRUNK_ONLY);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_SAVE);
		}

		_TrackPopupMenu(&menu);
	}

	return TRUE;
}

BOOL CResourceTree::TrackItemPopupMenu(HTREEITEM hSelectedItem)
{
	SelectItem(hSelectedItem);

	{
		CMenu	menu;
		menu.CreatePopupMenu();

		//@{ burumal 2006/01/18
		DWORD dwSelected = GetItemData(hSelectedItem);
		switch ( dwSelected )
		{
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM :
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR :
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_USABLE :
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_OTHER :
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP :
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR :
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER :
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM);
			_AddSeparator(&menu);			
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
			_TrackPopupMenu(&menu);
			break;

		default :
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM);
			_AddSeparator(&menu);			
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_TRUNK_ONLY);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_SAVE);
			_TrackPopupMenu(&menu);
			break;
		}
		//@}
	}

	return TRUE;
}

BOOL CResourceTree::TrackObjectPopupMenu(HTREEITEM hSelectedItem)
{
	SelectItem(hSelectedItem);

	{
		CMenu	menu;
		menu.CreatePopupMenu();

		if (E_AMT_ID_RESOURCE_TREE_DATA_OBJECT == GetItemData(hSelectedItem))
		{
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_INSERT_LABEL);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_LABEL);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_EDIT_LABEL);
			_AddSeparator(&menu);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
		}
		else
		{
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_MOVE_ITEM);
			_AddSeparator(&menu);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_TRUNK_ONLY);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_SAVE);
		}

		_TrackPopupMenu(&menu);
	}

	return TRUE;
}

BOOL CResourceTree::TrackSkillPopupMenu(HTREEITEM hSelectedItem)
{
	HTREEITEM	hParentItem	= GetParentItem(hSelectedItem);
	if (!hParentItem)
		return FALSE;

	if (E_AMT_ID_RESOURCE_TREE_DATA_SKILL != GetItemData(hParentItem))
		return FALSE;

	SelectItem(hSelectedItem);

	{
		CMenu	menu;
		menu.CreatePopupMenu();
		{
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_COPY_ITEM);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_DELETE_ITEM);
			_AddSeparator(&menu);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_UPDATE_RESOURCE);
			_AppendMenu(&menu, ID_AMT_COMMAND_RESOURCE_TREE_CONTEXT_MENU_TRUNK_ONLY);
		}

		_TrackPopupMenu(&menu);
	}

	return TRUE;
}

void CResourceTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hSelected = HitTest(point);
	if (hSelected)
	{
		CResourceTree *pCur = (CResourceTree *)(this);
		if ((!pCur) || (!CModelToolDlg::GetInstance()) || (!CModelToolDlg::GetInstance()->GetResourceForm()))
			return;

		if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter())
		{
			TrackCharacterPopupMenu(hSelected);
		}
		else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem())
		{
			TrackItemPopupMenu(hSelected);
		}
		else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject())
		{
			TrackObjectPopupMenu(hSelected);
		}
		else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill())
		{
			TrackSkillPopupMenu(hSelected);
		}
	}
	else
	{
		TrackDefaultPopupMenu();
	}

	CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CResourceTree::OnCommandResourceTreeContextMenuEditLabel()
{
	ReleaseCapture();

	HTREEITEM hSelected = GetSelectedItem();
	if (!hSelected)
		return;

	CString	strTextSelectedItem = GetItemText(hSelected);

	strTextSelectedItem = strTextSelectedItem.Mid(1, strTextSelectedItem.GetLength() - 2);

	CHAR	szLabel[AMT_MAX_STR], szNewLabel[AMT_MAX_STR];
	strcpy(szLabel, strTextSelectedItem);

	CEditDlg dlg(szLabel, "새로운 label을 입력하세요.");
	if (dlg.DoModal() == IDOK)
	{
		sprintf(szNewLabel, "[%s]", szLabel);
		SetItemText(hSelected, szNewLabel);

		if ( !CModelToolDlg::GetInstance()->GetResourceForm()->ChangeLabel( hSelected ) )
			return;
	}
}

void CResourceTree::OnCommandResourceTreeContextMenuDeleteLabel()
{
	HTREEITEM hSelected = GetSelectedItem();
	if (!hSelected)
		return;

	if (hSelected == GetRootItem())
	{
		MessageBox("루트 레이블은 삭제할 수 없습니다.");
		return;
	}

	if (ItemHasChildren(hSelected))
	{
		MessageBox("자식이 있으면 삭제할 수 없습니다.");
		return;
	}

	//@{ 2006/05/04 burumal
	//DeleteItem(hSelected);
	_DeleteItemEx(hSelected);
	//@}
}

void CResourceTree::OnCommandResourceTreeContextMenuInsertLabel()
{
	ReleaseCapture();

	HTREEITEM	hParent			= GetSelectedItem();
	if (!hParent)
		return;

	DWORD		dwDataParent	= GetItemData(hParent);

	CHAR	szLabel[AMT_MAX_STR], szNewLabel[AMT_MAX_STR];
	strcpy(szLabel, "");

	CEditDlg dlg(szLabel, "Label을 입력하세요.");
	if (dlg.DoModal() == IDOK)
	{
		sprintf(szNewLabel, "[%s]", szLabel);
		_InsertItem(szNewLabel, hParent, dwDataParent, TRUE);
	}
}

void CResourceTree::OnCommandResourceTreeContextMenuCopyItem()
{
	ReleaseCapture();

	HTREEITEM hSelectedItem	= GetSelectedItem();
	if (!hSelectedItem)
		return;

	INT32 lTID = (INT32)(GetItemData(hSelectedItem));
	if (lTID < 1)
	{
		MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
		return;
	}

	CResourceTree *pCur = (CResourceTree *)(this);
	if ((!pCur) && (!CModelToolDlg::GetInstance()) && (!CModelToolDlg::GetInstance()->GetResourceForm()))
	{
		MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
		return;
	}

	if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter())
	{
		if (!CModelToolApp::GetInstance()->CopyCharacterTemplate(lTID))
		{
			MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
			return;
		}
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem())
	{
		if (!CModelToolApp::GetInstance()->CopyItemTemplate(lTID))
		{
			MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
			return;
		}
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject())
	{
		if (!CModelToolApp::GetInstance()->CopyObjectTemplate(lTID))
		{
			MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
			return;
		}
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill())
	{
		if (!CModelToolApp::GetInstance()->CopySkillTemplate(lTID))
		{
			MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
			return;
		}
	}
	else
	{
		MessageBox("Copy에 실패했습니다!", "ERROR", MB_OK);
		return;
	}

	MessageBox("Copy를 성공했습니다~", "COPY", MB_OK);

	return;
}

void CResourceTree::OnCommandResourceTreeContextMenuDeleteItem()
{
	HTREEITEM hSelected	= GetSelectedItem();
	if ( !hSelected )		return;

	switch( GetItemData( hSelected ) )
	{
		case E_AMT_ID_RESOURCE_TREE_DATA_STATIC_CHARACTER:
		case E_AMT_ID_RESOURCE_TREE_DATA_STATIC_OBJECT:
		case E_AMT_ID_RESOURCE_TREE_DATA_OBJECT:
		case E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER:
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM:
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_USABLE:
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_OTHER:
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP:
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR:
		case E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER:
		case E_AMT_ID_RESOURCE_TREE_DATA_SKILL:
			return;
	}

	eAmtObjectType eType;
	if (this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter())
		eType = AMT_OBJECT_TYPE_CHARACTER;
	else if (this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem())
		eType = AMT_OBJECT_TYPE_ITEM;
	else if (this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject())
		eType = AMT_OBJECT_TYPE_OBJECT;
	else if (this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill())
		eType = AMT_OBJECT_TYPE_SKILL;
	else
		return;

	if( MessageBox("해당 오브젝트의 모든 속성이 사라집니다.\n그래도 삭제하시겠습니까?\n(INI ObjectTemplete File을 직접지우서야합니다)", "삭제할껴?", MB_ICONQUESTION | MB_YESNO) != IDYES )
		return;

	INT32 lTID = (INT32)(GetItemData(hSelected));
	if ( CModelToolApp::GetInstance()->DeleteResourceData( eType, lTID ) )
		_DeleteItemEx(hSelected);
}

void CResourceTree::OnCommandResourceTreeContextMenuUpdateResource()
{
	CResourceTree* pCur = (CResourceTree*)(this);

	BOOL bSave = FALSE;
	if (pCur != CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeData())
		if(MessageBox("템플릿을 저장하시겠습니까?", "", MB_ICONQUESTION | MB_YESNO) == IDYES)
			bSave = TRUE;

	if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeData())
	{
		DeleteFile("CharacterList.Dty");
		DeleteFile("ObjectList.Dty");
		CModelToolApp::GetInstance()->ReloadStaticData();
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter())
	{
		CModelToolApp::GetInstance()->ReloadCharacterTemplate(bSave);
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem())
	{
		CModelToolApp::GetInstance()->ReloadItemTemplate(bSave);
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject())
	{
		CModelToolApp::GetInstance()->ReloadObjectTemplate(bSave);
	}
	else if (pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill())
	{
		CModelToolApp::GetInstance()->ReloadSkillTemplate(bSave);
	}
}

HTREEITEM CResourceTree::OpenTree(DWORD dwData)
{
	CTreeCtrlEx	csSrcTree;
	csSrcTree._Create(this, CRect(0, 0, 0, 0));

	HTREEITEM hItem = GetRootItem();

	HTREEITEM hTemp;
	CString strText;
	while( hItem )
	{
		while( hItem )
		{
			if( dwData == GetItemData(hItem) )
			{
				hTemp = GetParentItem(hItem);
				if( hTemp )
				{
					strText = GetItemText(hTemp);
					hTemp	= csSrcTree._FindItemEx(strText);
					if( !hTemp )		return NULL;
						
				}
				strText = GetItemText(hItem);

				csSrcTree._InsertItem(strText, (hTemp) ? (hTemp) : (TVI_ROOT), (DWORD)(hItem));
			}

			hTemp = GetNextItem(hItem, TVGN_CHILD);
			if( !hTemp )	break;	//end

			hItem = hTemp;
		}

		while( hItem )
		{
			hTemp = GetNextItem(hItem, TVGN_NEXT);
			if( hTemp ) {
				hItem = hTemp;
				break;
			}

			hItem = GetParentItem(hItem);
			if( !hItem ) {  // 모두 돌았다.
				break;
			}
		}
	}

	DWORD	dwItem = 0;
	CTreeDlg dlg( &dwItem, &csSrcTree );
	return dlg.DoModal() == IDOK ? (HTREEITEM)(dwItem) : NULL;
}

void CResourceTree::OnCommandResourceTreeContextMenuMoveItem()
{
	ReleaseCapture();

	HTREEITEM hSelectedItem	= GetSelectedItem();
	if( !hSelectedItem )		return;

	CResourceTree* pCur = ( CResourceTree * )( this );

	DWORD dwData;
	if( pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER;
	}
	else if( pCur == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_OBJECT;
	}	
	else {
		return;
	}

	HTREEITEM hNewParentItem = OpenTree( dwData );
	if ( !hNewParentItem )					return;

	HTREEITEM hParentItem = GetParentItem( hSelectedItem );
	if( hParentItem == hNewParentItem )		return;
	

	DWORD	dwItemData		= GetItemData( hSelectedItem );
	CString	strNewLabel		= GetItemText( hNewParentItem );
	strNewLabel				= strNewLabel.Mid( 1, strNewLabel.GetLength() - 2 );

	switch( dwData )
	{
	case E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER:
		{
			CModelToolApp::GetInstance()->SetCharacterLabel((INT32)(dwItemData), (LPSTR)(LPCSTR)(strNewLabel));
		}
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_OBJECT:
		{
			CModelToolApp::GetInstance()->SetObjectLabel((INT32)(dwItemData), (LPSTR)(LPCSTR)(strNewLabel));
		}
		break;

	default:
		return;
	}

	HTREEITEM hMoved = _MoveItem( hSelectedItem, hNewParentItem );
	if ( !hMoved )		return;
	
	int nImageIdx = GetItemState(hSelectedItem, TVIS_STATEIMAGEMASK) >> 12;	
	BOOL bFlag = (nImageIdx == 1 ? TRUE : FALSE);
	SetItemState( hMoved, INDEXTOSTATEIMAGEMASK((INT32) bFlag), TVIS_STATEIMAGEMASK );
	
	DWORD dwMovedData = GetItemData( hMoved );
	switch( dwData )
	{
	case E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER:
		CModelToolApp::GetInstance()->SetCharacterTemplateDNF((INT32)(dwMovedData), bFlag);
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_OBJECT:
		CModelToolApp::GetInstance()->SetObjectTemplateDNF((INT32)(dwMovedData), bFlag);
		break;
	}
}

void CResourceTree::OnCommandResourceTreeContextMenuTrunkOnly()
{
	ReleaseCapture();

	HTREEITEM hSelectedItem	= GetSelectedItem();
	if( !hSelectedItem )		return;
	
	int nImageIdx = GetItemState(hSelectedItem, TVIS_STATEIMAGEMASK) >> 12;	
	BOOL bFlag = (nImageIdx == 1 ? FALSE : TRUE);
	SetItemState(hSelectedItem, INDEXTOSTATEIMAGEMASK((INT32) bFlag), TVIS_STATEIMAGEMASK);
	
	DWORD dwData;
	if( this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER;
	}
	else if( this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_OBJECT;
	}	
	else
	if( this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_ITEM;
	}
	else if( this == CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill() ) {
		dwData = E_AMT_ID_RESOURCE_TREE_DATA_SKILL;
	}	
	else {
		return;
	}

	INT32 nCurDNF = 0;
	DWORD dwItemData = GetItemData( hSelectedItem );

	switch( dwData )
	{
	case E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER:
		CModelToolApp::GetInstance()->SetCharacterTemplateDNF((INT32)(dwItemData), bFlag);		
		if ( CModelToolApp::GetInstance()->GetCharacterTemplateDNF((INT32)(dwItemData), &nCurDNF) )
			SetTreeDNF_Flag(hSelectedItem, CModelToolApp::GetInstance()->GetNationalCode(), nCurDNF);		
		if ( (nCurDNF & (0x1 << CModelToolApp::GetInstance()->GetNationalCode())) == FALSE )
			CModelToolApp::GetInstance()->ResetCharacterResourceTreeDNF(CModelToolApp::GetInstance()->GetNationalCode());
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_OBJECT:
		CModelToolApp::GetInstance()->SetObjectTemplateDNF((INT32)(dwItemData), bFlag);
		if ( CModelToolApp::GetInstance()->GetObjectTemplateDNF((INT32)(dwItemData), &nCurDNF) )
			SetTreeDNF_Flag(hSelectedItem, CModelToolApp::GetInstance()->GetNationalCode(), nCurDNF);
		if ( (nCurDNF & (0x1 << CModelToolApp::GetInstance()->GetNationalCode())) == FALSE )
			CModelToolApp::GetInstance()->ResetObjectResourceTreeDNF(CModelToolApp::GetInstance()->GetNationalCode());
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_SKILL :
		CModelToolApp::GetInstance()->SetSkillTemplateDNF((INT32)(dwItemData), bFlag);
		if ( CModelToolApp::GetInstance()->GetSkillTemplateDNF((INT32)(dwItemData), &nCurDNF) )
			SetTreeDNF_Flag(hSelectedItem, CModelToolApp::GetInstance()->GetNationalCode(), nCurDNF);
		if ( (nCurDNF & (0x1 << CModelToolApp::GetInstance()->GetNationalCode())) == FALSE )
			CModelToolApp::GetInstance()->ResetSkillResourceTreeDNF(CModelToolApp::GetInstance()->GetNationalCode());
		break;

	case E_AMT_ID_RESOURCE_TREE_DATA_ITEM :	
		CModelToolApp::GetInstance()->SetItemTemplateDNF((INT32)(dwItemData), bFlag);
		if ( CModelToolApp::GetInstance()->GetItemTemplateDNF((INT32)(dwItemData), &nCurDNF) )
			SetTreeDNF_Flag(hSelectedItem, CModelToolApp::GetInstance()->GetNationalCode(), nCurDNF);
		if ( (nCurDNF & (0x1 << CModelToolApp::GetInstance()->GetNationalCode())) == FALSE )
			CModelToolApp::GetInstance()->ResetItemResourceTreeDNF(CModelToolApp::GetInstance()->GetNationalCode());
		break;
	}
}

void CResourceTree::OnCommandResourceTreeContextMenuSave()
{
	HTREEITEM hSelectedItem	= GetSelectedItem();
	if( !hSelectedItem )		return;

	if( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter() == this )
		((CResourceForm*)GetParent())->GetResourceSaver().Create( GetItemData( hSelectedItem ) );
	else if( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject() == this )
		((CResourceForm*)GetParent())->GetResourceSaver().Create( GetItemData( hSelectedItem ), eResourceSaverObject );
	else if( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem() == this )
		((CResourceForm*)GetParent())->GetResourceSaver().Create( GetItemData( hSelectedItem ), eResourceSaverItem );
}

void CResourceTree::SetTreeDNF_Flag(HTREEITEM hSelected, INT32 nNationalCode, INT32 nDNF)
{	
	BOOL bFlag = (nDNF & (0x1 << nNationalCode)) ? TRUE : FALSE;
	if( bFlag )
	{
		HTREEITEM hParent = GetParentItem(hSelected);
		if ( hParent )
		{
			CResourceTree* pCur = ( CResourceTree * )( this );
			SetItemState(hParent, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
			SetTreeDNF_Flag(hParent, nNationalCode, 0xffffffff);	
		}	
	}
	
	SetItemState(hSelected, INDEXTOSTATEIMAGEMASK((INT32) bFlag), TVIS_STATEIMAGEMASK);	
}

