// DlgStaticTexPreview.cpp: implementation of the CDlgStaticTexPreview class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "DlgStaticTexPreview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgStaticTexPreview::CDlgStaticTexPreview(CWnd* pParent)
: CDlgStatic(pParent)
, m_sizeImage(0L, 0L)
{

}


BEGIN_MESSAGE_MAP(CDlgStaticTexPreview, CDlgStatic)
	//{{AFX_MSG_MAP(CDlgStaticTexPreview)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////
// CDlgStaticPropGrid message handlers
BOOL CDlgStaticTexPreview::OnInitDialog()
{
	CDlgStatic::OnInitDialog();

	UINT	ulStyle	= (UINT)::GetWindowLong(m_ctrlStatic.GetSafeHwnd(), GWL_STYLE);
	ulStyle	|= WS_VISIBLE;
	::SetWindowLong(m_ctrlStatic.GetSafeHwnd(), GWL_STYLE, (LONG)ulStyle);

	ulStyle = (UINT)::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	ulStyle |= WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, (LONG)ulStyle);

	m_bInit	= TRUE;
	return TRUE;
}
void CDlgStaticTexPreview::OnSize(UINT nType, int cx, int cy)
{
	if( !m_bInit )
		return;

	//CDlgStatic::OnSize(nType, cx, cy);

	vFitSize();

};

void CDlgStaticTexPreview::bChangeImg(LONG cx, LONG cy, LPCTSTR szImg)
{
	if( !m_bInit )
		return;

	if( szImg )
		SetWindowText(szImg);

	m_sizeImage.cx	= cx;
	m_sizeImage.cy	= cy;

	vFitSize();
};

void CDlgStaticTexPreview::vFitSize()
{	
	if( !m_bInit )
		return;
	CRect	rc;
	GetWindowRect(&rc);
	CRect	rcStatic(rc.left,rc.top,rc.left+m_sizeImage.cx*2,rc.top+m_sizeImage.cy*2);
	ScreenToClient(&rcStatic);
	m_ctrlStatic.MoveWindow( &rcStatic );

	
//	INT	nMin, nMax;
//	GetScrollRange(SB_VERT, &nMin, &nMax);
	if( rc.Width() > m_sizeImage.cx*2 ){
		//val &= (~(offFlag))
		UINT ulStyle = (UINT)::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
		ulStyle &= (~WS_HSCROLL)&(~ES_AUTOHSCROLL);
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, (LONG)ulStyle);
	}else{
		UINT ulStyle = (UINT)::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
		ulStyle |= WS_HSCROLL | ES_AUTOHSCROLL;
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, (LONG)ulStyle);

		SetScrollRange( SB_HORZ, 0, m_sizeImage.cx*2 - rc.Width() );
	}
	if( rc.Height() > m_sizeImage.cy*2 ){
		UINT ulStyle = (UINT)::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
		ulStyle &= (~WS_VSCROLL)&(~ES_AUTOVSCROLL);
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, (LONG)ulStyle);
	}else{
		UINT ulStyle = (UINT)::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
		ulStyle |= WS_VSCROLL | ES_AUTOVSCROLL;
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, (LONG)ulStyle);

		SetScrollRange( SB_VERT, 0, m_sizeImage.cy*2 - rc.Height());
	}
	UpdateData(FALSE);

	SetWindowPos( NULL, 0L, 0L, 0L, 0L, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
	return;
};

void CDlgStaticTexPreview::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( pScrollBar )
	{
		pScrollBar->SetScrollPos( nPos );
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgStaticTexPreview::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( pScrollBar )
	{
		pScrollBar->SetScrollPos( nPos );
	}
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
