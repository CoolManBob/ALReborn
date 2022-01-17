#if !defined(AFX_ROUGHMAPGENERATEDLG_H__E4D0E4C6_80FA_4349_9377_C10DB7BE50CD__INCLUDED_)
#define AFX_ROUGHMAPGENERATEDLG_H__E4D0E4C6_80FA_4349_9377_C10DB7BE50CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RoughMapGenerateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRoughMapGenerateDlg dialog

class CRoughMapGenerateDlg : public CDialog
{
// Construction
public:
	CRoughMapGenerateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRoughMapGenerateDlg)
	enum { IDD = IDD_GENERATE_ROUGH };
	float	m_fOffset;
	BOOL	m_bHeightGenerate;
	BOOL	m_bTileGenerate;
	BOOL	m_bVertexColorGenerate;
	BOOL	m_bApplyAlpha;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoughMapGenerateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoughMapGenerateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROUGHMAPGENERATEDLG_H__E4D0E4C6_80FA_4349_9377_C10DB7BE50CD__INCLUDED_)
