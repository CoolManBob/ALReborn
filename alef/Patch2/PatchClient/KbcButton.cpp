// KbcButton.cpp : implementation file
//

#include "stdafx.h"
#include "KbcButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKbcButton

CKbcButton::CKbcButton()
{	
	m_bDisable			= FALSE;
	m_bCursorOnButton	= FALSE;
	m_bHover			= FALSE;
	m_bToggle			= FALSE;
	m_bClicked			= FALSE;
	m_bLButtonDown		= FALSE;

	memset( m_strButtonText, 0, sizeof(m_strButtonText) );
}

CKbcButton::~CKbcButton()
{
}

void CKbcButton::SetButtonText( char *pstrText )
{
	if( pstrText != NULL )
	{
		sprintf( m_strButtonText, "%s", pstrText );
	}
}

char *CKbcButton::GetButtonText()
{
	return m_strButtonText;
}

BEGIN_MESSAGE_MAP(CKbcButton, CButton)
	//{{AFX_MSG_MAP(CKbcButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKbcButton message handlers

void CKbcButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// LPDRAWITEMSTRUCT 로 부터 DC 얻어오고...
	CWnd			*pcWnd;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// 비활성화 상태이면 비활성화로 세팅된 버튼을 그려준다.
	pcWnd = pDC->GetWindow();

	if( !pcWnd->IsWindowEnabled() )
	{
		m_bmpImage.Draw(pDC,3,0,0,m_nMask, GetButtonText() );
	}

	// 비활성화 상태가 아니라면..
	else
	{	
		//. fake toggle.
		if(m_bToggle)
		{
			if(m_bClicked)
			{
				m_bmpImage.Draw(pDC,2,0,0,m_nMask, GetButtonText());
			}
			else
			{
				m_bmpImage.Draw(pDC,0,0,0,m_nMask, GetButtonText());
			}
		}
		else
		{
			// 버튼이 클릭 되었을때 그림
			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				m_bLButtonDown = TRUE;
				m_bmpImage.Draw(pDC,2,0,0,m_nMask, GetButtonText());
			}

			// 버튼에 커서가 올라가 있을 때 그림.
			else if ( m_bHover)
				m_bmpImage.Draw(pDC,1,0,0,m_nMask, GetButtonText());

			// 아무 상태도 아닐때 그림..
			else
			{
				m_bLButtonDown = FALSE;
				m_bmpImage.Draw(pDC,0,0,0,m_nMask, GetButtonText());
			}
		}
	}
}

void CKbcButton::SetBmpButtonImage(CString strFileName,UINT nMask)
{
	m_bmpImage.LoadBitmap(strFileName);
	m_nMask = nMask;
	

	// 우선 버튼의 크기를 계산해 놓는다.
	// 그림의 높이와 폭에서 버튼의 크기를 얻어왔다.
	m_rectButton.left	= 0;
	m_rectButton.top	= 0;
	m_rectButton.right	= m_bmpImage.GetSliceWidth();
	m_rectButton.bottom	= m_bmpImage.GetHeight();


	// 버튼의 크기를 그림에다가 맞춰주어야 한다. 
	// 높이와 폭은 그림에서 나오므로 구하기 쉽다.
	// 버튼의 left 와 top 을 알아야 하는데 밑에 처럼하면 구해진다.
	CWnd *pWnd = this->GetParent();
	GetWindowRect(&m_rectButtonPos);
	pWnd->ScreenToClient(m_rectButtonPos);
	m_rectButtonPos.right	= m_rectButtonPos.left + m_bmpImage.GetSliceWidth();
	m_rectButtonPos.bottom	= m_rectButtonPos.top  + m_bmpImage.GetHeight();

	MoveWindow(m_rectButtonPos);
}


void CKbcButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// WM_MOUSEHOVE 와 WM_MOUSELEAVE를 발생시키기 위해서 
	// _TrackMouseEvent를 호출해준다.
	// m_bCursorOnWindow 로 체크를 해준 것은 마우스가 버튼 밖에 있다가
	// 버튼 위로 포인터가 움직였을 경우 한번만 WM_MOUSEHOVER 메세지를 발생시키기 위함이다.
	// 포인터가 버튼을 떠났을 경우에도 한번만 WM_MOUSELEAVE 메세지를 발생시키기 위함.
	if( m_bCursorOnButton == FALSE )
	{
		TRACKMOUSEEVENT tme;
		ZeroMemory(&tme,sizeof(TRACKMOUSEEVENT));
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bCursorOnButton = _TrackMouseEvent(&tme);
	}
	
	CButton::OnMouseMove(nFlags, point);
}

