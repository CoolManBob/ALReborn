#pragma once

#include "AgpdSkill.h"
#include "AgcdSkill.h"

#include "EventButton.h"

#include <list>

typedef std::list< INT32 >		ListSkillID;
typedef ListSkillID::iterator	ListSkillIDItr;

#define AGCM_CHAR_SKILL_DLG_TREE_ROOT_NAME							"[HIERARCHY]"
#define AGCM_CHAR_SKILL_DLG_TREE_ROOT_DATA							9999
#define AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_X_NUM						8
#define AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_Y_NUM						2
#define AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM						(AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_X_NUM * AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_Y_NUM)

#define AGCM_CHAR_SKILL_DLG_EBS_COMMON_BLANK						5
#define AGCM_CHAR_SKILL_DLG_EBS_COMMON_EXTENT						25
#define AGCM_CHAR_SKILL_DLG_EBS_COMMON_X							267
//#define AGCM_CHAR_SKILL_DLG_EBS_ANIM_Y								160
#define AGCM_CHAR_SKILL_DLG_EBS_MATCH_Y								264
#define AGCM_CHAR_SKILL_DLG_EBS_BASE_Y								170

enum eAgcmCharSkillDlgEventButtonParamsData
{
	E_AGMC_CHAR_SKILL_DLG_EBPD_SKILL_INDEX,
	E_AGMC_CHAR_SKILL_DLG_EBPD_WEAPON_TYPE,
	E_AGMC_CHAR_SKILL_DLG_EBPD_EFFECT_INDEX,
	E_AGMC_CHAR_SKILL_DLG_EBPD_NUM
};

enum eAgcmCharSkillDlgEventButton
{
	E_AGCM_CHAR_SKILL_DLG_EB_MATCH = 0,
	E_AGCM_CHAR_SKILL_DLG_EB_BASE,
	E_AGCM_CHAR_SKILL_DLG_EB_NUM
};

class AgcmCharSkillDlg : public CDialog
{
// Construction
public:
	AgcmCharSkillDlg(	AgcdCharacterTemplate *pstAgcdCharacterTemplate,
						AgpdSkillTemplateAttachData *pcsAgpdSkillTemplateAttachData,
						AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData,
						CHAR *szFindAnimPathName1,
						CHAR *szFindAnimPathName2,
						CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmCharSkillDlg)
	enum { IDD = IDD_CHAR_SKILL };
	CComboBox	m_csWeaponType;
	CTreeCtrl	m_csSkillTree;
	CString	m_strInfo			;
	CString	m_strCurAnimFrame	;
	CString	m_strAnim			;
	CString	m_strSkillName		;
	BOOL	m_bAlwaysShow		;
	//}}AFX_DATA

protected:
	CHAR						m_szFindAnimPathName1[256];
	CHAR						m_szFindAnimPathName2[256];

	AgcdCharacterTemplate		*m_pstAgcdCharacterTemplate;
	AgpdSkillTemplateAttachData	*m_pcsAgpdSkillTemplateAttachData;
	AgcdSkillAttachTemplateData	*m_pcsAgcdSkillAttachTemplateData;

	//CEventButton				*m_pacsAnimationButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	//CEventButton				*m_pacsMatchButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	//CEventButton				*m_pacsEtcButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	CEventButton				*m_pacsBaseButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];
	CEventButton				*m_pacsMatchButton[AGCM_CHAR_SKILL_DLG_EVENT_BUTTON_NUM];

	INT32						m_lSkillIndex;
	INT32						m_lWeaponType;
	INT32						m_lEffectIndex;

	INT32						m_lPreWeaponType;

public:
//	BOOL		UpdateButton(); //AgcdUseEffectSetData *pstData);
//	BOOL		UpdateSearchCondition();
//	BOOL		UpdateScriptDlg();
	BOOL		InitializeDlg(
		AgcdCharacterTemplate *pcsAgcdCharacterTemplate,
		AgpdSkillTemplateAttachData *pcsAgpdSkillTemplateAttachData,
		AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData	);
	BOOL		UpdateSkillControl(BOOL bEnable);

protected:
	BOOL		InitializeSkillTree();
	BOOL		InitializeWeaponType();

	BOOL		InitializeSkillControl();

	BOOL		InitializeAllEventButton();
	INT32		CreateEventButton(eAgcmCharSkillDlgEventButton eType, INT32 lSkillIndex, INT32 lWeaponType, INT32 lEffectIndex, CHAR *szForceText = NULL);
//	BOOL		UpdateBaseButton(INT32 lMatchIndex, INT32 lDisplayIndex, INT32 lMaxIndex);
	BOOL		MoveToMatchButton(INT32 lMatchIndex);
	BOOL		DestroyAllEventButton();
	BOOL		AddButton(BOOL bCopyCurData = FALSE);
	BOOL		RemoveButton();

	BOOL		UpdateEffectDlg(AgcdUseEffectSet *pstSet, INT32 lIndex);

	//. 2005. 08. 30 Nonstopdj
	//. 등록할려는 스킬이름이 트리에 이미 존재하는지 판단.
	BOOL		CheckSkillExist(const CHAR* szName);

	//. pSkillName과 같은 이름을 가진 모든 SkillTreeItem을 삭제한다.
	void		RemoveSameNameSkillTreeItem(CHAR* pSkillName);

	HTREEITEM	InsertSkillTreeItem(CHAR *szName, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible = FALSE);
	HTREEITEM	FindSkillTreeItem(INT32 lSkillIndex, INT32 lWeaponType = -1);
	BOOL		RemoveAllSkillTreeItem();
	BOOL		RemoveSkillTreeItem(HTREEITEM hSkill);
	BOOL		RemoveSkillAnimation(INT32 lSkillIndex, INT32 lWeaponType);
	BOOL		RemoveSkillEffect(INT32 lSkillIndex, INT32 lWeaponType);
	BOOL		RemoveSkillInfo(INT32 lSkillIndex, INT32 lWeaponType);

	BOOL		CheckIdentityWeaponType();

	INT32		GetNumVisualInfo();
	BOOL		GetSkillIDList( ListSkillID& listSkillID );

	// Callback
	static BOOL	EventButtonCB(PVOID pvData);

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	// Overrides	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(AgcmCharSkillDlg)
	afx_msg void OnDestroy();
	afx_msg void OnSelchangedTreeSkill(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonChangeSkillInfo();
	afx_msg void OnButtonAddEffect();
	afx_msg void OnButtonCopyEffect();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonOpenConditionList();
	afx_msg void OnButtonOpenStatusList();
	afx_msg void OnButtonOpenEffectList();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonSetAnimation();
	afx_msg void OnButtonRemoveEffect();
	afx_msg void OnSelchangeComboWeaponType();
	afx_msg void OnButtonAnimPlay();
	afx_msg void OnButtonAnimStop();
	afx_msg void OnButtonSetAnimProperty();
	afx_msg void OnButtonOpenCustDataList();
	afx_msg void OnBnClickedCheckHideCastingEffect();
	afx_msg void OnButtonAlwaysShow();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};