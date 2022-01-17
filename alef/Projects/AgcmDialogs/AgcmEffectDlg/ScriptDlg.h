#pragma once

#define D_AGCD_EFFECT_DATA_SCRIPT_SOUND_NAME					"SoundName="
#define D_AGCD_EFFECT_DATA_SCRIPT_EID_NAME						"EID="
#define D_AGCD_EFFECT_DATA_SCRIPT_OFFSETX_NAME					"OffsetX="
#define D_AGCD_EFFECT_DATA_SCRIPT_OFFSETY_NAME					"OffsetY="
#define D_AGCD_EFFECT_DATA_SCRIPT_OFFSETZ_NAME					"OffsetZ="
#define D_AGCD_EFFECT_DATA_SCRIPT_SCALE_NAME					"Scale="
#define D_AGCD_EFFECT_DATA_SCRIPT_NODE_NAME						"Node="
#define D_AGCD_EFFECT_DATA_SCRIPT_GAP_NAME						"Gap="
#define D_AGCD_EFFECT_DATA_SCRIPT_CONDITION_NAME				"Condition="
#define D_AGCD_EFFECT_DATA_SCRIPT_CUSTOM_FLAGS					"CustFlags="
#define D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_RIGHT					"RotRight="
#define D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_UP						"RotUp="
#define D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_AT						"RotAt="

#define D_AGCD_EFFECT_CUST_DATA_ANIM_POINT_NAME					"AnimPoint="
#define D_AGCD_EFFECT_CUST_DATA_DESTROY_MATCH_NAME				"DestroyMatch="
#define D_AGCD_EFFECT_CUST_DATA_TARGET_STRUCK_ANIM_NAME			"TargetStruckAnim="
#define D_AGCD_EFFECT_CUST_DATA_EXCEPTION_TYPE_NAME				"ExtType="
#define D_AGCD_EFFECT_CUST_DATA_EXCEPTION_TYPE_CUST_DATA_NAME	"ExtTypeCustData="
#define D_AGCD_EFFECT_CUST_DATA_CHECK_TIME_ON_NAME				"CheckTimeOn="
#define D_AGCD_EFFECT_CUST_DATA_CHECK_TIME_OFF_NAME				"CheckTimeOff="
#define D_AGCD_EFFECT_CUST_DATA_TARGET_OPTION_NAME				"TargetOption="
#define D_AGCD_EFFECT_CUST_DATA_CALC_DIST_OPTION_NAME			"CalcDistOption="
#define D_AGCD_EFFECT_CUST_DATA_LINK_SKILL_NAME					"LinkSkill="
#define D_AGCD_EFFECT_CUST_DATA_DIRECTION_OPTION_NAME			"DirOption="
#define D_AGCD_EFFECT_CUST_DATA_CHECK_USER_NAME					"CheckUser="
#define D_AGCD_EFFECT_CUST_DATA_ACTION_OBJECT_NAME				"ActionObj="
#define D_AGCD_EFFECT_CUST_DATA_NATURE_CONDITION				"NatureCondition="
#define D_AGCD_EFFECT_CUST_DATA_CASTING_EFFECT					"CastingEffect="

class CScriptDlg : public CDialog
{
public:
	CScriptDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CScriptDlg)
	enum { IDD = IDD_EFFECT_SCRIPT };
	CString	m_strScript;
	//}}AFX_DATA

protected:
	AgcdUseEffectSet	*m_pstAgcdUseEffectSet;
	INT32				m_lEffectIndex;

public:
	VOID	InputString(CHAR *szString);
	VOID	UpdateScriptDlg(AgcdUseEffectSet *pstSet = NULL, INT32 lEffectIndex = -1);
	VOID	EnableSaveButton(BOOL bEnable = TRUE);

protected:	
	VOID	UpdateScript(CHAR *szHeader, CHAR *szData);
	BOOL	AddCustData(AgcdUseEffectSetData *pcsData, CHAR *pszAddCustData);

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CScriptDlg)
	afx_msg void OnDestroy();
	afx_msg void OnButtonSave();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};
