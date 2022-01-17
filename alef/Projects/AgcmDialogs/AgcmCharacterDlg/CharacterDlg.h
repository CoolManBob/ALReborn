#pragma once

class CCharacterDlg : public CDialog
{
public:
	CCharacterDlg(AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_CHARACTER };
	CString	m_strArmourClump;
	CString	m_strBaseClump;
	CString	m_strTemplateName;
	UINT	m_unHeight;
	UINT	m_unRiderHeight;		// 말탔을때 높이
	UINT	m_unHitRange;
	CString	m_strPickAtomic;
	float	m_fBSphereCenterX;
	float	m_fBSphereCenterY;
	float	m_fBSphereCenterZ;
	float	m_fBSphereRadius;
	float	m_fScale;
	float	m_fDepth;
	int		m_lNodeIndex[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
	BOOL	m_bTagging;
	float	m_fSiegeWarfareCollisionBoxWidth;
	float	m_fSiegeWarfareCollisionBoxHeight;
	float	m_fSiegeWarfareCollisionSphereRadius;
	float	m_fSiegeWarfareCollisionOffsetX;
	float	m_fSiegeWarfareCollisionOffsetZ;
	UINT	m_lLookAtNode;
	BOOL	m_bUseBending;
	float	m_fBendingFactor;
	BOOL	m_bSelfDestructionAttackType;
	BOOL	m_bNonPickingType;

protected:
	AgpdCharacterTemplate *m_pcsAgpdCharacterTemplate;
	AgcdCharacterTemplate *m_pcsAgcdCharacterTemplate;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	
	afx_msg void OnButtonSetArmourClump();
	afx_msg void OnButtonSetBaseClump();
	afx_msg void OnButtonSetCharacterHeight();
	afx_msg void OnButtonSetCharacterHitRange();
	afx_msg void OnButtonSetPickAtomic();
	afx_msg void OnButtonRemoveArmourClump();
	afx_msg void OnButtonRemoveBaseClump();
	afx_msg void OnButtonRemovePickAtomic();
	afx_msg void OnButtonGenerateBSphere();
	afx_msg void OnButtonCustomFlags();
	afx_msg void OnBnClickedButtonSetRiderHeight();
	afx_msg void OnBnClickedButtonSetCharacterDepth();
	afx_msg void OnBnClickedCheck1();	

	DECLARE_MESSAGE_MAP()

};
