// ObjectListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "ObjectListDlg.h"
#include "EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectListDlg dialog


CObjectListDlg::CObjectListDlg(ObjectListDlgCallback pfInitCallback, ObjectListDlgCallback pfEndCallback, PVOID pvCustClass, CHAR *pszDest, CWnd* pParent /*=NULL*/)
	: CDialog(CObjectListDlg::IDD, pParent),m_bListUpdated( FALSE )
{
	//{{AFX_DATA_INIT(CObjectListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pvCustClass		= pvCustClass;
	m_pfInitCallback	= pfInitCallback;
	m_pfEndCallback		= pfEndCallback;
	m_pszDest			= pszDest;
}


void CObjectListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectListDlg)
	DDX_Control(pDX, IDC_TREE_OBJECT_LIST, m_csList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectListDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectListDlg)
	ON_BN_CLICKED(IDC_BUTTON_OL_ADD, OnButtonOlAdd)
	ON_BN_CLICKED(IDC_BUTTON_OL_REMOVE, OnButtonOlRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectListDlg message handlers

BOOL CObjectListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_pfInitCallback)
		m_pfInitCallback(this, m_pvCustClass);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CObjectListDlg::_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible)
{
	HTREEITEM hThis = m_csList.InsertItem(lpszItem, hParent, TVI_SORT);
	if(!hThis)
		return NULL;

	if(dwData)
	{
		if(!m_csList.SetItemData(hThis, dwData))
			return NULL;
	}

	if(bEnsureVisible)
		m_csList.EnsureVisible(hThis);

	return hThis;
}

HTREEITEM CObjectListDlg::_FindItem(LPCSTR lpszItem)
{
	HTREEITEM hItem = m_csList.GetRootItem();

	HTREEITEM	hTemp;
	CString		str;
	while(hItem)
	{
		while(hItem)
		{
			str = m_csList.GetItemText(hItem);
			if(str == lpszItem)
				return hItem;

			hTemp = m_csList.GetNextItem(hItem, TVGN_CHILD);
			if(!hTemp)
			{
				break;
			}

			hItem = hTemp;
		}

		while(hItem)
		{
			hTemp = m_csList.GetNextItem(hItem, TVGN_NEXT);
			if(hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = m_csList.GetParentItem(hItem);
			if(!hItem)
			{
				return NULL;
			}
		}
	}

	return NULL;
}

void CObjectListDlg::OnOK() 
{
	// TODO: Add extra validation here
	HTREEITEM hSelected = m_csList.GetSelectedItem();
	if(!hSelected)
	{
		MessageBox("먼저 선택하세요.", "Error");
		return;
	}

	if(m_csList.GetItemData(hSelected) != OBJECGT_LIST_DLG_TREE_ITEM_SELECT)
	{
		MessageBox("카테고리는 선택하실 수 없습니다.", "Error");
		return;
	}

	if(m_pfEndCallback)
	{
		if( m_pfEndCallback(this, m_pvCustClass) && m_bListUpdated )
		{
			// 이게 파일에서만 저장하는거라 모델툴 자체에 업데이트가 되어있지 않다.
			// 그래서 리스트가 변경된 경우라면 모델툴에서 LoadTemplate를 다시 해서
			// 리스트를 재정비를 해주어야 하는데 귀찮아서
			// 다시 띄우라고 메시지만 띄운다
			// 대체 첨에 누가 이렇게 만든겨! 버럭
			// by 마고자 2007/12/03

			MessageBox( "오브젝트 카테고리가 업데이트 되었습니다. 모델툴 다시 실행시키세요" );
		}
	}

	strcpy(m_pszDest, m_csList.GetItemText(hSelected));

	CDialog::OnOK();
}

void CObjectListDlg::OnButtonOlAdd() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM	hParent	= m_csList.GetSelectedItem();
	if(!hParent)
	{
		MessageBox("먼저 아이템을 선택하세요.", "Error");
		return;
	}

	CHAR	szNewName[256];
	CHAR	szCategory[256];
	CEditDlg	dlg(szNewName);
	if(dlg.DoModal() == IDOK)
	{
		if(!strcmp(szNewName, ""))
		{
			MessageBox("카테고리 이름을 다시 지정하세요!", "Error");
			return;
		}

		sprintf(szCategory, "[%s]", szNewName);
		_InsertItem(szCategory, hParent, OBJECGT_LIST_DLG_TREE_ITEM_SELECT, TRUE);

		m_bListUpdated	= TRUE;
	}
}

void CObjectListDlg::OnButtonOlRemove() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM	hParent	= m_csList.GetSelectedItem();
	if(!hParent)
	{
		MessageBox("먼저 아이템을 선택하세요.", "Error");
		return;
	}

	m_csList.DeleteItem(hParent);
	m_bListUpdated	= TRUE;
}

VOID CObjectListDlg::_EnableButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_OL_ADD)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_OL_REMOVE)->EnableWindow(bEnable);
}