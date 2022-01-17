// AgcmAdminDlgCharacterItem.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 01.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

const int MAX_ITEM_POS = 7;
const char* ITEM_POS_STRING[] =  {"Equip", "Inventory", "Bag1", "Bag2", "SalesBag", "Store", "Board"};

AgcmAdminDlgCharacterItem::AgcmAdminDlgCharacterItem()
{
	m_iResourceID = IDD_CHAR_ITEM;
	m_pfDlgProc = CharacterItemDlgProc;

	m_pfSearchItem = NULL;
	SetLastSearchItem(NULL);
}

AgcmAdminDlgCharacterItem::~AgcmAdminDlgCharacterItem()
{
	ClearItemList();
}

BOOL AgcmAdminDlgCharacterItem::SetCBSearchItem(ADMIN_CB pfCallback)
{
	m_pfSearchItem = pfCallback;
	return TRUE;
}

// 마지막으로 검색한 정보를 저장해 놓는다.
BOOL AgcmAdminDlgCharacterItem::SetLastSearchItem(stAgpdAdminCharItem* pstItem)
{
	if(pstItem)
		memcpy(&m_stLastSearchItem, pstItem, sizeof(m_stLastSearchItem));
	else
		memset(&m_stLastSearchItem, 0, sizeof(m_stLastSearchItem));

	return TRUE;
}

// 마지막으로 검색한 정보를 얻는다.
stAgpdAdminCharItem* AgcmAdminDlgCharacterItem::GetLastSearchItem()
{
	return &m_stLastSearchItem;
}

// ComboBox Index 로부터 AgpdItem.h 에서의 Item Position (Status) 값을 얻어낸다.
INT16 AgcmAdminDlgCharacterItem::GetItemPosByCBIndex(INT16 lCBIndex)
{
	INT16 lReturn = AGPDITEM_STATUS_NONE;
	switch(lCBIndex)
	{
		case 0:	// Equip
			lReturn = AGPDITEM_STATUS_EQUIP;
			break;

		case 1:	// Inventory
			lReturn = AGPDITEM_STATUS_INVENTORY;
			break;

		case 2:	// Bag1
			break;

		case 3:	// Bag2
			break;

		case 4:	// SalesBag
			lReturn = AGPDITEM_STATUS_SALESBOX_GRID;
			break;

		case 5:	// Store
			lReturn = AGPDITEM_STATUS_BANK;
			break;

		case 6:	// Board
			lReturn = AGPDITEM_STATUS_SALESBOX_BACKOUT;
			break;

		default:
			break;
	}

	return lReturn;
}

// lType 은 아이템의 위치를 가리킨다. Equip, Inventory, Bag1, Bag2, Bag, Store, Board......
BOOL AgcmAdminDlgCharacterItem::SetItem(INT16 lType, stAgpdAdminItemData* pstItem)
{
	// lType 과 pstItem 의 소유주를 비교해서, 마지막에 검색한 것 일때만 List 에 넣는다.
	//if(lType != GetLastSearchItem()->m_lType)
	//	return FALSE;

	// 이름으로 검사한다.
	if(strcmp(pstItem->m_szCharName, GetLastSearchItem()->m_szCharName) != 0)
		return FALSE;

	// Position 이 다르면 나간다.
	if(lType != GetItemPosByCBIndex(GetLastSearchItem()->m_lPos))
		return FALSE;

	// 나중엔, Slot 위치까지 검사해서 이미 있는 지 확인한다.
	//
	//
	
	// Slot 을 검사하던지, Item UniqueID 로 검사하던지 해서 해야한다.
	//
	//

	// 메모리 할당해서 새로 넣어준다.
	stAgpdAdminItemData* pstItemData = new stAgpdAdminItemData;
	memcpy(pstItemData, pstItem, sizeof(stAgpdAdminItemData));
	m_listItem.AddTail(pstItemData);

	return ShowData();
}

// Member List 에서 뽑아서 준다.
stAgpdAdminItemData* AgcmAdminDlgCharacterItem::GetItemBySlot(AgpmItemPart eItemPart)
{
	if(m_listItem.IsEmpty())
		return NULL;

	stAgpdAdminItemData* pstResult = NULL;
	AuNode<stAgpdAdminItemData*>* pcNode = m_listItem.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData()->m_lSlot == (INT16)eItemPart)
		{
			pstResult = pcNode->GetData();
			break;
		}
		pcNode = pcNode->GetNextNode();
	}

	return pstResult;
}

