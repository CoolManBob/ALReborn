#pragma once

enum eEED_MouseMode
{
	EED_POSIT = 0,
	EED_ROTATE,
	EED_SCALE
};

class CEditEquipmentsDlg : public CDialog
{
// Construction
public:
	CEditEquipmentsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditEquipmentsDlg)
	enum { IDD = IDD_EDIT_EQUIPMENTS };
	BOOL	m_bCheckPickMode;
	float	m_fPositAt;
	float	m_fPositRight;
	float	m_fPositUp;
	int		m_nRotateAxis;
	float	m_fRotateDegree;
	float	m_fScaleRight;
	float	m_fScaleUp;
	float	m_fScaleAt;
	BOOL	m_bMouseMode;
	int		m_nMouseModeAxis;
	//}}AFX_DATA

public:
	BOOL	IsPickMode();
	BOOL	IsMouseMode();

	INT32	GetMouseModeAxis();
	INT32	GetMouseMode();

protected:
	VOID	ResetValue();
	BOOL	EnableMouseModeAxisControl(BOOL bEnable = TRUE);
	BOOL	CheckMouseModeAxisControl();
	void	KeyDown( WPARAM wParam );

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnButtonEditEquipApply();
	afx_msg void OnDestroy();
	afx_msg void OnButtonEditEquipReset();
	afx_msg void OnCheckEditEquipMouseMode();
	afx_msg void OnButtonEditEquipCopy();
	afx_msg void OnSelchangeComboMouseMode();

	DECLARE_MESSAGE_MAP()
};
