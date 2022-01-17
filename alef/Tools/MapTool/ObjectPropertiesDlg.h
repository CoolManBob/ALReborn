#if !defined(AFX_OBJECTPROPERTIESDLG_H__A82543D3_7BBC_41C8_B5EF_9386E226E959__INCLUDED_)
#define AFX_OBJECTPROPERTIESDLG_H__A82543D3_7BBC_41C8_B5EF_9386E226E959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectPropertiesDlg.h : header file
//

#include "MyEngine.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDlg dialog

class CObjectPropertiesDlg : public CDialog
{
// Construction
public:
	ApdObject *		m_pcsObject;
	AgcdObject *	m_pstAgcdObject;

	void SetObject(ApdObject *pcsObject, AgcdObject *pstAgcdObject);
	CObjectPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectPropertiesDlg)
	enum { IDD = IDD_OBJECT_PROPERTIES };
	BOOL	m_bStatic;
	float	m_fPositionX;
	float	m_fPositionY;
	float	m_fPositionZ;
	float	m_fScaleX;
	float	m_fScaleY;
	float	m_fScaleZ;
	float	m_fDegreeX;
	float	m_fDegreeY;
	BOOL	m_bBridge;
	BOOL	m_bWall;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectPropertiesDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTPROPERTIESDLG_H__A82543D3_7BBC_41C8_B5EF_9386E226E959__INCLUDED_)
