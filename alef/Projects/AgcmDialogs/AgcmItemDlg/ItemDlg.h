#pragma once

#define ITEM_DLG_TYPE_EQUIP			"Equip"
#define ITEM_DLG_TYPE_USABLE		"Usable"
#define ITEM_DLG_TYPE_OTHER			"Other"

#define ITEM_DLG_KIND_ARMOUR		"Armour"
#define ITEM_DLG_KIND_WEAPON		"Weapon"
#define ITEM_DLG_KIND_SHIELD		"Shield"
#define ITEM_DLG_KIND_RING			"Ring"
#define ITEM_DLG_KIND_NECKLACE		"Necklace"
#define ITEM_DLG_KIND_RIDE			"Ride"

#define ITEM_DLG_OTHER_SKULL		"Skull"
#define ITEM_DLG_OTHER_MONEY		"Money"

#define ITEM_DLG_SS_FIRE			"Fire"
#define ITEM_DLG_SS_AIR				"Air"
#define ITEM_DLG_SS_WATER			"Water"
#define ITEM_DLG_SS_EARTH			"Earth"
#define ITEM_DLG_SS_MAGIC			"Magic"
#define ITEM_DLG_SS_ICE				"Ice"
#define ITEM_DLG_SS_LIGHTENING		"Lightening"
#define ITEM_DLG_SS_POISON			"Poison"
#define ITEM_DLG_SS_CON				"CON"
#define ITEM_DLG_SS_WIS				"WIS"
#define ITEM_DLG_SS_DEX				"DEX"
#define ITEM_DLG_SS_STR				"STR"
#define ITEM_DLG_SS_INT				"INT"

class CItemDlg : public CDialog
{
public:
	CItemDlg(AgpdItemTemplate *pcsAgpdItemTemplae, AgcdItemTemplate *pcsAgcdItemTemplate, CWnd* pParent = NULL);   // standard constructor

	void		SetWeaponTypeSelectable(bool bEnable) { m_bWeaponTypeSelectable = bEnable; }

	//{{AFX_DATA(CItemDlg)
	enum { IDD = IDD_ITEM };
	CComboBox	m_csItemSpiritStone;
	CComboBox	m_csItemUsable;
	CComboBox	m_csItemOther;
	CComboBox	m_csItemWeaponType;
	CComboBox	m_csItemType;
	CComboBox	m_csItemPart;
	CComboBox	m_csItemKind;
	CComboBox	m_ctlLightSystem;
	BOOL		m_bEquipTwoHands;
	BOOL		m_bWithFace;
	BOOL		m_bWithHair;
	BOOL		m_bAvatarItem;
	CString		m_strBaseClump;
	CString		m_strFieldClump;
	CString		m_strSecondClump;
	CString		m_strUIImage;
	CString		m_strTemplateName;
	CString		m_strFirstColor;
	CString		m_strSecondColor;
	UINT		m_unStackableNum;
	CString		m_strPickClump;
	CString		m_strSmallUIImage;
	float		m_fBSphereCenterX;
	float		m_fBSphereCenterY;
	float		m_fBSphereCenterZ;
	float		m_fBSphereRadius;
	DWORD		m_uGap;
	//}}AFX_DATA

protected:
	AgpdItemTemplate*	m_pcsAgpdItemTemplate;
	AgcdItemTemplate*	m_pcsAgcdItemTemplate;

	bool		m_bWeaponTypeSelectable;

protected:
	VOID		SetItemKind();

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	// Generated message map functions
	//{{AFX_MSG(CItemDlg)
	afx_msg void OnButtonSetBaseClump();
	afx_msg void OnButtonSetFieldClump();
	afx_msg void OnButtonSetSecondClump();
	afx_msg void OnButtonSetSmallUiImage();
	afx_msg void OnButtonSetUiImage();
	afx_msg void OnSelchangeComboItemType();
	afx_msg void OnSelchangeComboItemPart();
	afx_msg void OnSelchangeComboItemWtype();
	afx_msg void OnSelchangeComboItemKind();
	afx_msg void OnButtonRemoveBaseClump();
	afx_msg void OnButtonRemoveFieldClump();
	afx_msg void OnButtonRemoveSecondClump();
	afx_msg void OnButtonRemoveSmallUiImage();
	afx_msg void OnButtonRemoveUiImage();
	afx_msg void OnSelchangeComboItemUsableType();
	afx_msg void OnButtonRemovePickClump();
	afx_msg void OnButtonGenerateBsphere();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};