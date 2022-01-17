#pragma once

#include "ApBase.h"

class AgcmEditBlockingDlg : public CDialog
{
public:
	AgcmEditBlockingDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(AgcmEditBlockingDlg)
	enum { IDD = IDD_EDIT_BLOCKING_INFO };
	CButton		m_csBtNone;
	CComboBox	m_csCurIndex;
	CButton		m_csBtUp;
	CButton		m_csBtScale;
	CButton		m_csBtRight;
	CButton		m_csBtTranslate;
	CButton		m_csBtHeight;
	CButton		m_csBtAt;
	CButton		m_csBtAll;
	CString		m_strBlockingType;
	float		m_fEditX;
	float		m_fEditY;
	float		m_fEditZ;
	//}}AFX_DATA

	BOOL		m_bClick;

protected:
	BOOL SetBlockingType();
	VOID InitializeEditControl();
	VOID InitializeEditControl_None();
	VOID InitializeEditControl_Box();
	VOID InitializeEditControl_Sphere();
	VOID InitializeEditControl_Cylinder();
	VOID InitializeEditControl_Translate();

public:
	BOOL	IsNone(); // 사용자 정의.
	BOOL	IsScale();
	BOOL	IsTranslate();
	BOOL	IsRight();
	BOOL	IsUp();
	BOOL	IsAt();
	BOOL	IsAll();
	BOOL	IsHeight();

	BOOL	IsClick()	{	return m_bClick;	}
	BOOL	ClearEdit();

	// 내부적으로 쓰임...
	VOID	InitaliizeBlockingList();
	void	SetPos( float fX, float fY, float fZ );
	void	KeyDown( WPARAM wParam );

public:
	virtual BOOL Create(CWnd *pParentWnd = NULL);

protected:
	//Override
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//Message Map
	afx_msg void OnCheckEditBlockingScale();
	afx_msg void OnCheckEditBlockingPosit();
	afx_msg void OnCheckEditBlockingRight();
	afx_msg void OnCheckEditBlockingUp();
	afx_msg void OnCheckEditBlockingAt();
	afx_msg void OnCheckEditBlockingAll();
	afx_msg void OnCheckEditBlockingHeight();
	afx_msg void OnSelchangeComboCurIndex();
	afx_msg void OnButtonChangeType();
	afx_msg void OnCheckEditBlockingNone();
	afx_msg void OnClose();
	afx_msg void OnButtonInsert();
	afx_msg void OnButtonCopyAndInsert();
	afx_msg void OnButtonDelete();
	afx_msg void OnBnClickedBlockUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
