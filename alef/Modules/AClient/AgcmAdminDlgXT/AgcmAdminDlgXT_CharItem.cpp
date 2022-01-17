// AgcmAdminDlgXT_CharItem.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MESSAGE_BOX_TITLE	"ArchLord Admin - Char Item"

#define ITEM_POS_STRING_INV_1	"Inv-1"
#define ITEM_POS_STRING_INV_2	"Inv-2"
#define ITEM_POS_STRING_INV_3	"Inv-3"
#define ITEM_POS_STRING_INV_4	"Inv-4"
#define ITEM_POS_STRING_BANK	"Bank"

#define TEXTURE_EXTENSION		".png"


/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_CharItem dialog


AgcmAdminDlgXT_CharItem::AgcmAdminDlgXT_CharItem(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_CharItem::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_CharItem)
	m_szItemDesc = _T("");
	m_szBankMoney = _T("0");
	m_szInvenMoney = _T("0");
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;

	memset(m_szCharName, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));
}


void AgcmAdminDlgXT_CharItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_CharItem)
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_7_4, m_csGridBtn_7_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_7_3, m_csGridBtn_7_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_7_2, m_csGridBtn_7_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_7_1, m_csGridBtn_7_1);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_6_4, m_csGridBtn_6_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_6_3, m_csGridBtn_6_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_6_2, m_csGridBtn_6_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_6_1, m_csGridBtn_6_1);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_5_4, m_csGridBtn_5_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_5_3, m_csGridBtn_5_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_5_2, m_csGridBtn_5_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_5_1, m_csGridBtn_5_1);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_4_4, m_csGridBtn_4_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_4_3, m_csGridBtn_4_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_4_2, m_csGridBtn_4_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_4_1, m_csGridBtn_4_1);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_3_4, m_csGridBtn_3_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_3_3, m_csGridBtn_3_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_3_2, m_csGridBtn_3_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_3_1, m_csGridBtn_3_1);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_2_4, m_csGridBtn_2_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_2_3, m_csGridBtn_2_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_2_2, m_csGridBtn_2_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_2_1, m_csGridBtn_2_1);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_1_4, m_csGridBtn_1_4);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_1_3, m_csGridBtn_1_3);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_1_2, m_csGridBtn_1_2);
	DDX_Control(pDX, IDC_B_CHAR_ITEM_GRID_1_1, m_csGridBtn_1_1);
	DDX_Control(pDX, IDC_S_CHAR_VIEW_CHARNAME, m_csCharNameStatic);
	DDX_Control(pDX, IDC_CB_CHAR_ITEM_INDEX, m_csItemPosCB);
	DDX_Text(pDX, IDC_E_CHAR_ITEM_DESC, m_szItemDesc);
	DDX_Text(pDX, IDC_E_CHAR_ITEM_BANK_MONEY, m_szBankMoney);
	DDX_Text(pDX, IDC_E_CHAR_ITEM_INVEN_MONEY, m_szInvenMoney);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_CharItem, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_CharItem)
	ON_CBN_SELCHANGE(IDC_CB_CHAR_ITEM_INDEX, OnSelchangeCbCharItemIndex)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_REFRESH, OnBCharItemRefresh)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_1_1, OnBCharItemGrid1_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_1_2, OnBCharItemGrid1_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_1_3, OnBCharItemGrid1_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_1_4, OnBCharItemGrid1_4)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_2_1, OnBCharItemGrid2_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_2_2, OnBCharItemGrid2_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_2_3, OnBCharItemGrid2_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_2_4, OnBCharItemGrid2_4)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_3_1, OnBCharItemGrid3_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_3_2, OnBCharItemGrid3_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_3_3, OnBCharItemGrid3_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_3_4, OnBCharItemGrid3_4)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_4_1, OnBCharItemGrid4_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_4_2, OnBCharItemGrid4_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_4_3, OnBCharItemGrid4_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_4_4, OnBCharItemGrid4_4)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_5_1, OnBCharItemGrid5_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_5_2, OnBCharItemGrid5_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_5_3, OnBCharItemGrid5_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_5_4, OnBCharItemGrid5_4)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_6_1, OnBCharItemGrid6_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_6_2, OnBCharItemGrid6_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_6_3, OnBCharItemGrid6_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_6_4, OnBCharItemGrid6_4)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_7_1, OnBCharItemGrid7_1)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_7_2, OnBCharItemGrid7_2)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_7_3, OnBCharItemGrid7_3)
	ON_BN_CLICKED(IDC_B_CHAR_ITEM_GRID_7_4, OnBCharItemGrid7_4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_CharItem message handlers

BOOL AgcmAdminDlgXT_CharItem::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::OpenDlg(INT nShowCmd /* = SW_SHOW */)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
	{
		//CDialog::Create(IDD, m_pParentWnd);
		CDialog::Create(IDD, NULL);
	}

	ShowWindow(nShowCmd);

	ClearItemGrid();
	ShowItemGrid();

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::IsInitialized()
{
	return m_bInitialized;
}

void AgcmAdminDlgXT_CharItem::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_CharItem::Unlock()
{
	m_csLock.Unlock();
}




BOOL AgcmAdminDlgXT_CharItem::SetCharName(CHAR* szCharName)
{
	if(!szCharName)
		return FALSE;

	strncpy(m_szCharName, szCharName, AGPACHARACTER_MAX_ID_STRING);

	if(m_bInitialized)
	{
		m_csCharNameStatic.SetWindowText(m_szCharName);
	}

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::IsSearchCharacter(CHAR* szCharName)
{
	if(!szCharName)
		return FALSE;

	return strcmp(m_szCharName, szCharName) == 0 ? TRUE : FALSE;
}

BOOL AgcmAdminDlgXT_CharItem::SetItem(INT16 lType, stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return FALSE;

	// 먼저 검색한 캐릭터의 것인지.
	if(IsSearchCharacter(pstItem->m_szCharName) == FALSE)
		return FALSE;

	BOOL bRedraw = FALSE;

	// 장착, 인벤 과 뱅크를 구별해야 한다.
	if(pstItem->m_lPos == AGPDITEM_STATUS_EQUIP ||
		pstItem->m_lPos == AGPDITEM_STATUS_INVENTORY)
	{
		bRedraw = TRUE;

		// 같은 위치에 아이템을 발견하면...
		stAgpdAdminItemData* pstItemData = GetItem(pstItem);
		if(pstItemData)
		{
			// 걍 메모리 복사해버린다.
			memcpy(pstItemData, pstItem, sizeof(stAgpdAdminItemData));
		}
		else
		{
			// 아니면 새로 추가
			pstItemData = new stAgpdAdminItemData;
			memcpy(pstItemData, pstItem, sizeof(stAgpdAdminItemData));
			m_listItem.push_back(pstItemData);
		}
	}
	else if(pstItem->m_lPos == AGPDITEM_STATUS_BANK)
	{
		bRedraw = TRUE;

		// 같은 위치에 아이템을 발견하면...
		stAgpdAdminItemData* pstItemData = GetItemFromBank(pstItem);
		if(pstItemData)
		{
			// 걍 메모리 복사해버린다.
			memcpy(pstItemData, pstItem, sizeof(stAgpdAdminItemData));
		}

		// 이제 추가한다.
		pstItemData = new stAgpdAdminItemData;
		memcpy(pstItemData, pstItem, sizeof(stAgpdAdminItemData));
		m_listBankItem.push_back(pstItemData);
	}

	if(bRedraw)
	{
		ClearItemGrid();
		ShowItemGrid();
	}

	return TRUE;
}

stAgpdAdminItemData* AgcmAdminDlgXT_CharItem::GetItem(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return NULL;

	list<stAgpdAdminItemData*>::iterator iterData = m_listItem.begin();
	while(iterData != m_listItem.end())
	{
		if(*iterData)
		{
			// Position 과 Grid 를 비교한다.
			if((*iterData)->m_lPos == pstItem->m_lPos &&
				(*iterData)->m_lLayer == pstItem->m_lLayer &&
				(*iterData)->m_lRow == pstItem->m_lRow &&
				(*iterData)->m_lCol == pstItem->m_lCol)
				return (*iterData);
		}

		iterData++;
	}

	return NULL;
}

stAgpdAdminItemData* AgcmAdminDlgXT_CharItem::GetItemFromBank(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return NULL;

	list<stAgpdAdminItemData*>::iterator iterData = m_listBankItem.begin();
	while(iterData != m_listBankItem.end())
	{
		if(*iterData)
		{
			// Position 과 Grid 를 비교한다.
			if((*iterData)->m_lPos == pstItem->m_lPos &&
				(*iterData)->m_lLayer == pstItem->m_lLayer &&
				(*iterData)->m_lRow == pstItem->m_lRow &&
				(*iterData)->m_lCol == pstItem->m_lCol)
				return (*iterData);
		}

		iterData++;
	}

	return NULL;
}

// 해당 위치의 아이템을 비운다.
BOOL AgcmAdminDlgXT_CharItem::DeleteItem(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return FALSE;

	BOOL bResult = FALSE;

	list<stAgpdAdminItemData*>::iterator iterData = m_listItem.begin();
	while(iterData != m_listItem.end())
	{
		if(*iterData)
		{
			// Position 과 Grid 를 비교한다.
			if((*iterData)->m_lPos == pstItem->m_lPos &&
				(*iterData)->m_lLayer == pstItem->m_lLayer &&
				(*iterData)->m_lRow == pstItem->m_lRow &&
				(*iterData)->m_lCol == pstItem->m_lCol)
			{
				bResult = TRUE;
				delete *iterData;
				m_listItem.erase(iterData);
				
				break;
			}
		}

		iterData++;
	}

	return bResult;
}

BOOL AgcmAdminDlgXT_CharItem::DeleteItemFromBank(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return FALSE;

	BOOL bResult = FALSE;

	list<stAgpdAdminItemData*>::iterator iterData = m_listBankItem.begin();
	while(iterData != m_listBankItem.end())
	{
		if(*iterData)
		{
			// Position 과 Grid 를 비교한다.
			if((*iterData)->m_lPos == pstItem->m_lPos &&
				(*iterData)->m_lLayer == pstItem->m_lLayer &&
				(*iterData)->m_lRow == pstItem->m_lRow &&
				(*iterData)->m_lCol == pstItem->m_lCol)
			{
				bResult = TRUE;
				delete *iterData;
				m_listBankItem.erase(iterData);
				
				break;
			}
		}

		iterData++;
	}

	return bResult;
}

BOOL AgcmAdminDlgXT_CharItem::SetInvenMoney(INT32 lMoney)
{
	if(lMoney < 0)
		return FALSE;

	m_szInvenMoney.Format("%d", lMoney);

	if(m_bInitialized)
		UpdateData(FALSE);

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::SetBankMoney(INT32 lMoney)
{
	if(lMoney < 0)
		return FALSE;

	m_szBankMoney.Format("%d", lMoney);

	if(m_bInitialized)
		UpdateData(FALSE);

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::SetItemDesc(LPCTSTR szDesc)
{
	if(!szDesc)
		return FALSE;

	m_szItemDesc = szDesc;

	if(m_bInitialized)
		UpdateData(FALSE);

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::ShowItemGrid()
{
	if(!m_bInitialized)
		return FALSE;

	if(m_listItem.size() == 0 && m_listBankItem.size() == 0)
		return FALSE;

	INT32 lItemPos = GetItemPosByComboBox();
	if(lItemPos == AGPDITEM_STATUS_EQUIP ||
		lItemPos == AGPDITEM_STATUS_INVENTORY)		// 이큅이나 인벤이면 하나로 처리
	{
		INT32 lInvenPos = GetInventoryIndexByComboBox();

		//CString szItemName = _T("");
		CButton* pcsButton = NULL;
		list<stAgpdAdminItemData*>::iterator iterData = m_listItem.begin();
		while(iterData != m_listItem.end())
		{
			if(!*iterData)
				break;

			if((*iterData)->m_lPos != AGPDITEM_STATUS_EQUIP && (*iterData)->m_lLayer != lInvenPos)
			{
				iterData++;
				continue;
			}

			pcsButton = GetGridBtnByPos(*iterData);
			if(pcsButton)
			{
				//szItemName = (*iterData)->m_szItemName;
				//szItemName.Replace(' ', '\n');	// 이름 중간의 공백은 한줄띄기로 바꿔준다.
				//pcsButton->SetWindowText((LPCTSTR)szItemName);

				((CButtonIncludeItemData*)pcsButton)->m_pstItemData = *iterData;

				DrawItemTexture(*iterData, pcsButton);
			}

			iterData++;
		}
	}
	else if(lItemPos == AGPDITEM_STATUS_BANK)		// 뱅크는 따로처리
	{
		list<stAgpdAdminItemData*>::iterator iterData = m_listBankItem.begin();
		while(iterData != m_listBankItem.end())
		{		
			iterData++;
		}
	}

	return TRUE;
}


BOOL AgcmAdminDlgXT_CharItem::ClearItemGrid()
{
	if(!m_bInitialized)
		return FALSE;

	m_csGridBtn_7_4.InitData();
	m_csGridBtn_7_3.InitData();
	m_csGridBtn_7_2.InitData();
	m_csGridBtn_7_1.InitData();
	m_csGridBtn_6_4.InitData();
	m_csGridBtn_6_3.InitData();
	m_csGridBtn_6_2.InitData();
	m_csGridBtn_6_1.InitData();
	m_csGridBtn_5_4.InitData();
	m_csGridBtn_5_3.InitData();
	m_csGridBtn_5_2.InitData();
	m_csGridBtn_5_1.InitData();
	m_csGridBtn_4_4.InitData();
	m_csGridBtn_4_3.InitData();
	m_csGridBtn_4_2.InitData();
	m_csGridBtn_4_1.InitData();
	m_csGridBtn_3_4.InitData();
	m_csGridBtn_3_3.InitData();
	m_csGridBtn_3_2.InitData();
	m_csGridBtn_3_1.InitData();
	m_csGridBtn_2_4.InitData();
	m_csGridBtn_2_3.InitData();
	m_csGridBtn_2_2.InitData();
	m_csGridBtn_2_1.InitData();
	m_csGridBtn_1_4.InitData();
	m_csGridBtn_1_3.InitData();
	m_csGridBtn_1_2.InitData();
	m_csGridBtn_1_1.InitData();

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::ClearItemList()
{
	if(m_listItem.size() == 0)
		return FALSE;

	list<stAgpdAdminItemData*>::iterator iterData = m_listItem.begin();
	while(iterData != m_listItem.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listItem.clear();
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::ClearBankItemList()
{
	if(m_listBankItem.size() == 0)
		return FALSE;

	list<stAgpdAdminItemData*>::iterator iterData = m_listBankItem.begin();
	while(iterData != m_listBankItem.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listBankItem.clear();

	return TRUE;
}

INT32 AgcmAdminDlgXT_CharItem::GetItemPosByComboBox()
{
	INT32 lResult = 0;

	INT32 lIndex = m_csItemPosCB.GetCurSel();
	if(lIndex != CB_ERR)
	{
		CString szItemPos = _T("");
		m_csItemPosCB.GetLBText(lIndex, szItemPos);

		if(szItemPos.Compare(ITEM_POS_STRING_INV_1) == 0 ||
			szItemPos.Compare(ITEM_POS_STRING_INV_2) == 0 ||
			szItemPos.Compare(ITEM_POS_STRING_INV_3) == 0 ||
			szItemPos.Compare(ITEM_POS_STRING_INV_4) == 0)
			lResult = AGPDITEM_STATUS_INVENTORY;
		else if(szItemPos.Compare(ITEM_POS_STRING_BANK) == 0)
			lResult = AGPDITEM_STATUS_BANK;
	}

	return lResult;
}

INT32 AgcmAdminDlgXT_CharItem::GetInventoryIndexByComboBox()
{
	INT32 lResult = 0;

	INT32 lIndex = m_csItemPosCB.GetCurSel();
	if(lIndex != CB_ERR)
	{
		CString szItemPos = _T("");
		m_csItemPosCB.GetLBText(lIndex, szItemPos);

		if(szItemPos.Compare(ITEM_POS_STRING_INV_1) == 0)
			lResult = 0;
		if(szItemPos.Compare(ITEM_POS_STRING_INV_2) == 0)
			lResult = 1;
		if(szItemPos.Compare(ITEM_POS_STRING_INV_3) == 0)
			lResult = 2;
		if(szItemPos.Compare(ITEM_POS_STRING_INV_4) == 0)
			lResult = 3;
	}

	return lResult;
}

BOOL AgcmAdminDlgXT_CharItem::IsBankItem(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return FALSE;

	return pstItem->m_lPos == AGPDITEM_STATUS_BANK ? TRUE : FALSE;
}

// 이함수 만들기 왕 거시기다~~~
// 버튼 28개를 다 구별시켜줘야해~~~
// 왕왕왕왕왕~~~ 거시기야~
CButton* AgcmAdminDlgXT_CharItem::GetGridBtnByPos(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return NULL;

	CButton* pcsButton = NULL;
	switch(pstItem->m_lPos)
	{
		case AGPDITEM_STATUS_EQUIP:
			if(pstItem->m_lRow == 0)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_1_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_1_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_1_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_1_4;
			}
			else if(pstItem->m_lRow == 1)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_2_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_2_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_2_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_2_4;
			}
			else if(pstItem->m_lRow == 2)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_3_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_3_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_3_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_3_4;
			}
			break;

		case AGPDITEM_STATUS_INVENTORY:
			if(pstItem->m_lRow == 0)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_4_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_4_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_4_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_4_4;
			}
			else if(pstItem->m_lRow == 1)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_5_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_5_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_5_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_5_4;
			}
			else if(pstItem->m_lRow == 2)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_6_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_6_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_6_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_6_4;
			}
			else if(pstItem->m_lRow == 3)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_7_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_7_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_7_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_7_4;
			}
			break;

		case AGPDITEM_STATUS_BANK:
			if(pstItem->m_lRow == 0)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_1_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_1_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_1_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_1_4;
			}
			else if(pstItem->m_lRow == 1)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_2_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_2_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_2_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_2_4;
			}
			else if(pstItem->m_lRow == 2)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_3_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_3_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_3_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_3_4;
			}
			else if(pstItem->m_lRow == 3)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_4_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_4_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_4_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_4_4;
			}
			else if(pstItem->m_lRow == 4)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_5_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_5_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_5_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_5_4;
			}
			else if(pstItem->m_lRow == 5)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_6_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_6_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_6_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_6_4;
			}
			else if(pstItem->m_lRow == 6)
			{
				if(pstItem->m_lCol == 0)
					pcsButton = &m_csGridBtn_7_1;
				else if(pstItem->m_lCol == 1)
					pcsButton = &m_csGridBtn_7_2;
				else if(pstItem->m_lCol == 2)
					pcsButton = &m_csGridBtn_7_3;
				else if(pstItem->m_lCol == 3)
					pcsButton = &m_csGridBtn_7_4;
			}
			break;
	}

	return pcsButton;
}

