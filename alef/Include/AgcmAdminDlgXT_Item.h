#if !defined(AFX_AGCMADMINDLGXT_ITEM_H__4CED90B3_D9D6_4232_94C8_A84BCDEFCB58__INCLUDED_)
#define AFX_AGCMADMINDLGXT_ITEM_H__4CED90B3_D9D6_4232_94C8_A84BCDEFCB58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Item.h : header file
//

#include "Resource.h"
#include <hash_map>

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Item dialog

class AgcmAdminDlgXT_Item : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Item(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Item)
	enum { IDD = IDD_ITEM };
	CStatic	m_csItemDesc;
	CEdit	m_csItemCountEdit;
	CXTFlatComboBox	m_csRuneItemCB;
	CXTFlatComboBox	m_csSpiritStoneCB;
	CXTFlatComboBox	m_csEgoItemCB;
	CXTButton	m_csConvertBtn;
	CStatic	m_csDrawPlace2;
	CStatic	m_csDrawPlace;
	CXTFlatComboBox	m_csItemCB;
	CXTButton	m_csCreateBtn;
	CString	m_szItemCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Item)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	ADMIN_CB m_pfCBItemCreate;
	ADMIN_CB m_pfCBGetInventoryItem;

	hash_map<INT32, stAgpdAdminItemTemplate*> m_mapItemTemplate;
	list<stAgpdAdminItemData*> m_listInventoryItem;

	enum eItemOperationMode { ITEM_OPERATION_MODE_CREATE = 0, ITEM_OPERATION_MODE_CONVERT };
	eItemOperationMode m_eMode;

	INT32 m_lSelectedIndex;

	CCriticalSection m_csLock;

public:
	// Windows
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	void Lock();
	void Unlock();

	// Callback Fuction Registration
	BOOL SetCallbackItemCreate(ADMIN_CB pfCBItemCreate);
	BOOL SetCallbackGetInventoryItem(ADMIN_CB pfCallback);

	BOOL AddItemTemplate(stAgpdAdminItemTemplate* pstItemTemplate);
	BOOL RemoveItemTemplate(INT32 lITID);
	BOOL ClearItemTemplateData();

	stAgpdAdminItemTemplate* GetItemTemplate(INT32 lTID);
	stAgpdAdminItemTemplate* GetItemTemplate(const char* szItemName);

	BOOL ReceiveResult(stAgpdAdminItemOperation* pstItemOperation);

	BOOL GetInventoryItemList();
	stAgpdAdminItemData* GetInventoryItem(INT32 lItemID);
	BOOL ClearInventoryItemListData();
	BOOL RefreshInventoryItemList();

	BOOL ShowItemTemplate();
	BOOL ClearItemTemplate();

	// Convert Property
	BOOL ShowRuneItem();
	BOOL ClearRuneItem();

	BOOL ShowSpiritStoneItem();
	BOOL ClearSpiritStoneItem();

	BOOL ShowEgoItem();
	BOOL ClearEgoItem();

	BOOL SetComboBoxCursorSelectedIndex();

// Implementation
protected:
	BOOL ApplyEnableControls();
	BOOL DrawConvertItemTexture(CString& szName);
	INT32 GetItemIDFromCBText(CString& szName);
	
	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Item)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	afx_msg void OnSelchangeCbItemTid();
	afx_msg void OnBItemCreate();
	afx_msg void OnBItemConvert();
	afx_msg void OnRItemCreate();
	afx_msg void OnRItemConvert();
	afx_msg void OnSelchangeCbItemSpiritStoneTid();
	afx_msg void OnEditchangeCbItemTid();
	afx_msg void OnEditchangeCbItemSpiritStoneTid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_ITEM_H__4CED90B3_D9D6_4232_94C8_A84BCDEFCB58__INCLUDED_)