void CKbcButton::SetBmpButtonImage(UINT32 ulResourceID ,UINT nMask)
{
	m_bmpImage.LoadBitmap(ulResourceID);
	m_nMask = nMask;
	

	// 우선 버튼의 크기를 계산해 놓는다.
	// 그림의 높이와 폭에서 버튼의 크기를 얻어왔다.
	m_rectButton.left	= 0;
	m_rectButton.top	= 0;
	m_rectButton.right	= m_bmpImage.GetSliceWidth();
	m_rectButton.bottom	= m_bmpImage.GetHeight();


	// 버튼의 크기를 그림에다가 맞춰주어야 한다. 
	// 높이와 폭은 그림에서 나오므로 구하기 쉽다.
	// 버튼의 left 와 top 을 알아야 하는데 밑에 처럼하면 구해진다.
	CWnd *pWnd = this->GetParent();
	if( pWnd )
	{
		pWnd->GetWindowRect(&m_rectButtonPos);
		pWnd->ScreenToClient(m_rectButtonPos);
	}

	m_rectButtonPos.right	= m_rectButtonPos.left + m_bmpImage.GetSliceWidth();
	m_rectButtonPos.bottom	= m_rectButtonPos.top  + m_bmpImage.GetHeight();

	MoveWindow(m_rectButtonPos);
}

LRESULT CKbcButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHover = TRUE;
	// DrawItem이 호출되라고 Invalidate()함수 부름
	Invalidate();
	return 0L;
}

LRESULT CKbcButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bCursorOnButton	= FALSE;
	m_bHover			= FALSE;
	// DrawItem이 호출되라고 Invalidate()함수 부름
	Invalidate();
	return 0L;
}

void CKbcButton::SetButtonEnable()
{
	m_bDisable = FALSE;
	// DrawItem이 호출되라고 Invalidate()함수 부름
	Invalidate();
}

void CKbcButton::SetButtonDisable()
{
	m_bDisable = TRUE;
	// DrawItem이 호출되라고 Invalidate()함수 부름
	Invalidate();
}

void CKbcButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// m_bDisable 이 TRUE 라면 	CButton::OnLButtonDown(nFlags, point) 을
	// 실행하지 않는다. 왜냐 이게 실행되면 부모에게 Notify 해주기 때문에
	// 부모가 이 버튼이 눌려 졌다는걸 알게 된다. 모르게 해야 하므로 
	// 함수를 불러주지 않는다.
	if( !m_bDisable )
		CButton::OnLButtonDown(nFlags, point);
}

BOOL CKbcButton::PreTranslateMessage(MSG* pMsg) 
{
	// 툴팁을 띄우기 위해 RelayEvent를 호출해준다.
	// 툴팁이 아직 생성되지 않았다면..
	// 즉 SetToolTipText 함수가 호출되지 않아 Create되지 않았다면
	// 건너뛴다.
	if( m_ToolTip.GetSafeHwnd() != NULL )
		m_ToolTip.RelayEvent(pMsg);
	
	return CButton::PreTranslateMessage(pMsg);
}

void CKbcButton::SetToolTipText(CString strText)
{
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(this,(LPCTSTR)strText);
}

BOOL CKbcButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
//	::SetCursor(m_hCursor);
	return TRUE;
}

void CKbcButton::SetCursor(HCURSOR hCursor)
{
//	m_hCursor = hCursor;
}

BOOL CKbcButton::OnEraseBkgnd(CDC* pDC) 
{
	// 이 함수에서 해 주는 작업은 버튼밑에 있는 바탕을 버튼바탕에다가 그려주는 것이다.
	// 만약 버튼이 올라가는 윈도에 바탕 그림이 그려져 있다면 버튼의 네모난 영역이 뒤덮어 버리기 때문에
	// 버튼 위에서 사진을 투명하게 그려줘도 사격형인 버튼 모양이 나타나게 된다.
	// 그러므로 버튼이 올라가는 윈도의, 버튼이 올라가는 그 위치의 그림을 얻어와서
	// 버튼에다가 배경으로 그려주면 투명한 것 처럼 보인다. 이 함수의 역할이다..


	// 여기서 보면 부모 윈도의 DC를 얻어 오는 것을 알 수 있다.
	CWnd *pWnd		 = this->GetParent();
	CDC  *pParentDC	 = pWnd->GetDC();

	CDC					srcDC;

	CBitmap				srcBmp;
	CBitmap				*oldsrcBmp;

	
	srcDC.CreateCompatibleDC(pParentDC);
	srcBmp.CreateCompatibleBitmap(pParentDC,m_bmpImage.GetSliceWidth(),m_bmpImage.GetHeight());
	oldsrcBmp = srcDC.SelectObject(&srcBmp);

	// 결국 부모 윈도에서 버튼이 올라가면서 덮게되는 그림을 srcDC로 가져온다.
	srcDC.BitBlt(0,0,m_bmpImage.GetSliceWidth(),m_bmpImage.GetHeight(),
		pParentDC,m_rectButtonPos.left,m_rectButtonPos.top,SRCCOPY);


	pParentDC->SelectObject(oldsrcBmp);
	ReleaseDC(pParentDC);

	pDC->BitBlt(0,0,m_bmpImage.GetSliceWidth(),m_bmpImage.GetHeight(),&srcDC,0,0,SRCCOPY);

	return TRUE;
}
