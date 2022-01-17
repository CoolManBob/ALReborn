#pragma once

class CObjectTypeDlg : public CDialog
{
// Construction
public:
	CObjectTypeDlg(INT32 *plObjectType, RwRGBA *pstRGBA, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectTypeDlg)
	enum { IDD = IDD_OBJECT_TYPE };
	BOOL	m_bAlpha;
	BOOL	m_bAmbient;
	BOOL	m_bBlocking;
	BOOL	m_bLight;
	BOOL	m_bNoCameraAlpha;
	BOOL	m_bPreLight;
	BOOL	m_bRidable;
	UINT	m_unAlpha;
	UINT	m_unBlue;
	UINT	m_unGreen;
	UINT	m_unRed;
	BOOL	m_bFade;
	BOOL	m_bIsSystemObject;
	BOOL	m_bShadow;
	BOOL	m_bAlphaFunc;
	BOOL	m_bUDA;
	BOOL	m_bOccluder;
	BOOL	m_bDonotCull;
	CComboBox m_ctlCamCollision;
	//}}AFX_DATA

protected:
	INT32		*m_plObjectType;
	RwRGBA		*m_pstRGBA;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CObjectTypeDlg)
	afx_msg void OnCheckPreLight();
	afx_msg void OnCbnSelchangeComboCamCollision();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
