#if !defined(AFX_AGCMADMINDLGXT_CHARACTER_H__D26E0E03_85C5_495F_BEEA_C0D5BB54A0EE__INCLUDED_)
#define AFX_AGCMADMINDLGXT_CHARACTER_H__D26E0E03_85C5_495F_BEEA_C0D5BB54A0EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAdminDlgXT_Character.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Character dialog

class AgcmAdminDlgXT_Character : public CDialog
{
// Construction
public:
	AgcmAdminDlgXT_Character(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAdminDlgXT_Character)
	enum { IDD = IDD_CHAR_DATA };
	CStatic	m_csDrawPlace;
	CXTButton	m_csBanButton;
	CXTButton	m_csRefreshButton;
	CXTFlatComboBox	m_csCharViewCB;
	CStatic	m_csViewGridPlace;
	CStatic	m_csCharGridPlace;
	CString	m_szCharViewInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAdminDlgXT_Character)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bInitialized;

	// Callback Function Pointer
	ADMIN_CB m_pfEditCharacter;
	ADMIN_CB m_pfSkillRollback;
	ADMIN_CB m_pfSearchParty;	// Party 검색
	ADMIN_CB m_pfReset;			// Party 깨기

	// Basic
	stAgpdAdminSearch m_stLastSearch;	// 마지막 검색한 캐릭터 정보
	stAgpdAdminCharData m_stCharData;	// 모든 페이지에서 Base 정보로 쓰인다.

	// Item
	list<stAgpdAdminItemData*> m_listItem;

	// Skill
	list<stAgpdAdminSkillMastery*> m_listMastery;
	list<stAgpdAdminSkillData*> m_listSkillData;

	// Party
	list<stAgpdAdminCharPartyMember*> m_listMember;

	// Grid
	CXTPPropertyGrid* m_pcsCharGrid;
	CXTPPropertyGrid* m_pcsViewGrid;	// 아래 세개 중 하나를 보여준다.
	CXTPPropertyGrid* m_pcsItemGrid;
	CXTPPropertyGrid* m_pcsSkillGrid;
	CXTPPropertyGrid* m_pcsPartyGrid;

	CCriticalSection m_csLock;

public:
	// Windows
	BOOL Create();
	BOOL OpenDlg(INT nShowCmd = SW_SHOW);
	BOOL CloseDlg();
	BOOL IsInitialized();

	void Lock();
	void Unlock();

	// Basic
	BOOL SetCBEditCharacter(ADMIN_CB pfCallback);

	BOOL SetLastSearch(stAgpdAdminSearch* pstLastSearch);
	stAgpdAdminSearch* GetLastSearch();
	BOOL IsSearchCharacter(stAgpdAdminCharData* pstCharData);
	BOOL IsSearchCharacter(INT32 lCID);
	BOOL IsSearchCharacter(CHAR* szName);

	BOOL SetCharData(stAgpdAdminCharData* pstCharData);
	BOOL SetCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub);
	stAgpdAdminCharData* GetCharData();

	BOOL ShowCharGrid();
	BOOL ClearCharGrid();

	// Edit
	BOOL EditChar(INT32 lCID, LPCTSTR szCharName, LPCTSTR szEditField, LPCTSTR szNewValue);
	BOOL ReceiveEditResult(stAgpdAdminCharEdit* pstCharEdit);
	
	// Item
	BOOL SetItem(INT16 lType, stAgpdAdminItemData* pstItem);
	stAgpdAdminItemData* GetItemBySlot(AgpmItemPart eItemPart);
	stAgpdAdminItemData* GetItem(stAgpdAdminItemData* pstItem);
	BOOL DeleteItem(stAgpdAdminItemData* pstItem);

	CXTPPropertyGridItem* GetItemCategory(stAgpdAdminItemData* pstItem);
	CString GetItemDescription(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertHistory(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertAttrInfo(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertWeaponAttrInfo(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertArmourAttrInfo(stAgpdAdminItemData* pstItem);
	CString GetItemDescriptionConvertShieldAttrInfo(stAgpdAdminItemData* pstItem);

	BOOL ShowItemGrid();

	BOOL ClearItemList();	// 멤버 리스트를 비운다.
	BOOL ClearItemGrid();	// 화면출력을 비운다.

	// Skill
	BOOL SetCBSkillRollback(ADMIN_CB pfCallback);
	
	BOOL SetMasteryList(PVOID pList);
	BOOL SetSkill(stAgpdAdminSkillMastery* pstMastery, stAgpdAdminSkillData* pstSkill);
	stAgpdAdminSkillData* GetSkill(stAgpdAdminSkillData* pstSkill);
	stAgpdAdminSkillData* GetSkill(const char* szSkilName);

	BOOL SetCharSkillPoint(CHAR* szCharName, INT32 lTotalSP, INT32 lRemainSP);

	CString GetSkillDescription(stAgpdAdminSkillData* pstSkill);
	CString GetSkillDescriptionLevel(stAgpdAdminSkillDataDesc* pstSkillDesc);

	BOOL DeleteSkill(stAgpdAdminSkillData* pstSkill);	// 이미 List 에 있는 스킬이면 지운다.

	BOOL ShowSkillGrid();

	BOOL ClearMasteryList();
	BOOL ClearSkillList();
	BOOL ClearSkillGrid();

	// Party
	BOOL SetCBSearchParty(ADMIN_CB pfCallback);
	BOOL SetCBReset(ADMIN_CB pfCallback);

	BOOL SetMember(stAgpdAdminCharParty* pstParty);	// 2004.03.22 추가
	BOOL SetMember(stAgpdAdminCharPartyMember* pstMember);
	BOOL SetMember(stAgpdAdminCharDataSub* pstMemberSub);
	BOOL SetMember(PVOID pList);

	stAgpdAdminCharPartyMember* GetMember(INT32 lCID);
	stAgpdAdminCharPartyMember* GetMember(CHAR* szMemberName);

	BOOL ShowMemberGrid();

	BOOL ClearMemberList();
	BOOL ClearMemberGrid();

	//////////////////////////////////////////////////////////////////////////
	BOOL ClearAllData();

// Implementation
protected:
	BOOL OnInitGrids();
	BOOL OnInitCharGrid();
	BOOL OnInitItemGrid();
	BOOL OnInitSkillGrid();
	BOOL OnInitPartyGrid();

	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	LRESULT OnCharGridSelChanged(CXTPPropertyGridItem* pItem);
	LRESULT OnItemGridSelChanged(CXTPPropertyGridItem* pItem);
	LRESULT OnSkillGridSelChanged(CXTPPropertyGridItem* pItem);
	LRESULT OnPartyGridSelChanged(CXTPPropertyGridItem* pItem);

	LRESULT OnCharGridItemValueChanged(CXTPPropertyGridItem* pItem);

	BOOL DrawTexture(stAgpdAdminItemData* pstItemData);
	BOOL DrawTexture(stAgpdAdminItemTemplate* pstItemTemplate);
	BOOL DrawTexture(stAgpdAdminSkillData* pstSkillData);
	BOOL DrawTexture(CHAR* szTextureName, eTextureType eType = TEXTURE_TYPE_ITEM);

	// Generated message map functions
	//{{AFX_MSG(AgcmAdminDlgXT_Character)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSelchangeCbCharView();
	afx_msg void OnBtnCharRefresh();
	afx_msg void OnBtnCharBan();
	virtual void OnOK() {;}
	virtual void OnCancel() {;}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMADMINDLGXT_CHARACTER_H__D26E0E03_85C5_495F_BEEA_C0D5BB54A0EE__INCLUDED_)
