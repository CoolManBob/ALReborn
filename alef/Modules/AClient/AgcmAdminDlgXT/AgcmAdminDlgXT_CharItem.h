#if !defined(AFX_AGCMADMINDLGXT_CHARITEM_H__2B4610AB_4E8D_4009_9E9E_1C091E2B3AAE__INCLUDED_)
#define AFX_AGCMADMINDLGXT_CHARITEM_H__2B4610AB_4E8D_4009_9E9E_1C091E2B3AAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_CharItem.h : header file
//
//
// 운영팀이 아이템을 그리드 형태로 보여달라고 요청해서 아이템만 떼서 다시 만듬.
// 스킬은 걍 리스트로 해도 될 듯..?
//

class CButtonIncludeItemData : public CButton
{
public:
	stAgpdAdminItemData* m_pstItemData;

public:
	CButtonIncludeItemData() { m_pstItemData = NULL; }
	virtual ~CButtonIncludeItemData() {;}

	void InitData() { SetWindowText(_T("")); m_pstItemData = NULL; }
};

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_CharItem dialog

class AgcmAdminDlgXT_CharItem : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_CharItem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_CharItem)
	enum { IDD = IDD_CHAR_ITEM };
	CButtonIncludeItemData	m_csGridBtn_7_4;
	CButtonIncludeItemData	m_csGridBtn_7_3;
	CButtonIncludeItemData	m_csGridBtn_7_2;
	CButtonIncludeItemData	m_csGridBtn_7_1;
	CButtonIncludeItemData	m_csGridBtn_6_4;
	CButtonIncludeItemData	m_csGridBtn_6_3;
	CButtonIncludeItemData	m_csGridBtn_6_2;
	CButtonIncludeItemData	m_csGridBtn_6_1;
	CButtonIncludeItemData	m_csGridBtn_5_4;
	CButtonIncludeItemData	m_csGridBtn_5_3;
	CButtonIncludeItemData	m_csGridBtn_5_2;
	CButtonIncludeItemData	m_csGridBtn_5_1;
	CButtonIncludeItemData	m_csGridBtn_4_4;
	CButtonIncludeItemData	m_csGridBtn_4_3;
	CButtonIncludeItemData	m_csGridBtn_4_2;
	CButtonIncludeItemData	m_csGridBtn_4_1;
	CButtonIncludeItemData	m_csGridBtn_3_4;
	CButtonIncludeItemData	m_csGridBtn_3_3;
	CButtonIncludeItemData	m_csGridBtn_3_2;
	CButtonIncludeItemData	m_csGridBtn_3_1;
	CButtonIncludeItemData	m_csGridBtn_2_4;
	CButtonIncludeItemData	m_csGridBtn_2_3;
	CButtonIncludeItemData	m_csGridBtn_2_2;
	CButtonIncludeItemData	m_csGridBtn_2_1;
	CButtonIncludeItemData	m_csGridBtn_1_4;
	CButtonIncludeItemData	m_csGridBtn_1_3;
	CButtonIncludeItemData	m_csGridBtn_1_2;
	CButtonIncludeItemData	m_csGridBtn_1_1;
	CStatic	m_csCharNameStatic;
	CXTFlatComboBox	m_csItemPosCB;
	CString	m_szItemDesc;
	CString	m_szBankMoney;
	CString	m_szInvenMoney;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_CharItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bInitialized;

	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	list<stAgpdAdminItemData*> m_listItem;
	list<stAgpdAdminItemData*> m_listBankItem;

	CCriticalSection m_csLock;
	
public:
	// Windows
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	void Lock();
	void Unlock();

	BOOL SetCharName(CHAR* szCharName);
	BOOL IsSearchCharacter(CHAR* szCharName);
	
	BOOL SetItem(INT16 lType, stAgpdAdminItemData* pstItem);
	stAgpdAdminItemData* GetItem(stAgpdAdminItemData* pstItem);
	stAgpdAdminItemData* GetItemFromBank(stAgpdAdminItemData* pstItem);
	BOOL DeleteItem(stAgpdAdminItemData* pstItem);
	BOOL DeleteItemFromBank(stAgpdAdminItemData* pstItem);

	BOOL SetInvenMoney(INT32 lMoney);
	BOOL SetBankMoney(INT32 lMoney);
	BOOL SetItemDesc(LPCTSTR szDesc);
	
	BOOL ShowItemGrid();
	
	BOOL ClearItemGrid();
	BOOL ClearItemList();
	BOOL ClearBankItemList();
	BOOL ClearInvenMoney();
	BOOL ClearBankMoney();
	BOOL ClearItemDesc();
	
	INT32 GetItemPosByComboBox();
	INT32 GetInventoryIndexByComboBox();
	BOOL IsBankItem(stAgpdAdminItemData* pstItem);
	CButton* GetGridBtnByPos(stAgpdAdminItemData* pstItem);
	
	BOOL DrawItemTexture(stAgpdAdminItemData* pstItem, CButton* pcsButton = NULL);

	// Item Desc
	BOOL MakeItemDesc(stAgpdAdminItemData* pstItem);
	CString GetItemDescription(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertHistory(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertAttrInfo(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertWeaponAttrInfo(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertArmourAttrInfo(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertShieldAttrInfo(stAgpdAdminItemData* pstItem);

protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_CharItem)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCbCharItemIndex();
	afx_msg void OnBCharItemRefresh();
	afx_msg void OnBCharItemGrid1_1();
	afx_msg void OnBCharItemGrid1_2();
	afx_msg void OnBCharItemGrid1_3();
	afx_msg void OnBCharItemGrid1_4();
	afx_msg void OnBCharItemGrid2_1();
	afx_msg void OnBCharItemGrid2_2();
	afx_msg void OnBCharItemGrid2_3();
	afx_msg void OnBCharItemGrid2_4();
	afx_msg void OnBCharItemGrid3_1();
	afx_msg void OnBCharItemGrid3_2();
	afx_msg void OnBCharItemGrid3_3();
	afx_msg void OnBCharItemGrid3_4();
	afx_msg void OnBCharItemGrid4_1();
	afx_msg void OnBCharItemGrid4_2();
	afx_msg void OnBCharItemGrid4_3();
	afx_msg void OnBCharItemGrid4_4();
	afx_msg void OnBCharItemGrid5_1();
	afx_msg void OnBCharItemGrid5_2();
	afx_msg void OnBCharItemGrid5_3();
	afx_msg void OnBCharItemGrid5_4();
	afx_msg void OnBCharItemGrid6_1();
	afx_msg void OnBCharItemGrid6_2();
	afx_msg void OnBCharItemGrid6_3();
	afx_msg void OnBCharItemGrid6_4();
	afx_msg void OnBCharItemGrid7_1();
	afx_msg void OnBCharItemGrid7_2();
	afx_msg void OnBCharItemGrid7_3();
	afx_msg void OnBCharItemGrid7_4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_CHARITEM_H__2B4610AB_4E8D_4009_9E9E_1C091E2B3AAE__INCLUDED_)
