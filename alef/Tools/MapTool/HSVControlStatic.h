#if !defined(AFX_HSVCONTROLSTATIC_H__30CE755F_BEBB_4057_93F3_89D6F62772B5__INCLUDED_)
#define AFX_HSVCONTROLSTATIC_H__30CE755F_BEBB_4057_93F3_89D6F62772B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HSVControlStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHSVControlStatic window

// 데이타가 변경됐을떼 페어런트에 통보함.
#define	WM_HSVCONTROL_NOTIFY	( WM_USER + 4863 )

#define	HSV_CIRCLE_WIDTH	194
#define	HSV_CIRCLE_HEIGHT	208

// Box 위치..
#define	HSV_CIRCLE_CENTER_X			97
#define	HSV_CIRCLE_CENTER_Y			87

#define	BAN_JI_RUM					74

#define	HSV_CIRCLE_BOX_X1			53
#define	HSV_CIRCLE_BOX_Y1			43
#define	HSV_CIRCLE_BOX_X2			142
#define	HSV_CIRCLE_BOX_Y2			132
#define	HSV_CIRCLE_BOX_WIDTH		( HSV_CIRCLE_BOX_X2 - HSV_CIRCLE_BOX_X1 )
#define	HSV_CIRCLE_BOX_HEIGHT		( HSV_CIRCLE_BOX_Y2 - HSV_CIRCLE_BOX_Y1 )

#define HSV_BOX_MARGIN				6
#define HSV_BOX_SV					256
#define HSV_BOX_H					19
#define HSV_BOX_BORDER				2
#define HSV_BOX_SIZE				300

class CHSVControlStatic : public CStatic
{
// Construction
public:
	enum TYPE
	{
		BOXTYPE,		// 박스형태. 윈도우 크기를 바꾸지 않음.
		CIRCLETYPE		// 원형태.. 코렐 드로우 스타일.
	};

	enum POSITION
	{
		POS_NONE	,
		POS_HUE		,
		POS_SATVAL	,
		POS_HUESAT	,
		POS_VAL
	};

	CHSVControlStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHSVControlStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	// 초기화.
	void Init( int type , COLORREF color );
	virtual ~CHSVControlStatic();

protected:
	int		m_nType		;
	int		m_nPosition	;

	CDC		m_MemDC1	;	// BOX - HS 박스 , CIRCLE - 배경그림.
	CDC		m_MemDC2	;	// BOX - V 박스 , CIRCLE - SV박스.

	CBitmap	m_Bitmap1	;	// BOX - HS 이미지 , CIRCLE - 배경그림.
	CBitmap	m_Bitmap2	;	// BOX - V 이미지 , CIRCLE - SV박스.

	CPen	m_BlackPen	;
	CPen	m_WhitePen	;
	CBrush	m_NullBrush	;
	CFont	m_Font		;

	// 네모형태..
	int		m_nBox_H_Width;	// h 선택창의 넓이..

	CRect	m_RectHS	;
	CRect	m_RectV		;
	void	DrawHSTable			( CDC * pDC , CRect &rect	);
	void	DrawHTable			( CDC * pDC , CRect &rect	);
	void	DrawVTable			( CDC * pDC , CRect &rect	);
	void	BOX_DrawSelectBox	( CDC * pDC , CRect &rect	);

	void	BOX_LButtonDown		( UINT nFlags, CPoint point	);
	void	BOX_LButtonUp		( UINT nFlags, CPoint point	);
	void	BOX_MouseMove		( UINT nFlags, CPoint point	);

	// 원형태..

	CRect	m_RectCircle	;
	CRect	m_RectVBox		;
	CRect	m_RectPreview	;	// 프리뷰 이미지.
	CRect	m_RectRef		;	// RGB값 출력박스.
	
	void DrawSVTable			( CDC * pDC , CRect	&rect	);
	void DrawSVTable_Photoshop	( CDC * pDC , CRect	&rect	);
	void CIRCLE_DrawSelectBox	( CDC * pDC , CRect &rect	);

	void CIRCLE_LButtonDown		( UINT nFlags, CPoint point	);
	void CIRCLE_LButtonUp		( UINT nFlags, CPoint point	);
	void CIRCLE_MouseMove		( UINT nFlags, CPoint point	);

	// Double Buffering
	CDC		m_MemDCBackBuffer	;
	CBitmap	m_bmpBackBuffer		;	

public:
	// 결과물..
	int		m_nR;	// 0~255
	int		m_nG;	// 0~255
	int		m_nB;	// 0~255

	float	m_fH;	// 0.0f ~ 360.0f
	float	m_fS;	// 0.0f ~ 1.0f
	float	m_fV;	// 0.0f ~ 1.0f

	COLORREF	GetRGB()
	{
		return RGB( m_nR , m_nG , m_nB );
	}

	void	SetRGB( int r , int g , int b		);
	void	SetHSV( float h , float s , float v	);
	// Generated message map functions
protected:
	//{{AFX_MSG(CHSVControlStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

void	__DrawBorder( CDC * pDC , CRect &rect );
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHSVControlRGBInputDlg dialog

class CHSVControlRGBInputDlg : public CDialog
{
// Construction
public:
	CHSVControlRGBInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHSVControlRGBInputDlg)
	enum { IDD = IDD_HSVCONTROL_RGB_INPUT_DLG };
	int		m_nR;
	int		m_nG;
	int		m_nB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHSVControlRGBInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHSVControlRGBInputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

void hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b);
void rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSVCONTROLSTATIC_H__30CE755F_BEBB_4057_93F3_89D6F62772B5__INCLUDED_)
