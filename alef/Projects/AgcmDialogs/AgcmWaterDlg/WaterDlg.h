#if !defined(AFX_WATERDLG_H__B2F16319_5D82_4907_8E2B_1EA8C7231ADF__INCLUDED_)
#define AFX_WATERDLG_H__B2F16319_5D82_4907_8E2B_1EA8C7231ADF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaterDlg.h : header file
//
#include "../resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWaterDlg dialog

//typedef enum AgcmWaterDlgCallbackPoint
//{
//	AGCMWATERDLG_CB_ID_CHANGESTATUS = 0
//} AgcmWaterDlgCallbackPoint;

class CWaterDlg : public CDialog
{
// Construction
public:
	CWaterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaterDlg)
	enum { IDD = IDD_WATER_DLG };
	int		m_iStatusID;
	int		m_iWaterType;
	int		m_iSelBOP_L1;
	int		m_iSelBMode_L1;
	CString	m_strWaterFile_L1;
	int		m_iRed_L1;
	int		m_iGreen_L1;
	int		m_iBlue_L1;
	int		m_iAlpha_L1;
	float	m_fU_L1;
	float	m_fV_L1;
	int		m_iTileSize_L1;
	int		m_iSelBMode_L2;
	CString	m_strWaterFile_L2;
	float	m_fU_L2;
	float	m_fV_L2;
	CString	m_strWaveFile;
	int		m_iWaveWidth;
	int		m_iWaveHeight;
	float	m_fWaveLifeTime;
	float	m_fWaveScaleX;
	float	m_fWaveScaleZ;
	int		m_iWaveRed;
	int		m_iWaveGreen;
	int		m_iWaveBlue;
	int		m_iWaveMinNum;
	int		m_iWaveMaxNum;
	//}}AFX_DATA

	bool	m_bDataChange;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaterDlg)
	afx_msg void OnButton_L1File();
	afx_msg void OnButton_L1Color();
	afx_msg void OnButton_L2File();
	afx_msg void OnButton_WaveFile();
	afx_msg void OnButton_WaveColor();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit_StatusID();
	afx_msg void OnChangeEdit_L1WaterFile();
	afx_msg void OnChangeEdit_L1Red();
	afx_msg void OnChangeEdit_L1Green();
	afx_msg void OnChangeEdit_L1Blue();
	afx_msg void OnChangeEdit_L1Alpha();
	afx_msg void OnChangeEdit_L1U();
	afx_msg void OnChangeEdit_L1V();
	afx_msg void OnChangeEdit_L1TileSize();
	afx_msg void OnChangeEdit_L2WaterFile();
	afx_msg void OnChangeEdit_L2U();
	afx_msg void OnChangeEdit_L2V();
	afx_msg void OnChangeEdit_WaveFile();
	afx_msg void OnChangeEdit_WaveWidth();
	afx_msg void OnChangeEdit_WaveHeight();
	afx_msg void OnChangeEdit_WaveLifeTime();
	afx_msg void OnChangeEdit_WaveScaleX();
	afx_msg void OnChangeEdit_WaveScaleZ();
	afx_msg void OnChangeEdit_WaveRed();
	afx_msg void OnChangeEdit_WaveGreen();
	afx_msg void OnChangeEdit_WaveBlue();
	afx_msg void OnChangeEdit_WaveMinNum();
	afx_msg void OnChangeEdit_WaveMaxNum();
	afx_msg void OnBmodeRadio1();
	afx_msg void OnBmodeRadio2();
	afx_msg void OnBmodeRadio3();
	afx_msg void OnBmodeRadio4();
	afx_msg void OnBmodeRadio5();
	afx_msg void OnBmodeRadio6();
	afx_msg void OnBmodeRadio7();
	afx_msg void OnBmodeRadio8();
	afx_msg void OnBmodeRadio9();
	afx_msg void OnBmodeRadio10();
	afx_msg void OnBmodeRadio11();
	afx_msg void OnBelndopRadio0();
	afx_msg void OnBelndopRadio1();
	afx_msg void OnBelndopRadio2();
	afx_msg void OnWaterTypeRadio0();
	afx_msg void OnWaterTypeRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERDLG_H__B2F16319_5D82_4907_8E2B_1EA8C7231ADF__INCLUDED_)
