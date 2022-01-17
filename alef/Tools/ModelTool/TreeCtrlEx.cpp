// TreeCtrlEx__.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "TreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx()
{
}

VOID CTreeCtrlEx::_Create(CWnd *pcsParent, RECT &stInitRect, DWORD dwStyle)
{
	InitializeMember();
	Create(WS_VISIBLE|WS_CHILD|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|dwStyle, stInitRect, pcsParent, NULL);
}

CTreeCtrlEx::~CTreeCtrlEx()
{	
	_DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.		
		//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
		ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers
HTREEITEM CTreeCtrlEx::_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible, int nImage, int nSelectedImage)
{
	HTREEITEM hThis = InsertItem(lpszItem, hParent, TVI_SORT);
	if( !hThis )								return NULL;
	if( dwData && !SetItemData(hThis, dwData) )	return NULL;
	if( nImage > -1 && !SetItemImage(hThis, nImage, nSelectedImage) )	return NULL;
	if( m_mapTreeItem.end() != m_mapTreeItem.find( lpszItem ) )			return hThis;

	if (bEnsureVisible)
		EnsureVisible(hThis);

	CUSTOM_TREEITEM* pNewItem = new CUSTOM_TREEITEM;
	pNewItem->hHandle = hThis;
	pNewItem->dwColor = RGB(0, 0, 0); // default color is black

	if( !(m_mapTreeItem.insert(make_pair(string(lpszItem), pNewItem)).second) )
		_asm nop;

	return hThis;
}

BOOL	CTreeCtrlEx::_UpdateItem( HTREEITEM hItem, int nImage, int nSelectedImage )
{
	if( nImage > -1 )
		return SetItemImage( hItem, nImage, nSelectedImage );

	return TRUE;
}

HTREEITEM CTreeCtrlEx::_FindItem(LPCSTR lpszItem)
{
	HTREEITEM hItem = GetRootItem();

	HTREEITEM	hTemp;
	CString		str;
	while (hItem)
	{
		while (hItem)
		{
			str = GetItemText(hItem);
			//if(str == lpszItem)
			if (!str.CompareNoCase(lpszItem))
				return hItem;

			hTemp = GetNextItem(hItem, TVGN_CHILD);
			if (!hTemp)
			{
				break;
			}

			hItem = hTemp;
		}

		while (hItem)
		{
			hTemp = GetNextItem(hItem, TVGN_NEXT);
			if (hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = GetParentItem(hItem);
			if (!hItem)
			{
				return NULL;
			}
		}
	}

	return NULL;
}

VOID CTreeCtrlEx::_ShowWindow()
{
	ModifyStyle(0, WS_VISIBLE);
	EnableWindow(TRUE);
}

VOID CTreeCtrlEx::_HideWindow()
{
	ModifyStyle(WS_VISIBLE, 0);
	EnableWindow(FALSE);
}

VOID CTreeCtrlEx::SetDrag(HTREEITEM hSelected)
{
	this->Select(hSelected, TVGN_CARET);
	m_bDrag = TRUE;
	SetCapture();
}

VOID CTreeCtrlEx::InitializeMember()
{
	m_bDrag = FALSE;
}

BOOL CTreeCtrlEx::_Copy(CTreeCtrlEx *pSrc)
{
	HTREEITEM hItem = pSrc->GetRootItem();

	HTREEITEM	hTemp;
	CString		strText;
	while (hItem)
	{
		while (hItem)
		{
			hTemp = pSrc->GetParentItem(hItem);
			if (hTemp)
			{
				strText = GetItemText(hTemp);
				hTemp	= _FindItemEx(strText);
				if( !hTemp )		
					return FALSE;
			}

			strText = pSrc->GetItemText(hItem);
			_InsertItem(strText, (hTemp) ? (hTemp) : (TVI_ROOT), pSrc->GetItemData(hItem));

			hTemp = pSrc->GetNextItem(hItem, TVGN_CHILD);
			if( !hTemp )
				break;

			hItem = hTemp;
		}

		while (hItem)
		{
			hTemp = pSrc->GetNextItem(hItem, TVGN_NEXT);
			if (hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = pSrc->GetParentItem(hItem);
			if ( !hItem )
				return FALSE;
		}
	}

	return TRUE;
}

HTREEITEM CTreeCtrlEx::_MoveItem(HTREEITEM hMoveItem, HTREEITEM hNewParentItem, BOOL bDeleteMovedItem)
{
	CString	strText	= GetItemText(hMoveItem);
	DWORD	dwData	= GetItemData(hMoveItem);

	//@{ 2006/05/04 burumal
	//if (bDeleteMovedItem)
		//DeleteItem(hMoveItem);
	if (bDeleteMovedItem)
		_DeleteItemEx(strText.GetBuffer());
	//@}

	return _InsertItem(strText, hNewParentItem, dwData);
}

//@{ burumal 2006/02/02
void CTreeCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nChar == 'c' || nChar == 'C' )
	{
		if ( GetKeyState(VK_LCONTROL) < 0 )
		{
			HTREEITEM hSelectedItem	= GetSelectedItem();
			if (!hSelectedItem)
				return;			

			CString strItemText = GetItemText(hSelectedItem);			
			//MessageBox(strItemText.GetString(), strItemText.GetString(), MB_OK);

			if ( OpenClipboard() ) 
			{ 			
				HANDLE hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (_tcslen(strItemText.GetString()) + 1) * sizeof(TCHAR)); 
				if(hglbCopy) { 
					EmptyClipboard(); 
					LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
					_tcscpy(lptstrCopy, strItemText.GetString()); 
					GlobalUnlock(hglbCopy); 

#ifdef _UNICODE 
					SetClipboardData(CF_UNICODETEXT,hglbCopy); 
#else 
					SetClipboardData(CF_TEXT,hglbCopy); 
#endif 								
				} 
				CloseClipboard();
			}
		}
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

HTREEITEM CTreeCtrlEx::_FindItemEx(LPCSTR lpszName)
{
	CHAR szItem[128];
	ZeroMemory(szItem, sizeof(szItem));
	strcpy(szItem, lpszName);

	mapTREE_ITEMIter itFind = m_mapTreeItem.find(string(szItem));
	return itFind != m_mapTreeItem.end() ? itFind->second->hHandle : NULL;
}

void CTreeCtrlEx::_DeleteItemEx(LPCSTR lpszName)
{		
	CHAR szItem[128];
	ZeroMemory(szItem, sizeof(szItem));
	strcpy(szItem, lpszName);

	mapTREE_ITEMIter itFind = m_mapTreeItem.find( szItem );
	if ( itFind != m_mapTreeItem.end() )
	{
		DeleteItem( (*itFind).second->hHandle );
		delete ( (*itFind).second );
		m_mapTreeItem.erase( itFind );
		//m_mapTreeItem.erase( (*itFind).first );
	}
}

void CTreeCtrlEx::_DeleteItemEx(HTREEITEM hDel)
{
	if ( !hDel )		return;
	
	CString	strText = GetItemText(hDel);
	_DeleteItemEx( strText.GetBuffer() );
}

void CTreeCtrlEx::_DeleteAllItems()
{	
	for ( mapTREE_ITEMIter itItem = m_mapTreeItem.begin(); itItem != m_mapTreeItem.end(); ++itItem )
	{
		//DeleteItem( (*itItem).second->hHandle );
		if ( itItem->second )
			delete itItem->second;
	}
	m_mapTreeItem.clear();

	DeleteAllItems();
}
//@}

//@{ 2006/09/21 burumal
void CTreeCtrlEx::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
	SetItemState(hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD);
}

