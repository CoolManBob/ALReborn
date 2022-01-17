// ObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "ObjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectDlg dialog


CObjectDlg::CObjectDlg(ObjectDlgCallback pfCallback, PVOID pvCustClass, CHAR *pszDest, CWnd* pParent /*=NULL*/)
	: CDialog(CObjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pvCustClass	= pvCustClass;
	m_pfCallback	= pfCallback;
	m_pszDest		= pszDest;
}


void CObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectDlg)
	DDX_Control(pDX, IDC_TREE_OBJECT_LIST, m_csObjectList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectDlg message handlers

BOOL CObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pfCallback(this, m_pvCustClass);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CObjectDlg::_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD dwData)
{
	HTREEITEM hThis = m_csObjectList.InsertItem(lpszItem, hParent, TVI_SORT);
	if(!hThis)
		return NULL;

	if(dwData)
	{
		if(!m_csObjectList.SetItemData(hThis, dwData))
			return NULL;
	}

	return hThis;
}

HTREEITEM CObjectDlg::_FindItem(LPCSTR lpszItem)
{
	HTREEITEM hItem = m_csObjectList.GetRootItem();

	HTREEITEM	hTemp;
	CString		str;
	while(hItem)
	{
		while(hItem)
		{
			str = m_csObjectList.GetItemText(hItem);
			if(str == lpszItem)
				return hItem;

			hTemp = m_csObjectList.GetNextItem(hItem, TVGN_CHILD);
			if(!hTemp)
			{
				break;
			}

			hItem = hTemp;
		}

		while(hItem)
		{
			hTemp = m_csObjectList.GetNextItem(hItem, TVGN_NEXT);
			if(hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = m_csObjectList.GetParentItem(hItem);
			if(!hItem)
			{
				return NULL;
			}
		}
	}

	return NULL;
}

void CObjectDlg::OnOK() 
{
	// TODO: Add extra validation here
	HTREEITEM hSelected = m_csObjectList.GetSelectedItem();
	if(!hSelected)
	{
		MessageBox("먼저 선택하세요.", "Error");
		return;
	}

	if(m_csObjectList.GetItemData(hSelected) != OBJECGT_DLG_TREE_ITEM_SELECT)
	{
		MessageBox("카테고리는 선택하실 수 없습니다.", "Error");
		return;
	}

	strcpy(m_pszDest, m_csObjectList.GetItemText(hSelected));

	CDialog::OnOK();
}
