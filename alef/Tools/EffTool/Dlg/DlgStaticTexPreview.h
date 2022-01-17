// DlgStaticTexPreview.h: interface for the CDlgStaticTexPreview class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGSTATICTEXPREVIEW_H__A18CBEE6_69FF_4EB8_A2DD_3AE6340A57F6__INCLUDED_)
#define AFX_DLGSTATICTEXPREVIEW_H__A18CBEE6_69FF_4EB8_A2DD_3AE6340A57F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgStatic.h"

class CDlgStaticTexPreview : public CDlgStatic  
{
	CSize	m_sizeImage;

	void	vFitSize();
public:
	void	bChangeImg(LONG cx, LONG cy, LPCTSTR szImg=NULL);
	HWND	bGetHwndStatic(void) { return(GetSafeHwnd() ? m_ctrlStatic.GetSafeHwnd() : NULL); };
public:
	CDlgStaticTexPreview(CWnd* pParent = NULL);

protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgStaticPropGrid)
	virtual	BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DLGSTATICTEXPREVIEW_H__A18CBEE6_69FF_4EB8_A2DD_3AE6340A57F6__INCLUDED_)
