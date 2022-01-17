#pragma once


// CMoreButton

class CMoreButton : public CButton
{
	DECLARE_DYNAMIC(CMoreButton)

public:
	CMoreButton();
	virtual ~CMoreButton();

protected:
	DECLARE_MESSAGE_MAP()

	INT		m_nStatus;
	BOOL	m_bCursorOnButton;


	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PreSubclassWindow();

	LRESULT OnMouseHover( WPARAM wParam , LPARAM lParam );
	LRESULT OnMouseLeave( WPARAM wPAram , LPARAM lParam );
};


