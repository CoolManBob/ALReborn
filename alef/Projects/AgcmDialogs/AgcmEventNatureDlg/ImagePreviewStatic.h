#if !defined(AFX_IMAGEPREVIEWSTATIC_H__2F49920E_4212_4612_98FD_7EF8F8376D7E__INCLUDED_)
#define AFX_IMAGEPREVIEWSTATIC_H__2F49920E_4212_4612_98FD_7EF8F8376D7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImagePreviewStatic.h : header file
//

#include "bmp.h"
/////////////////////////////////////////////////////////////////////////////
// CImagePreviewStatic window

#include "AgcmEventNatureDlg.h"

#define	WM_IMAGEPREVIEW_NOTIFY_EVENT	( WM_USER + 13514 )

class CImagePreviewStatic : public CStatic
{
// Construction
public:
	AgcmEventNatureDlg *m_pEventNatureDlg	;
	CImagePreviewStatic();

// Attributes
public:
	CBmp	m_Bmp		;
// Operations
public:
	INT32	m_nIndex	;
	INT32	GetIndex() { return m_nIndex; }
	INT32	SetIndex( INT32 nIndex , char * pFilename = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagePreviewStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImagePreviewStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImagePreviewStatic)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPREVIEWSTATIC_H__2F49920E_4212_4612_98FD_7EF8F8376D7E__INCLUDED_)