BOOL AgcmAdminDlgXT_CharItem::DrawItemTexture(stAgpdAdminItemData* pstItem, CButton* pcsButton /* = NULL */)
{
	if(!pstItem)
		return FALSE;

	if(!pcsButton)
		pcsButton = GetGridBtnByPos(pstItem);

	if(!pcsButton)
		return FALSE;

	stAgpdAdminItemTemplate* pstItemTemplate = AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->GetItemTemplate(pstItem->m_szItemName);
	if(!pstItemTemplate)
		return FALSE;

	CString szFullPath = _T("");
	szFullPath = TEXTURE_ITEM_BASIC_PATH;
	szFullPath += pstItemTemplate->m_szTexturePath;
	szFullPath += TEXTURE_EXTENSION;

	if(_access((LPCTSTR)szFullPath, 0) == -1)
	{
		// 텍스쳐가 없다. 걍 아이템 이름만 쓰고 나간다.
		// 스택이 여러개 되어 있으면 갯수도 보여준다.
		if(pstItem->m_lCount > 1)
		{
			szFullPath.Format("%s (%d)", pstItem->m_szItemName, pstItem->m_lCount);
			pcsButton->SetWindowText((LPCTSTR)szFullPath);
		}
		else
			pcsButton->SetWindowText(pstItem->m_szItemName);
		
		return TRUE;
	}

	CDC* cdc = pcsButton->GetDC();
	if(!cdc)
		return FALSE;

	Graphics graphics(cdc->m_hDC);

	if(szFullPath.IsEmpty())	// 지우기
	{
		SolidBrush whiteBrush(Color(255, 0, 0, 0));
		CRect cRect;
		pcsButton->GetClientRect(&cRect);

		graphics.FillRectangle(&whiteBrush, cRect.left, cRect.top, cRect.Width(), cRect.Height());
	}
	else
	{
		WCHAR* wszFullPath = new WCHAR[szFullPath.GetLength() + 1];
		memset(wszFullPath, 0, sizeof(WCHAR) * (szFullPath.GetLength() + 1));
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szFullPath, -1, wszFullPath, szFullPath.GetLength());

		Image image(wszFullPath);
		graphics.DrawImage(&image, 0, 0, image.GetWidth(), image.GetHeight());

		delete [] wszFullPath;
	}

	return TRUE;
}

