#pragma once

class CObjectDlg : public CDialog
{
public:
	CObjectDlg(ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CObjectDlg)
	enum { IDD = IDD_OBJECT };
	CString	m_strBaseClump;
	CString	m_strCategory;
	CString	m_strCollisionClump;
	CString	m_strTemplateName;
	CString	m_strInitAnimName;
	CString	m_strPickClump;
	int		m_lGroupIndex;
	float	m_fAnimSpeed;
	float	m_fBSphereCenterX;
	float	m_fBSphereCenterY;
	float	m_fBSphereCenterZ;
	float	m_fBSphereRadius;
	CComboBox m_ctrRidableMaterialType;
	//}}AFX_DATA

protected:
	AgcmObject*				m_pcsAgcmObject;
	AgcmObjectList*			m_pcsAgcmObjectList;

	ApdObjectTemplate*		m_pcsApdObjectTemplate;
	AgcdObjectTemplate*		m_pcsAgcdObjectTemplate;
	INT32					m_lPreGroupIndex;
	AgcdObjectTemplateGroup	m_stGroup;

protected:
	VOID		InitializeGroupIndex(BOOL	bSetMax = FALSE);
	VOID		UpdateGroupData();
	VOID		EnableGroupControl(BOOL bEnable = TRUE);

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	//{{AFX_MSG(CObjectDlg)
	afx_msg void OnButtonSetBaseClump();
	afx_msg void OnButtonSetCategory();
	afx_msg void OnButtonSetCollisionClump();
	afx_msg void OnButtonSetInitAnim();
	afx_msg void OnButtonRemoveBaseClump();
	afx_msg void OnButtonRemoveCollisionClump();
	afx_msg void OnButtonRemoveInitAnim();
	afx_msg void OnButtonGroupAdd();
	afx_msg void OnButtonGroupRemove();
	afx_msg void OnButtonRemovePickClump();
	afx_msg void OnButtonSetPickClump();
	afx_msg void OnButtonGenerateBSphere();
	afx_msg void OnSelchangeComboGroupIndex();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
