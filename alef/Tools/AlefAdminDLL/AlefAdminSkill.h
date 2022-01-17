#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "Resource.h"
#include "AgpmEventSkillMaster.h"
#include "AgpmProduct.h"

// AlefAdminSkill 폼 뷰입니다.

class AlefAdminSkill : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminSkill)

protected:
	AlefAdminSkill();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminSkill();

public:
	enum { IDD = IDD_SKILL };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	typedef list<stAgpdAdminSkill*>				SkillList;
	typedef list<stAgpdAdminSkill*>::iterator	SkillIter;
	typedef vector<INT32>						ProductVector;
	typedef vector<INT32>::iterator				ProductIter;

protected:
	SkillList m_listSkill;
	ProductVector m_vcProduct;
	INT32 m_lSkillPoint;

	AgpmSkill* m_pcsAgpmSkill;
	AgpmEventSkillMaster* m_pcsAgpmEventSkillMaster;
	AgpmProduct* m_pcsAgpmProduct;

public:
	virtual void OnInitialUpdate();

	BOOL InitComboBox();
	BOOL InitListView();

	BOOL OnReceive(AgpdSkill* pcsSkill);
	BOOL OnReceiveSkillString(stAgpdAdminSkillString* pstSkillString);

	BOOL AddSkillList(stAgpdAdminSkill* pstSkill);
	BOOL ClearSkillList();
	stAgpdAdminSkill* GetSkill(INT32 lTID);
	stAgpdAdminSkill* GetSkill(CHAR* szSkillName);

	AgpdSkillTemplate* GetSkillTemplate(INT32 lTID);
	AgpdSkillTemplate* GetSkillTemplate(CHAR* szSkillName);

	BOOL ShowData(stAgpdAdminSkill* pstSkill);
	BOOL ShowData();
	BOOL ShowProductCompose();
	BOOL ClearSkillListCtrl();
	BOOL ClearSkillAllListCtrl();

	BOOL ShowAllSkill();
	BOOL SetAllSkillByTID(INT32 lTID);

	BOOL SetSkillPoint(INT32 lSkillPoint);
	BOOL CheckLearnableSkill(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL ProcessLevelUpDown(BOOL bUp);
	BOOL ProcessProductLevel(BOOL bUp);

	INT32 GetSelectedOwnSkill(vector<INT32>& vcSelectedSkill);
	INT32 GetSelectedAllSkill(vector<INT32>& vcSelectedSkill);

	INT32 MakeLearnString(vector<INT32>& vcSelectedSkill, stAgpdAdminSkillString& stSkillString);
	INT32 MakeLevelString(vector<INT32>& vcSelectedSkill, stAgpdAdminSkillString& stSkillString, BOOL bUp);
	INT32 MakeInitAllString(stAgpdAdminSkillString& stSkillString);
	INT32 MakeInitSpecificString(vector<INT32>& vcSelectedSkill, stAgpdAdminSkillString& stSkillString);
	INT32 MakeCurrentString(stAgpdAdminSkillString& stSkillString);
	
	INT32 MakeLearnComposeString(vector<INT32>& vcSelected, stAgpdAdminSkillString& stSkillString);
	INT32 MakeInitAllComposeString(stAgpdAdminSkillString& stSkillString);
	INT32 MakeInitSpecificComposeString(vector<INT32>& vcSelected, stAgpdAdminSkillString& stSkillString);
	INT32 MakeCurrentProductComposeString(stAgpdAdminSkillString& stSkillString);

	BOOL CheckEnableInitialization(AgpdSkillTemplate* pcsSkillTemplate);
	BOOL IsDefaultSkill(AgpdSkillTemplate* pcsSkillTemplate, INT32 lCharTID);
	
	inline BOOL IsOwnIndexProduct();
	inline BOOL IsOwnIndexCompose();
	inline BOOL IsAllIndexCompose();
	inline BOOL IsProductSkill(INT32 lTID);
	inline BOOL IsProductSkill(AgpdSkillTemplate* pcsSkillTemplate);
	INT32 GetLevelOfExp(stAgpdAdminSkill& stSkill);
	INT32 GetExpOfCurrLevel(stAgpdAdminSkill& stSkill);
	INT32 GetMaxExpOfCurrLevel(stAgpdAdminSkill& stSkill, INT32 lLevel = 0);
	INT32 GetAccExpOfCurrLevel(stAgpdAdminSkill& stSkill, INT32 lLevel = 0);

	BOOL AddAdminMessage(CHAR* szSkillName, UINT ulResourceID, INT32 lValue1 = 0);
	inline void MessageRefreshOffLine();

	BOOL	IsHighLevelSkill( AuRaceType eRaceType, AuCharClassType eClassType, INT32 nSkillTID );

	// nSkillTID 에 해당하는 스킬이 현재 선택된 분류에 맞는 스킬인가 ( 드래곤시온 전용 )
	BOOL	IsCurrentClassSkillForDragonScion( INT32 nSelect, INT32 nSkillTID );

	static BOOL CBComposeInfo(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	CXTFlatComboBox m_csSkillTypeCB;
	CListCtrl m_csOwnSkillLV;
	CXTFlatHeaderCtrl m_flatHeaderOwnSkill;

	CXTFlatComboBox m_csSkillCategory;
	CListCtrl m_csSkillLV;
	CXTFlatHeaderCtrl m_flatHeaderAllSkill;

	CXTButton m_csInitAllBtn;
	CXTButton m_csInitSpecificBtn;
	CXTButton m_csRefreshBtn;
	CXTButton m_csLearnBtn;
	CXTButton m_csLevelUpBtn;
	CXTButton m_csLevelDownBtn;
	CXTButton m_csProductExpChangeBtn;

	CButton m_csOwnSkillAllCheckBox;
	CButton m_csAllSkillAllCheckBox;

	afx_msg void OnBnClickedBRefresh();
	afx_msg void OnCbnSelchangeCbSkillType();
	afx_msg void OnCbnSelchangeCbSkillCategory();
	afx_msg void OnBnClickedBLearn();
	afx_msg void OnNMClickLvAllSkill(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLvOwnSkill(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCkOwnSkillAll();
	afx_msg void OnBnClickedCkAllSkillAll();
	afx_msg void OnBnClickedBInitSkillAll();
	afx_msg void OnBnClickedBInitSkillSpecific();
	afx_msg void OnBnClickedBLevelUp();
	afx_msg void OnBnClickedBLevelDown();
	afx_msg void OnBnClickedBProductExpChange();
};