BOOL AgcmAdminDlgXT_CharItem::MakeItemDesc(stAgpdAdminItemData* pstItem)
{
	if(!pstItem)
		return FALSE;

	if(!m_bInitialized)
		return FALSE;

	SetItemDesc((LPCTSTR)GetItemDescription(pstItem));

	return TRUE;
}

CString AgcmAdminDlgXT_CharItem::GetItemDescription(stAgpdAdminItemData* pstItem)
{
	CString szItem = _T("");

	if(!pstItem)
		return szItem;

	CString szTmp = _T("");

	szItem += "이름 : ";
	szItem += pstItem->m_szItemName;
	if(pstItem->m_lCount > 1)
		szTmp.Format(" (%d 개)", pstItem->m_lCount);

	if(pstItem->m_lNumPhysicalConvert > 0)
	{
		szTmp.Format(" + %d\r\n", pstItem->m_lNumPhysicalConvert);
		szItem += szTmp;
	}

	if(pstItem->m_lNumSocket > 0)
	{
		szTmp.Format(" [%d/%d]", pstItem->m_lNumConvertedSocket, pstItem->m_lNumSocket);
		szItem += szTmp;
	}

	szItem += "\r\n";

	szItem += "NPC 판매가격 : ";
	szTmp.Format("%d\r\n", pstItem->m_lPrice);
	szItem += szTmp;

	szItem += "랭크 : ";
	szTmp.Format("%d\r\n", pstItem->m_lRank);
	szItem += szTmp;

	switch(pstItem->m_lKind)
	{
		case AGPMITEM_EQUIP_KIND_WEAPON:
		{
			szItem += "무기 타입 : ";
			szTmp.Format("%s/%d\r\n", pstItem->m_szWeaponType, pstItem->m_lAttackRange);
			szItem += szTmp;

			szItem += "물리공격력 : ";
			szTmp.Format("%d - %d\r\n", pstItem->m_lPhyMinDmg, pstItem->m_lPhyMaxDmg);
			szItem += szTmp;

			szItem += "공격 속도 : ";
			if(pstItem->m_lAttackSpeed < 50)
				szItem += "매우 느림";
			else if(pstItem->m_lAttackSpeed < 60)
				szItem += "느림";
			else if(pstItem->m_lAttackSpeed < 70)
				szItem += "보통";
			else if(pstItem->m_lAttackSpeed < 80)
				szItem += "빠름";
			else
				szItem += "매우 빠름";
			
			szTmp.Format("(%d)\r\n", pstItem->m_lAttackSpeed);
			szItem += szTmp;

			break;
		}

		case AGPMITEM_EQUIP_KIND_ARMOUR:
		{
			szItem += "물리 방어력 : ";
			szTmp.Format("%d\r\n", pstItem->m_lPhyDefense);
			szItem += szTmp;

			break;
		}

		case AGPMITEM_EQUIP_KIND_SHIELD:
		{
			szItem += "물리 방어력 : ";
			szTmp.Format("%d\r\n", pstItem->m_lPhyDefense);
			szItem += szTmp;

			szItem += "블럭 : ";
			szTmp.Format("%d\r\n", pstItem->m_lPhyDefenseRate);
			szItem += szTmp;

			break;
		}
	}

	szItem += "내구도 : ";
	szTmp.Format("%d %%\r\n", pstItem->m_lDurability);
	szItem += szTmp;

	// 제한 사항 시작
	szItem += "\r\n[장착조건]\r\n";

	if(strlen(pstItem->m_szRaceName) > 0)
	{
		szItem += pstItem->m_szRaceName;

		if(strlen(pstItem->m_szClassName) > 0 || strlen(pstItem->m_szGenderName) > 0)
			szItem += ", ";
		else
			szItem += "\r\n";
	}

	if(strlen(pstItem->m_szClassName) > 0)
	{
		szItem += pstItem->m_szClassName;

		if(strlen(pstItem->m_szGenderName) > 0)
			szItem += ", ";
		else
			szItem += "\r\n";
	}

	if(strlen(pstItem->m_szGenderName) > 0)
	{
		szItem += pstItem->m_szGenderName;
		szItem += "\r\n";
	}

	if(pstItem->m_lNeedLevel > 0)
	{
		szTmp.Format("필요레벨 : %d\r\n", pstItem->m_lNeedLevel);
		szItem += szTmp;
	}

	if(pstItem->m_lNeedStr > 0)
	{
		szTmp.Format("필요 Str : %d\r\n", pstItem->m_lNeedStr);
		szItem += szTmp;
	}

	if(pstItem->m_lNeedCon > 0)
	{
		szTmp.Format("필요 Con : %d\r\n", pstItem->m_lNeedCon);
		szItem += szTmp;
	}

	if(pstItem->m_lNeedInt > 0)
	{
		szTmp.Format("필요 Int : %d\r\n", pstItem->m_lNeedInt);
		szItem += szTmp;
	}

	if(pstItem->m_lNeedWis > 0)
	{
		szTmp.Format("필요 Wis : %d\r\n", pstItem->m_lNeedWis);
		szItem += szTmp;
	}

	if(pstItem->m_lNeedDex > 0)
	{
		szTmp.Format("필요 Dex : %d\r\n", pstItem->m_lNeedDex);
		szItem += szTmp;
	}

	// 2004.04.06. Convert Description 추가
	szItem += GetItemDescriptionConvertHistory(pstItem);
	szItem += GetItemDescriptionConvertAttrInfo(pstItem);

	return szItem;
}

