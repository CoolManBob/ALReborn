#pragma once

#include "../resource.h"
#include "AgcmLODDlg.h"

class CLODDlg : public CDialog
{
public:
	CLODDlg(AgcdPreLOD *pstPreLOD, AgcdDefaultLODInfo *pstInfo, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLODDlg)
	enum { IDD = IDD_LOD_DLG };
	CListBox	m_csLODDisplay;
	BOOL	m_bBillboard;
	BOOL	m_bTransformGeom;
	BOOL	m_bUseAtomicIndex;
	int		m_lLODIndex;
	BOOL	m_bYAxis;
	int		m_nDistanceType;
	UINT	m_unMaxDistance;
	//}}AFX_DATA

protected:
	AgcdPreLOD			m_csAgcdPreLOD;
	AgcdDefaultLODInfo*	m_pstLODInfo;

	INT32				m_lPreLODIndex;

protected:
	VOID				EnableCheckButton(BOOL bEnable = TRUE);
	VOID				UpdateLODDisplay();
	VOID				ReCalcLODDisplayHorizon();
	BOOL				InitializeLODIndex();
	BOOL				InitializeDistanceType();
	BOOL				UpdateDistanceType();

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(CLODDlg)
	afx_msg void OnSelchangeComboLoddlgIndex();
	afx_msg void OnSelchangeListLodDlg();
	afx_msg void OnDestroy();
	afx_msg void OnCheckBillboard();
	afx_msg void OnCheckTransformGeometry();
	afx_msg void OnCheckYaxis();
	afx_msg void OnButtonApplyPreLod();
	afx_msg void OnButtonChangeDistance();
	afx_msg void OnButtonChangeLoddata();
	afx_msg void OnButtonInsertLoddata();
	afx_msg void OnButtonRemoveLoddata();
	afx_msg void OnButtonInsertDummy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};