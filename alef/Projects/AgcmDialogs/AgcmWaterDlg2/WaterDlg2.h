#if !defined(AFX_WATERDLG2_H__BB6ADC1C_8F6F_4B57_8ECC_43D5B9926EF0__INCLUDED_)
#define AFX_WATERDLG2_H__BB6ADC1C_8F6F_4B57_8ECC_43D5B9926EF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaterDlg2.h : header file
//

#include "../resource.h"
/////////////////////////////////////////////////////////////////////////////
// WaterDlg2 dialog

class WaterDlg2 : public CDialog
{
// Construction
public:
	WaterDlg2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(WaterDlg2)
	enum { IDD = IDD_HWATER_DIALOG };
	float	m_fAlphaMax;
	float	m_fAlphaMin;
	CString	m_strBump;
	float	m_fBlue;
	float	m_fGreen;
	float	m_fRed;
	float	m_fDirX0;
	float	m_fDirX1;
	float	m_fDirX2;
	float	m_fDirX3;
	float	m_fDirY0;
	float	m_fDirY1;
	float	m_fDirY2;
	float	m_fDirY3;
	CString	m_strFresnel;
	float	m_fHeight0;
	float	m_fHeight1;
	float	m_fHeight2;
	float	m_fHeight3;
	UINT	m_iHwID;
	float	m_fReflectionWeight;
	float	m_fSpeed0;
	float	m_fSpeed1;
	float	m_fSpeed2;
	float	m_fSpeed3;
	float	m_fTexX0;
	float	m_fTexX1;
	float	m_fTexY0;
	float	m_fTexY1;
	UINT	m_iVertexPerTile;
	float	m_fAlphaDecHeight;
	float	m_fWaveDecHeight;
	float	m_fWaveMax;
	float	m_fWaveMin;
	//}}AFX_DATA
	bool	m_bDataChange;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WaterDlg2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(WaterDlg2)
	afx_msg void OnChangeHwaterEditColorBlue();
	afx_msg void OnChangeHwaterEditColorGreen();
	afx_msg void OnChangeHwaterEditColorRed();
	afx_msg void OnChangeHwaterEditDirx0();
	afx_msg void OnChangeHwaterEditDirx1();
	afx_msg void OnChangeHwaterEditDirx2();
	afx_msg void OnChangeHwaterEditDirx3();
	afx_msg void OnChangeHwaterEditDiry0();
	afx_msg void OnChangeHwaterEditDiry1();
	afx_msg void OnChangeHwaterEditDiry2();
	afx_msg void OnChangeHwaterEditDiry3();
	afx_msg void OnChangeHwaterEditHeight0();
	afx_msg void OnChangeHwaterEditHeight1();
	afx_msg void OnChangeHwaterEditHeight2();
	afx_msg void OnChangeHwaterEditHeight3();
	afx_msg void OnChangeHwaterEditId();
	afx_msg void OnChangeHwaterEditSpeed0();
	afx_msg void OnChangeHwaterEditSpeed1();
	afx_msg void OnChangeHwaterEditSpeed2();
	afx_msg void OnChangeHwaterEditSpeed3();
	afx_msg void OnChangeHwaterEditTexX0();
	afx_msg void OnChangeHwaterEditTexX1();
	afx_msg void OnChangeHwaterEditTexY0();
	afx_msg void OnChangeHwaterEditTexY1();
	afx_msg void OnHwaterButtonBump();
	afx_msg void OnHwaterButtonFresnel();
	afx_msg void OnChangeHwaterEditReflectionWeight();
	afx_msg void OnChangeHwaterEditWaveDecHeight();
	afx_msg void OnChangeHwaterEditWaveMax();
	afx_msg void OnChangeHwaterEditWaveMin();
	afx_msg void OnChangeHwaterEditAlphaDecHeight();
	afx_msg void OnChangeHwaterEditAlphaMax();
	afx_msg void OnChangeHwaterEditAlphaMin();
	afx_msg void OnChangeHwaterEditVertexpertile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//@{ Jaewon 20050706
	// ;)
	afx_msg void OnBnClickedSpecularColor();
	afx_msg void OnBnClickedBaseColor();
	// Base color red component
	float m_fBaseR;
	// Base color green component
	float m_fBaseG;
	// Base color blue component
	float m_fBaseB;
	// ps.1.4 shader index
	BOOL m_iShader14ID;
	// ps.1.1 shader index
	BOOL m_iShader11ID;
	afx_msg void OnEnChangeBaseColorR();
	afx_msg void OnEnChangeBaseColorG();
	afx_msg void OnEnChangeBaseColorB();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	//@} Jaewon
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERDLG2_H__BB6ADC1C_8F6F_4B57_8ECC_43D5B9926EF0__INCLUDED_)
