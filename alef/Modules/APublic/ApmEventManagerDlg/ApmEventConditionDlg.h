#if !defined(AFX_APMEVENTCONDITIONDLG_H__80F2CA2C_EE05_400F_ADFD_0602FB8C0E1E__INCLUDED_)
#define AFX_APMEVENTCONDITIONDLG_H__80F2CA2C_EE05_400F_ADFD_0602FB8C0E1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApmEventConditionDlg.h : header file
//

#include "resource.h"
#include "ApmEventManager.h"

/////////////////////////////////////////////////////////////////////////////
// ApmEventConditionDlg dialog

class ApmEventConditionDlg : public CDialog
{
private:
	ApdEvent *			m_pstEvent;
	ApmEventManager *	m_pcsApmEventManager;
	AgpmItem *			m_pcsAgpmItem;

	ApdEventArea		m_eAreaType;

// Construction
public:
	void SetTarget(BOOL bEnable);
	void SetArea(BOOL bEnable);
	void SetEnvironment(BOOL bEnable);
	void SetTime(BOOL bEnable);
	void InitData(ApdEvent *pstEvent, ApmEventManager *pcsApmEventManager, AgpmItem *pcsAgpmItem);
	ApmEventConditionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ApmEventConditionDlg)
	enum { IDD = IDD_EVENT_CONDITION };
	CEdit	m_csAreaBoxSupZ;
	CEdit	m_csAreaBoxSupY;
	CEdit	m_csAreaBoxSupX;
	CEdit	m_csAreaBoxInfZ;
	CEdit	m_csAreaBoxInfY;
	CEdit	m_csAreaBoxInfX;
	CEdit	m_csAreaFanAngle;
	CEdit	m_csAreaFanRadius;
	CEdit	m_csAreaSphereRadius;
	CListBox	m_csTargetItems;
	float	m_fAreaSphereRadius;
	float	m_fAreaFanRadius;
	float	m_fAreaFanAngle;
	float	m_fAreaBoxInfX;
	float	m_fAreaBoxInfY;
	float	m_fAreaBoxInfZ;
	float	m_fAreaBoxSupX;
	float	m_fAreaBoxSupY;
	float	m_fAreaBoxSupZ;
	BOOL	m_bTarget;
	BOOL	m_bArea;
	BOOL	m_bEnvironment;
	BOOL	m_bTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ApmEventConditionDlg)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ApmEventConditionDlg)
	afx_msg void OnTarget();
	afx_msg void OnArea();
	afx_msg void OnEnvironment();
	afx_msg void OnTime();
	afx_msg void OnEventConditionTargetFactor();
	afx_msg void OnEventConditionTargetItemAdd();
	afx_msg void OnEventConditionTargetItemRemove();
	afx_msg void OnEventConditionAreaSphere();
	afx_msg void OnEventConditionAreaFan();
	afx_msg void OnEventConditionAreaBox();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APMEVENTCONDITIONDLG_H__80F2CA2C_EE05_400F_ADFD_0602FB8C0E1E__INCLUDED_)
