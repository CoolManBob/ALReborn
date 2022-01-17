#if !defined(AFX_KBCBUTTON_H__41D0AB53_20C4_4079_B3E2_3E8B6B337CD5__INCLUDED_)
#define AFX_KBCBUTTON_H__41D0AB53_20C4_4079_B3E2_3E8B6B337CD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KbcButton.h : header file
//

#include "KbcBmp.h"

/////////////////////////////////////////////////////////////////////////////
// CKbcButton window
// CKbcButton 사용법
// DDX_Control 을 사용해서 쓰고자 하는 곳에서 CKbcButton으로 연결해준다음.. 
// SetBmpButtonImage("test.bmp",RGB(0,0,255)); 를 호출하면 된다....
//
// 제작 - 곽병찬(kbckbc@postech.ac.kr | http://kbckbc.com)
// 혹시라도 사용하시는 분께선 클래스 이름이나마 유지해주시기 바랍니다.
// 그래도 저의 노력이 담긴 거란걸 이해해주시기 바랍니다.

class CKbcButton : public CButton
{
// Construction
public:
	CKbcButton();

	// 투명 비트맵을 그려주는 변수
	CKbcBmp			m_bmpImage;
	UINT			m_nMask;
	char			m_strButtonText[255];

	// 비트맵으로 해볼라고
	void			SetBmpButtonImage(CString,UINT);
	void			SetBmpButtonImage(UINT32,UINT);

	// 버튼을 활성화/비활성화 시킨다.
	void			SetButtonEnable();
	void			SetButtonDisable();

	// 툴팁 메세지를 보인다.
	void			SetToolTipText(CString);

	// 버튼위에서 커서를 바꾼다.
	void			SetCursor(HCURSOR);

	//. 2006. 4. 11. nonstopdj
	//. fake toggle btn.
	BOOL			m_bToggle;
	BOOL			m_bClicked;
	BOOL			m_bLButtonDown;

protected:
	// 활성화인지 비활성화인지 잡고 있는 변수
	// 초기값은 FALSE로 활성화란 뜻이다.
	BOOL			m_bDisable;


	// 커서가 버튼위에 있느냐 없느냐 판단
	// WM_MOUSEHOVER 랑 WM_MOUSELEAVE 메세지를 한번씩만 날려주기 위해 쓴다.
	BOOL			m_bCursorOnButton;


	// 지금 버튼이 하버 상태이면 하버된 그림을 그린다.
	// DrawItem 쪽에서 쓴다.
	BOOL			m_bHover;


	// m_rectButton 는 버튼의 크기 0,0,10,10 이런 식의 값이다.
	// m_rectButtonPos 은 버튼이 올라가 있는 다이얼 로그에서 위치값이다.
	CRect			m_rectButton;
	CRect			m_rectButtonPos;

	// 툴팁 메세지를 위한 것~~
	CToolTipCtrl	m_ToolTip;


	// 사용자가 커서를 세팅할수도 있는데 그때 커서를 잡고 있는 변수
	HCURSOR			m_hCursor;	

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKbcButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKbcButton();
	void SetButtonText( char *pstrText );
	char *GetButtonText();

	// Generated message map functions
protected:
	//{{AFX_MSG(CKbcButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KBCBUTTON_H__41D0AB53_20C4_4079_B3E2_3E8B6B337CD5__INCLUDED_)