// 2004.04.06. Convert Description 추가
// 2004.09.08. Copy & Paste & Modify
CString AgcmAdminDlgXT_CharItem::GetItemDescriptionConvertHistory(stAgpdAdminItemData* pstItem)
{
	CString szHistory = _T("");

	if(!pstItem)
		return szHistory;

	if(pstItem->m_lNumPhysicalConvert == 0)
		return szHistory;

	stAgpdAdminItemConvertHistory* pstHistory = &pstItem->m_stConvertHistory;

	szHistory += "\r\n[개조 내역]\r\n";	// 두줄 띄고 시작
	CString szTmp = _T("");

	if(pstHistory->m_lNumMagicAttr)
	{
		szTmp.Format("마법 정령석 : + %d\r\n", pstHistory->m_lNumMagicAttr);
		szHistory += szTmp;
	}

	if(pstHistory->m_lNumWaterAttr)
	{
		szTmp.Format("물 정령석 : + %d\r\n", pstHistory->m_lNumWaterAttr);
		szHistory += szTmp;
	}

	if(pstHistory->m_lNumFireAttr)
	{
		szTmp.Format("불 정령석 : + %d\r\n", pstHistory->m_lNumFireAttr);
		szHistory += szTmp;
	}

	if(pstHistory->m_lNumAirAttr)
	{
		szTmp.Format("공기 정령석 : + %d\r\n", pstHistory->m_lNumAirAttr);
		szHistory += szTmp;
	}

	if(pstHistory->m_lNumEarthAttr)
	{
		szTmp.Format("땅 정령석 : + %d\r\n", pstHistory->m_lNumEarthAttr);
		szHistory += szTmp;
	}

	return szHistory;
}

