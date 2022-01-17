#include "afxwin.h"
#if !defined(AFX_OPTIONDIALOG_H__02B55982_6359_4204_A9FB_FED4C242B678__INCLUDED_)
#define AFX_OPTIONDIALOG_H__02B55982_6359_4204_A9FB_FED4C242B678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionDialog dialog

class COptionDialog : public CDialog
{
// Construction
public:
	COptionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionDialog)
	enum { IDD = IDD_OPTION };
	BOOL	m_bUsePolygonLock;
	BOOL	m_bUseBrushPolygon;
	BOOL	m_bCameraBlocking;
	BOOL	m_bShadow2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// 오브젝트 바운딩 스피어 표시
	BOOL m_bShowObjectBoundingSphere;
	// 오브젝트 콜리젼 아토믹 표시
	BOOL m_bShowObjectCollisionAtomic;
	// 오브젝트 피킹 아토믹 표시
	BOOL m_bShowObjectPickingAtomic;
	// 오브젝트 실  포리건 정보 표시
	BOOL m_bShowObjectRealPolygon;

	// 오브젝트 창에서 옥트리 정보 표시.
	BOOL m_bShowOctreeInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONDIALOG_H__02B55982_6359_4204_A9FB_FED4C242B678__INCLUDED_)
