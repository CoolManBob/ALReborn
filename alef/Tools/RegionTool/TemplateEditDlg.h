#include "afxwin.h"
#if !defined(AFX_TEMPLATEEDITDLG_H__6DDE33C3_87F9_4BED_A60F_7BCA347FC080__INCLUDED_)
#define AFX_TEMPLATEEDITDLG_H__6DDE33C3_87F9_4BED_A60F_7BCA347FC080__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateEditDlg dialog

class CTemplateEditDlg : public CDialog
{
// Construction
public:
	CTemplateEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateEditDlg)
	enum { IDD = IDD_TEMPLATE };
	CComboBox	m_ctlTypeCombo;
	CString	m_strComment	;
	int		m_nIndex		;
	CString	m_strName		;
//	int		m_nPriority		;
	CComboBox m_ctlSafety;
	BOOL m_bRidable			;
	BOOL m_bPet				;
	BOOL m_bItemWangBok	;
	BOOL m_bItemPotion		;
	BOOL m_bItemResurrect	;
	BOOL m_bDisableMinimap	;
	BOOL m_bJail			;
	BOOL m_bCharacterBlock	;
	BOOL m_bItemPositonType2;
	BOOL m_bItemPositonGuild;
	CString m_strWorldMap	;
	//}}AFX_DATA
	int		m_nType		;
	int		m_nSafety	;

	int		m_nWorldMapIndex;
	int		m_nDefaultSkySet;

	void	UpdateWorldMap();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void	SetTemplate( ApmMap::RegionTemplate * pTemplate );
protected:
	ApmMap::RegionTemplate * m_pTemplate;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateEditDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelectworldmap();
	CString m_strSkySet;
	afx_msg void OnBnClickedSelectskyset();
	BOOL m_bZoneLoading;
	float m_fSrcX;
	float m_fSrcZ;
	float m_fSrcHeight;
	int m_nRadius;
	afx_msg void OnBnClickedZoneloading();
	float m_fDstX;
	float m_fDstZ;
public:
	int m_nVDistance;
	int	m_nLevelLimit;
	int	m_nLevelMin;

public:
	afx_msg void OnBnClickedChoiceregion();
public:
	int m_nParentIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEEDITDLG_H__6DDE33C3_87F9_4BED_A60F_7BCA347FC080__INCLUDED_)