// 2004.04.06. Convert Description 추가
// 2004.09.08. Copy & Paste & Modify
CString AgcmAdminDlgXT_CharItem::GetItemDescriptionConvertAttrInfo(stAgpdAdminItemData* pstItem)
{
	CString szAttrInfo = _T("");

	if(!pstItem)
		return szAttrInfo;

	if(pstItem->m_lNumPhysicalConvert == 0)
		return szAttrInfo;

	szAttrInfo += "\r\n[개조 속성]\r\n";

	switch(pstItem->m_lKind)
	{
		case AGPMITEM_EQUIP_KIND_WEAPON:
			szAttrInfo += GetItemDescriptionConvertWeaponAttrInfo(pstItem);
			break;

		case AGPMITEM_EQUIP_KIND_ARMOUR:
			szAttrInfo += GetItemDescriptionConvertArmourAttrInfo(pstItem);
			break;

		case AGPMITEM_EQUIP_KIND_SHIELD:
			szAttrInfo += GetItemDescriptionConvertShieldAttrInfo(pstItem);
			break;
	}

	return szAttrInfo;
}

// 2004.04.06. Convert Description 추가
// 2004.09.08. Copy & Past & Modify
CString AgcmAdminDlgXT_CharItem::GetItemDescriptionConvertWeaponAttrInfo(stAgpdAdminItemData* pstItem)
{
	CString szAttrInfo = _T("");

	if(!pstItem)
		return szAttrInfo;

	stAgpdAdminItemConvertWeaponAttrInfo* pstAttrInfo = &pstItem->m_stConvertWeaponAttrInfo;
	if(!pstAttrInfo)
		return szAttrInfo;

	CString szTmp = _T("");

	if(pstAttrInfo->m_lTotalDmgPhysical)
	{
		szTmp.Format("물리 공격 : + %d\r\n", pstAttrInfo->m_lTotalDmgPhysical);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalAttackSpeed)
	{
		szTmp.Format("공격 속도 : + %d\r\n", pstAttrInfo->m_lTotalAttackSpeed);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalAP)
	{
		szTmp.Format("명중률 : + %d\r\n", pstAttrInfo->m_lTotalAP);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalHPRecovery)
	{
		szTmp.Format("생명회복속도 상승 : + %d\r\n", pstAttrInfo->m_lTotalHPRecovery);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalMPRecovery)
	{
		szTmp.Format("마나회복속도 상승 : + %d\r\n", pstAttrInfo->m_lTotalMPRecovery);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalSPRecovery)
	{
		szTmp.Format("스테미너회복속도 상승 : + %d\r\n", pstAttrInfo->m_lTotalSPRecovery);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lNumConvertMagic)
	{
		szTmp.Format("마법 공격력(%d) : %d ~ %d\r\n",
			pstAttrInfo->m_lNumConvertMagic, pstAttrInfo->m_lDmgMagicMin, pstAttrInfo->m_lDmgMagicMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lNumConvertFire)
	{
		szTmp.Format("불 공격력(%d) : %d ~ %d\r\n",
			pstAttrInfo->m_lNumConvertFire, pstAttrInfo->m_lDmgMagicMin, pstAttrInfo->m_lDmgFireMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lNumConvertWater)
	{
		szTmp.Format("물 공격력(%d) : %d ~ %d\r\n",
			pstAttrInfo->m_lNumConvertWater, pstAttrInfo->m_lDmgWaterMin, pstAttrInfo->m_lDmgWaterMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lNumConvertAir)
	{
		szTmp.Format("공기 공격력(%d) : %d ~ %d\r\n",
			pstAttrInfo->m_lNumConvertAir, pstAttrInfo->m_lDmgAirMin, pstAttrInfo->m_lDmgAirMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lNumConvertEarth)
	{
		szTmp.Format("땅 공격력(%d) : %d ~ %d\r\n",
			pstAttrInfo->m_lNumConvertEarth, pstAttrInfo->m_lDmgEarthMin, pstAttrInfo->m_lDmgEarthMax);
		szAttrInfo += szTmp;
	}

	return szAttrInfo;
}

// 2004.04.06. Convert Description 추가
// 2004.09.08. Copy & Paste & Modify
CString AgcmAdminDlgXT_CharItem::GetItemDescriptionConvertArmourAttrInfo(stAgpdAdminItemData* pstItem)
{
	CString szAttrInfo = _T("");

	if(!pstItem)
		return szAttrInfo;

	stAgpdAdminItemConvertArmourAttrInfo* pstAttrInfo = &pstItem->m_stConvertArmourAttrInfo;
	if(!pstAttrInfo)
		return szAttrInfo;

	CString szTmp = _T("");

	if(pstAttrInfo->m_lTotalDefPhysical)
	{
		szTmp.Format("물리 방어 : + %d\r\n", pstAttrInfo->m_lTotalDefPhysical);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalHPMax)
	{
		szTmp.Format("생명 : + %d\r\n", pstAttrInfo->m_lTotalHPMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalMPMax)
	{
		szTmp.Format("마나 : + %d\r\n", pstAttrInfo->m_lTotalMPMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalSPMax)
	{
		szTmp.Format("스태미너 : + %d\r\n", pstAttrInfo->m_lTotalSPMax);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefMagic)
	{
		szTmp.Format("마법 방어 : + %d\r\n", pstAttrInfo->m_lDefMagic);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefFire)
	{
		szTmp.Format("불 방어 : + %d\r\n", pstAttrInfo->m_lDefFire);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefWater)
	{
		szTmp.Format("물 방어 : + %d\r\n", pstAttrInfo->m_lDefWater);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefAir)
	{
		szTmp.Format("공기 방어 : + %d\r\n", pstAttrInfo->m_lDefAir);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefEarth)
	{
		szTmp.Format("땅 방어 : + %d\r\n", pstAttrInfo->m_lDefEarth);
		szAttrInfo += szTmp;
	}

	return szAttrInfo;
}

// 2004.04.06. Convert Description 추가
// 2004.09.08. Copy & Paste & Modify
CString AgcmAdminDlgXT_CharItem::GetItemDescriptionConvertShieldAttrInfo(stAgpdAdminItemData* pstItem)
{
	CString szAttrInfo = _T("");

	if(!pstItem)
		return szAttrInfo;

	stAgpdAdminItemConvertShieldAttrInfo* pstAttrInfo = &pstItem->m_stConvertShieldAttrInfo;
	if(!pstAttrInfo)
		return szAttrInfo;

	CString szTmp = _T("");

	if(pstAttrInfo->m_lTotalDefPhysical)
	{
		szTmp.Format("물리 방어 : + %d\r\n", pstAttrInfo->m_lTotalDefPhysical);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalDefRatePhysical)
	{
		szTmp.Format("블록 : + %d\r\n", pstAttrInfo->m_lTotalDefRatePhysical);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalHPRecovery)
	{
		szTmp.Format("생명회복속도 상승 : + %d\r\n", pstAttrInfo->m_lTotalHPRecovery);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalMPRecovery)
	{
		szTmp.Format("마나회복속도 상승 : + %d\r\n", pstAttrInfo->m_lTotalMPRecovery);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lTotalSPRecovery)
	{
		szTmp.Format("스테미너회복속도 상승 : + %d\r\n", pstAttrInfo->m_lTotalSPRecovery);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefMagic)
	{
		szTmp.Format("마법 방어 : + %d\r\n", pstAttrInfo->m_lDefMagic);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefFire)
	{
		szTmp.Format("불 방어 : + %d\r\n", pstAttrInfo->m_lDefFire);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefWater)
	{
		szTmp.Format("물 방어 : + %d\r\n", pstAttrInfo->m_lDefWater);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefAir)
	{
		szTmp.Format("공기 방어 : + %d\r\n", pstAttrInfo->m_lDefAir);
		szAttrInfo += szTmp;
	}

	if(pstAttrInfo->m_lDefEarth)
	{
		szTmp.Format("땅 방어 : + %d\r\n", pstAttrInfo->m_lDefEarth);
		szAttrInfo += szTmp;
	}

	return szAttrInfo;
}

BOOL AgcmAdminDlgXT_CharItem::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_bInitialized = TRUE;

	m_csCharNameStatic.SetWindowText(m_szCharName);

	m_csItemPosCB.AddString(ITEM_POS_STRING_INV_1);
	m_csItemPosCB.AddString(ITEM_POS_STRING_INV_2);
	m_csItemPosCB.AddString(ITEM_POS_STRING_INV_3);
	m_csItemPosCB.AddString(ITEM_POS_STRING_INV_4);
	m_csItemPosCB.AddString(ITEM_POS_STRING_BANK);
	m_csItemPosCB.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAdminDlgXT_CharItem::OnSelchangeCbCharItemIndex() 
{
	// TODO: Add your control notification handler code here

	ClearItemGrid();
	SetItemDesc("");

	ShowItemGrid();
}

void AgcmAdminDlgXT_CharItem::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class

	ClearItemList();
	ClearBankItemList();
	
	CDialog::PostNcDestroy();
}

void AgcmAdminDlgXT_CharItem::OnBCharItemRefresh() 
{
	// TODO: Add your control notification handler code here
	
	ClearItemList();
	ClearBankItemList();

	// 다시 Search
	AgcmAdminDlgXT_Manager::Instance()->GetSearchDlg()->ProcessSearch(m_szCharName);
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid1_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_1_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_1_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid1_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_1_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_1_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid1_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_1_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_1_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid1_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_1_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_1_4.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid2_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_2_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_2_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid2_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_2_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_2_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid2_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_2_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_2_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid2_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_2_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_2_4.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid3_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_3_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_3_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid3_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_3_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_3_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid3_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_3_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_3_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid3_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_3_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_3_4.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid4_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_4_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_4_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid4_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_4_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_4_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid4_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_4_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_4_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid4_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_4_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_4_4.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid5_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_5_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_5_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid5_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_5_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_5_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid5_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_5_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_5_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid5_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_5_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_5_4.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid6_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_6_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_6_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid6_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_6_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_6_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid6_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_6_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_6_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid6_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_6_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_6_4.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid7_1() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_7_1.m_pstItemData)
		MakeItemDesc(m_csGridBtn_7_1.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid7_2() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_7_2.m_pstItemData)
		MakeItemDesc(m_csGridBtn_7_2.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid7_3() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_7_3.m_pstItemData)
		MakeItemDesc(m_csGridBtn_7_3.m_pstItemData);
	else
		SetItemDesc("");
}

void AgcmAdminDlgXT_CharItem::OnBCharItemGrid7_4() 
{
	// TODO: Add your control notification handler code here
	if(m_csGridBtn_7_4.m_pstItemData)
		MakeItemDesc(m_csGridBtn_7_4.m_pstItemData);
	else
		SetItemDesc("");
}