BOOL AgcmAdminDlgCharacterItem::ShowData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	// 화면을 비운다.
	ClearItemListView(hDlg);

	HWND hItemList = GetDlgItem(hDlg, IDC_LV_CHAR_ITEM_LIST);
	if(!hItemList)
		return FALSE;

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	// while 버전
	int iRows = 0;
	AuNode<stAgpdAdminItemData*>* pcNode = m_listItem.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData() == NULL)
			break;

		lvItem.iItem = iRows;

		// Slot
		lvItem.iSubItem = 0;
		wsprintf(szTmp, "%d", iRows+1);
		lvItem.pszText = szTmp;
		ListView_InsertItem(hItemList, &lvItem);

		// SlotName
		lvItem.iSubItem = 1;
		lvItem.pszText = pcNode->GetData()->m_szSlotName;
		ListView_SetItem(hItemList, &lvItem);

		// ItemName
		lvItem.iSubItem = 2;
		lvItem.pszText = pcNode->GetData()->m_szItemName;
		ListView_SetItem(hItemList, &lvItem);

		// ItemID
		lvItem.iSubItem = 3;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lItemID);
		lvItem.pszText = szTmp;
		ListView_SetItem(hItemList, &lvItem);

		// ItemNo
		lvItem.iSubItem = 4;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lItemNo);
		lvItem.pszText = szTmp;
		ListView_SetItem(hItemList, &lvItem);

		// Price
		lvItem.iSubItem = 5;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lPrice);
		lvItem.pszText = szTmp;
		ListView_SetItem(hItemList, &lvItem);

		// Level
		lvItem.iSubItem = 6;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lLevel);
		lvItem.pszText = szTmp;
		ListView_SetItem(hItemList, &lvItem);

		// Class
		lvItem.iSubItem = 7;
		lvItem.pszText = pcNode->GetData()->m_szClassName;
		ListView_SetItem(hItemList, &lvItem);
		
		// Race
		lvItem.iSubItem = 8;
		lvItem.pszText = pcNode->GetData()->m_szRaceName;
		ListView_SetItem(hItemList, &lvItem);

		iRows++;
		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgCharacterItem::ShowBasicData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_CHAR_ITEM_BASIC);
	if(!hBasicInfo)
		return FALSE;

	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->ShowBasicInfo(hBasicInfo);
}

BOOL AgcmAdminDlgCharacterItem::ClearItemList()
{
	if(m_listItem.IsEmpty() == FALSE)
	{
		AuNode<stAgpdAdminItemData*>* pcNode = NULL;
		AuNode<stAgpdAdminItemData*>* pcNode2 = NULL;

		pcNode = m_listItem.GetHeadNode();
		while(pcNode)
		{
			if(pcNode->GetData())
			{
				delete pcNode->GetData();

				pcNode2 = pcNode->GetNextNode();
				m_listItem.RemoveNode(pcNode);
				pcNode = pcNode2;
			}
			else
				break;
		}
	}

	return TRUE;
}

BOOL AgcmAdminDlgCharacterItem::ClearItemListView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hItemList = GetDlgItem(hDlg, IDC_LV_CHAR_ITEM_LIST);
	if(!hItemList)
		return FALSE;

	BOOL bResult = ListView_DeleteAllItems(hItemList);
	return bResult;
}

LRESULT CALLBACK AgcmAdminDlgCharacterItem::CharacterItemDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->code)
			{
				// Property Sheet
				case PSN_SETACTIVE:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->OnInitDialog(hDlg, wParam, lParam);

				case PSN_APPLY:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->CloseDlg(GetParent(hDlg));
			}
			return TRUE;
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_CB_CHAR_ITEM_POS:
				{
					if(HIWORD(wParam) == CBN_SELCHANGE)
						return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->OnItemPosChange(hDlg);

					break;
				}

				case IDC_BT_CHAR_ITEM_REFRESH:
					return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->OnRefreshBtn(hDlg);
			}
			return FALSE;

		// Property Page 용 Dialog 에는 WM_CLOSE 가 없다.
	}

	return FALSE;
}

