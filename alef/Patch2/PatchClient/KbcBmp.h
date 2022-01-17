#if !defined(AFX_KBCBMP_H__3B18A7EA_F8CB_4410_8E1E_AA0B72F039BB__INCLUDED_)
#define AFX_KBCBMP_H__3B18A7EA_F8CB_4410_8E1E_AA0B72F039BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KbcBmp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp view
//
// 제작 - 곽병찬(kbckbc@postech.ac.kr | http://kbckbc.com)
// 혹시라도 사용하시는 분께선 클래스 이름이나마 유지해주시기 바랍니다.
// 그래도 저의 노력이 담긴 거란걸 이해해주시기 바랍니다.

class CKbcBmp : public CView
{
public:
	void				LoadBitmap(CString);	// 파일 이름이 직접 들어올 때..
	void				LoadBitmap(UINT);		// 리소스가 들어올 때..

	// Draw - 비트맵 그려주는 함수이다. 보면 알겠쥐?
	// Draw(CDC *pDC,int nX,int nY)						- 그냥 비트맵 그려줄 때..
	// Draw(CDC *pDC,int nX,int nY,UINT nMask)			- 배경색 안보이게 비트맵 그려줄 때..
	// Draw(CDC *pDC,int nSeq,int nX,int nY,UINT nMask)	- KbcButton 에서 호출되는 함수..
	void				Draw(CDC *pDC,int nX,int nY);	
	void				Draw(CDC *pDC,int nX,int nY,UINT nMask);	
	void				Draw(CDC *pDC,int nSeq,int nX,int nY,UINT nMask, char *pstrText);	
	/////////////////////////////////////////////////// 

	int					GetWidth()		 { return m_nWidth; }
	int					GetSliceWidth()	 { return m_nSliceWidth; }
	int					GetHeight()		 { return m_nHeight; }
	void				SetFontColor(COLORREF color){ m_FontColor = color; }

protected:
	HBITMAP				m_hBitmap; 
	BITMAP				m_BmpInfo; 
	CBitmap				m_Bitmap;

	int					m_nWidth;
	int					m_nSliceWidth;
	int					m_nHeight;

	int					m_nMode;

	COLORREF			m_FontColor;

protected:
	
	DECLARE_DYNCREATE(CKbcBmp)
	
// Attributes
public:
	CKbcBmp();           // protected constructor used by dynamic creation
	virtual ~CKbcBmp();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKbcBmp)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CKbcBmp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KBCBMP_H__3B18A7EA_F8CB_4410_8E1E_AA0B72F039BB__INCLUDED_)
