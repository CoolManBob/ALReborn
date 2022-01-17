#if !defined(AFX_SKYTEMPLATEPREVIEWDLG_H__6F10C231_5556_4BAD_B625_92F54180AD69__INCLUDED_)
#define AFX_SKYTEMPLATEPREVIEWDLG_H__6F10C231_5556_4BAD_B625_92F54180AD69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkyTemplatePreviewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkyTemplatePreviewDlg dialog

class CSkyTemplatePreviewDlg : public CDialog
{
// Construction
public:
	CSkyTemplatePreviewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSkyTemplatePreviewDlg)
	enum { IDD = IDD_SKY_TEMPLATE_PREVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkyTemplatePreviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Attribute
	CBitmap	m_Bitmap;
	CDC		m_MemDC	;
	CRect	m_Rect	;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSkyTemplatePreviewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKYTEMPLATEPREVIEWDLG_H__6F10C231_5556_4BAD_B625_92F54180AD69__INCLUDED_)