LRESULT AgcmAdminDlgCharacterItem::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(GetInitialized())
	{
		ShowBasicData(hDlg);
		ShowData(hDlg);

		// Item 검색 패킷을 날린다.
		OnItemPosChange(hDlg);

		return TRUE;
	}

	// Property Sheet 에서 열어주므로, m_hDlg 는 NULL 이다.
	// 그래서 이렇게 초기화 할때 세팅해준다.
	m_hDlg = hDlg;

	InitBasicInfoLV(hDlg);
	InitItemInfoLV(hDlg);
	InitItemPosCB(hDlg);

	SetInitialized(TRUE);

	ShowBasicData(hDlg);
	ShowData(hDlg);

	// Item 검색 패킷을 날린다.
	OnItemPosChange(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterItem::InitBasicInfoLV(HWND hDlg)
{
	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_CHAR_ITEM_BASIC);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hBasicInfo, 0, &lvCol);

	lvCol.cx = 70;
	lvCol.pszText = "CharNo";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hBasicInfo, 1, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "AccName";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hBasicInfo, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Name";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hBasicInfo, 3, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hBasicInfo, 4, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hBasicInfo, 5, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hBasicInfo, 6, &lvCol);

	lvCol.cx = 100;
	lvCol.pszText = "UT";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hBasicInfo, 7, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hBasicInfo, 8, &lvCol);

	lvCol.cx = 120;
	lvCol.pszText = "IP";
	lvCol.iSubItem = 9;
	ListView_InsertColumn(hBasicInfo, 9, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "Status";
	lvCol.iSubItem = 10;
	ListView_InsertColumn(hBasicInfo, 10, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hBasicInfo, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

LRESULT AgcmAdminDlgCharacterItem::InitItemInfoLV(HWND hDlg)
{
	HWND hItemInfo = GetDlgItem(hDlg, IDC_LV_CHAR_ITEM_LIST);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 45;
	lvCol.pszText = "SlotNo";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hItemInfo, 0, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "SlotName";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hItemInfo, 1, &lvCol);

	lvCol.cx = 110;
	lvCol.pszText = "ItemName";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hItemInfo, 2, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "ItemID";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hItemInfo, 3, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "ItemNo";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hItemInfo, 4, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Price";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hItemInfo, 5, &lvCol);

	lvCol.cx = 30;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hItemInfo, 6, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hItemInfo, 7, &lvCol);

	lvCol.cx = 40;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hItemInfo, 8, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "";
	lvCol.iSubItem = 9;
	ListView_InsertColumn(hItemInfo, 9, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hItemInfo, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

// Combo Box Setting
LRESULT AgcmAdminDlgCharacterItem::InitItemPosCB(HWND hDlg)
{
	HWND hPos = GetDlgItem(hDlg, IDC_CB_CHAR_ITEM_POS);
	if(!hPos)
		return FALSE;

	for(int i = 0; i < MAX_ITEM_POS; ++i)
		SendMessage(hPos, CB_ADDSTRING, 0, (LPARAM)ITEM_POS_STRING[i]);

	SendMessage(hPos, CB_SETCURSEL, 0, 0);	// Default 0 (Body) 로 맞춘다.

	return TRUE;
}

// 검색할 Item 위치가 바뀐다.
// AgcmAdmin 을 통해서 검색 패킷을 날린다.
LRESULT AgcmAdminDlgCharacterItem::OnItemPosChange(HWND hDlg)
{
	if(!m_pfSearchItem)
		return FALSE;

	HWND hPos = GetDlgItem(hDlg, IDC_CB_CHAR_ITEM_POS);
	if(!hPos)
		return FALSE;

	INT8 nPos = (INT8)SendMessage(hPos, CB_GETCURSEL, 0, 0);
	if(nPos ==	CB_ERR)
		return FALSE;

	// 현재 저장되어 있는 것을 비운다.
	ClearItemList();
	ClearItemListView(hDlg);

	stAgpdAdminCharItem stItem;
	ZeroMemory(&stItem, sizeof(stItem));

	stItem.m_lPos = nPos;
	stItem.m_lCID = AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetLastSearch()->m_lObjectCID;
	strcpy(stItem.m_szCharName, AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetLastSearch()->m_szSearchName);

	if(stItem.m_lCID == 0 && strlen(stItem.m_szCharName) == 0)
		return FALSE;

	// Callback Execute
	m_pfSearchItem(&stItem, m_pcsCBClass, NULL);

	// 마지막에 Search 한 것을 새롭게 세팅해준다.
	SetLastSearchItem(&stItem);

	return TRUE;
}

// Refresh 버튼. 현재 Item Pos 으로 검색패킷을 한번 더 날리면 된다.
LRESULT AgcmAdminDlgCharacterItem::OnRefreshBtn(HWND hDlg)
{
	return OnItemPosChange(hDlg);
}