BOOL CTreeCtrlEx::IsItemBold(HTREEITEM hItem)
{
	return GetItemState(hItem, TVIS_BOLD) & TVIS_BOLD;
}

void CTreeCtrlEx::SetItemColor(HTREEITEM hItem, DWORD dwColor)
{
	if ( !hItem )		return;
	
	CUSTOM_TREEITEM* pFound = FindCustomTreeItemInfo(hItem);
	if ( pFound )
		pFound->dwColor = dwColor;
}

BOOL CTreeCtrlEx::GetItemColor(HTREEITEM hItem, DWORD* pdwColor)
{
	if ( !pdwColor || !hItem  )	return FALSE;

	CUSTOM_TREEITEM* pFound = FindCustomTreeItemInfo(hItem);
	if ( pFound )
		(*pdwColor) = pFound->dwColor;

	return TRUE;
}

CUSTOM_TREEITEM* CTreeCtrlEx::FindCustomTreeItemInfo(HTREEITEM hItem)
{
	if ( hItem == NULL )
		return NULL;

	CString strItem = GetItemText(hItem); 
	string strText( strItem.GetBuffer() );

	mapTREE_ITEMIter itFind = m_mapTreeItem.find( strText );
	return itFind == m_mapTreeItem.end() ? NULL : itFind->second;
}

// void CTreeCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
// { 
// 	HTREEITEM hItem;
// 	NMTVCUSTOMDRAW* pcd = (NMTVCUSTOMDRAW*) pNMHDR;	
// 
// 	switch ( pcd->nmcd.dwDrawStage ) 
// 	{ 
// 	case CDDS_PREPAINT: 
// 		*pResult = CDRF_NOTIFYITEMDRAW;      
// 		break; 
// 
// 	case CDDS_ITEMPREPAINT : 
// 		hItem = (HTREEITEM)pcd->nmcd.dwItemSpec;
// 
// 		CUSTOM_TREEITEM* pFound = FindCustomTreeItemInfo(hItem);
// 		if ( pFound )
// 		{
// 			pcd->clrText = pFound->dwColor;
// 		}
// 		*pResult = 0;
// 		break; 
// 	}     
// } 
//@}
