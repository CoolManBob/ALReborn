// KbcBmp.cpp : implementation file
//

#include "stdafx.h"
#include "KbcBmp.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp

IMPLEMENT_DYNCREATE(CKbcBmp, CView)

CKbcBmp::CKbcBmp()
{
	m_FontColor = RGB(255, 255, 255);
}

CKbcBmp::~CKbcBmp()
{
}

BEGIN_MESSAGE_MAP(CKbcBmp, CView)
	//{{AFX_MSG_MAP(CKbcBmp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp drawing

void CKbcBmp::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp diagnostics

#ifdef _DEBUG
void CKbcBmp::AssertValid() const
{
	CView::AssertValid();
}

void CKbcBmp::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKbcBmp message handlers

// 파일이름으로 직접 로드할 때..
void CKbcBmp::LoadBitmap(UINT nResourceID)
{
	m_Bitmap.LoadBitmap(nResourceID);
	m_Bitmap.GetBitmap(&m_BmpInfo);	

	m_nWidth		= m_BmpInfo.bmWidth;
	m_nSliceWidth	= (int)(m_BmpInfo.bmWidth/4);
	m_nHeight		= m_BmpInfo.bmHeight;


	m_nMode = 0;
}

// 파일이름으로 직접 로드할 때..
void CKbcBmp::LoadBitmap(CString strFileName)
{
	// 이미지를 불러온다.
	m_hBitmap = (HBITMAP)::LoadImage(
		AfxGetInstanceHandle(),
		strFileName,
		IMAGE_BITMAP,0,0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	// 이미지의 크기를 구하기 위함..
	(CBitmap::FromHandle(m_hBitmap))->GetBitmap(&m_BmpInfo);
	
	m_nWidth		= m_BmpInfo.bmWidth;
	m_nSliceWidth	= (int)(m_BmpInfo.bmWidth/4);
	m_nHeight		= m_BmpInfo.bmHeight;


	m_nMode = 1;
}

// 파일에서 직접 불러 올 때...
void CKbcBmp::Draw(CDC *pDC,int nX,int nY)
{
	CDC					oriDC;
	CBitmap				*poldoriDC;

	UINT BMPWIDTH		= m_nWidth;
	UINT BMPHEIGHT		= m_nHeight;

	// 원본 이미지를 oriDC에다가 복사해준다.
	// 그것을 srcDC에다가 복사해서 써야된다. 안그러면 원본 이미지가 상하는 일이 발생한다. 
	// 이미지 조작 전에는 로드 해놓고 그것의 복사본을 만들어 놓고 거기서 조작하자..
	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject( m_nMode ? CBitmap::FromHandle(m_hBitmap) : &m_Bitmap );

	pDC->BitBlt( nX, nY,BMPWIDTH,BMPHEIGHT,&oriDC,0,0,SRCCOPY);	 

	oriDC.SelectObject(poldoriDC);
}

// 파일에서 직접 불러 올 때...
void CKbcBmp::Draw(CDC *pDC,int nX,int nY,UINT nMask)
{
	UINT BLACK			= RGB(0,0,0);
	UINT WHITE			= RGB(255,255,255);
	UINT BMPWIDTH		= m_nWidth;
	UINT BMPHEIGHT		= m_nHeight;
	UINT MASK			= nMask;

	CDC					oriDC;
	CDC					srcDC;
	CDC					maskDC;
	CDC					memDC;
	CBitmap				srcBmp;
	CBitmap				maskBmp;	
	CBitmap				memBmp;
	CBitmap				*poldoriDC;
	CBitmap				*poldsrcDC;
	CBitmap				*poldmaskDC;
	CBitmap				*poldmemDC;
	
	// 원본 이미지를 oriDC에다가 복사해준다.
	// 그것을 srcDC에다가 복사해서 써야된다. 안그러면 원본 이미지가 상하는 일이 발생한다. 
	// 이미지 조작 전에는 로드 해놓고 그것의 복사본을 만들어 놓고 거기서 조작하자..
	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject( m_nMode ? CBitmap::FromHandle(m_hBitmap) : &m_Bitmap );

	// oriDC 이미지를 srcDC 에다가 복사해준다..
	srcDC.CreateCompatibleDC(pDC);
	srcBmp.CreateCompatibleBitmap(pDC,BMPWIDTH,BMPHEIGHT);
	poldsrcDC = srcDC.SelectObject(&srcBmp);
	srcDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&oriDC,0,0,SRCCOPY);

	// 마스크를 가지는 DC, 
	// mask는 그림이 그려질 부분은 Black, 배경은 White로 표시된 그림이다.
	maskDC.CreateCompatibleDC(pDC);
	maskBmp.CreateBitmap(BMPWIDTH,BMPHEIGHT,1,1,NULL);
	poldmaskDC = maskDC.SelectObject(&maskBmp);

	// 전송할(source) 그림의 배경을 지정해놓고 BitBlt하면 
	// 배경색은 흰색, 아닌것은 모두 검정색으로 전송을 해준다.
	// 전송을 해서 SRCCOPY하면 mask 이미지가 완성된다.
	srcDC.SetBkColor(MASK);
	maskDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&srcDC,0,0,SRCCOPY);


	// 전송을 받을 DC의 배경과 전경을 지정해 놓고 BitBlt 하면
	// 전송하는 그림의 White 는 전송 받을 DC의 Background의 색으로 변환 되고
	// 전송하는 그림의 Black 은 전송 받을 DC의 Foreground의 색으로 변환 된다.
	// 이렇게 되면 maskDC 의 그림과는 반전되는 그림이 나오게 된다.
	// 이것을 SRCAND연산을 하면 기존의 srcDC 에 있는 그림과 겹쳐지면서
	// 원본 그림의 배경은 검정색이 되버리고 아닌것은 원본 그대로 표시가 된다.
	srcDC.SetBkColor(BLACK);
	srcDC.SetTextColor(WHITE);
	srcDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);

	// 이제 모든 준비가 되었다. 우선 배경(이것은 원래 화면에 표시되던 배경으로 우리가 올리고자 하는 이미지의 배경이 아니다.)
	// 배경을 저장해 놓는다. memDC에..		
	memDC.CreateCompatibleDC(pDC);		
	memBmp.CreateCompatibleBitmap(pDC,BMPWIDTH,BMPHEIGHT);
	poldmemDC = memDC.SelectObject(&memBmp);
	memDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,pDC, nX, nY,SRCCOPY);
	
	// 그런다음 maskDC를 AND 연산으로 원래 화면의 배경과 합치면 
	// 그림부분만 검정색으로 표시되고 배경색은 살아있게 된다. 
	// 여기에다가 srcDC의 이미지를 OR 연산인 SRCPATINT 로 전송을 하게 되면
	// 배경색이 그대로 살아있는 이미지가 출력된다.
	memDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);
	memDC.BitBlt(0,0,BMPWIDTH,BMPHEIGHT,&srcDC,0,0,SRCPAINT);

	pDC->BitBlt( nX, nY,BMPWIDTH,BMPHEIGHT,&memDC,0,0,SRCCOPY);	 

	oriDC.SelectObject(poldoriDC);
	srcDC.SelectObject(poldsrcDC);
	maskDC.SelectObject(poldmaskDC);
	memDC.SelectObject(poldmemDC);
}

// CKbcBmpButton 에서 호출할 때 쓰는 함수.. 그림 네장이 연속적으로 붙은 것을 그려준다.nSeq에 따라..
void CKbcBmp::Draw(CDC *pDC,int nSeq,int nX,int nY,UINT nMask, char *pstrText)
{
	UINT BLACK			= RGB(0,0,0);
	UINT WHITE			= RGB(255,255,255);
	UINT BMPWIDTH		= m_nWidth;
	UINT BMPHEIGHT		= m_nHeight;
	UINT MASK			= nMask;
	UINT SLICEWIDTH		= m_nSliceWidth;
	UINT INTERVAL		= SLICEWIDTH * nSeq;

	CDC					oriDC;
	CDC					srcDC;
	CDC					maskDC;
	CDC					memDC;
	CBitmap				srcBmp;
	CBitmap				maskBmp;	
	CBitmap				memBmp;
	CBitmap				*poldoriDC;
	CBitmap				*poldsrcDC;
	CBitmap				*poldmaskDC;
	CBitmap				*poldmemDC;

	// 원본 이미지.. 4개 버튼의 그림이 다 들어 있는 것을 불러온다.
	// oriDC에는 전체 사이즈의 그림이 다 들어있다.
	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject(m_nMode ? CBitmap::FromHandle(m_hBitmap) : &m_Bitmap);

	// oriDC 의 1/4 이미지를 srcDC 에다가 복사해준다..
	srcDC.CreateCompatibleDC(pDC);
	srcBmp.CreateCompatibleBitmap(pDC,SLICEWIDTH,BMPHEIGHT);
	poldsrcDC = srcDC.SelectObject(&srcBmp);
	srcDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&oriDC,INTERVAL,0,SRCCOPY);

	// 마스크를 가지는 DC, 
	// mask는 그림이 그려질 부분은 Black, 배경은 White로 표시된 그림이다.
	maskDC.CreateCompatibleDC(pDC);
	maskBmp.CreateBitmap(SLICEWIDTH,BMPHEIGHT,1,1,NULL);
	poldmaskDC = maskDC.SelectObject(&maskBmp);

	// 전송할(source) 그림의 배경을 지정해놓고 BitBlt하면 
	// 배경색은 흰색, 아닌것은 모두 검정색으로 전송을 해준다.
	// 전송을 해서 SRCCOPY하면 mask 이미지가 완성된다.
	srcDC.SetBkColor(MASK);
	maskDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&srcDC,0,0,SRCCOPY);

	// 전송을 받을 DC의 배경과 전경을 지정해 놓고 BitBlt 하면
	// 전송하는 그림의 White 는 전송 받을 DC의 Background의 색으로 변환 되고
	// 전송하는 그림의 Black 은 전송 받을 DC의 Foreground의 색으로 변환 된다.
	// 이렇게 되면 maskDC 의 그림과는 반전되는 그림이 나오게 된다.
	// 이것을 SRCAND연산을 하면 기존의 srcDC 에 있는 그림과 겹쳐지면서
	// 원본 그림의 배경은 검정색이 되버리고 아닌것은 원본 그대로 표시가 된다.
	srcDC.SetBkColor(BLACK);
	srcDC.SetTextColor(WHITE);
	srcDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);

	// 이제 모든 준비가 되었다. 우선 배경(이것은 원래 화면에 표시되던 배경으로 우리가 올리고자 하는 이미지의 배경이 아니다.)
	// 배경을 저장해 놓는다. memDC에..		
	memDC.CreateCompatibleDC(pDC);		
	memBmp.CreateCompatibleBitmap(pDC,SLICEWIDTH,BMPHEIGHT);
	poldmemDC = memDC.SelectObject(&memBmp);
	memDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,pDC, nX, nY,SRCCOPY);
	
	// 그런다음 maskDC를 AND 연산으로 원래 화면의 배경과 합치면 
	// 그림부분만 검정색으로 표시되고 배경색은 살아있게 된다. 
	// 여기에다가 srcDC의 이미지를 OR 연산인 SRCPATINT 로 전송을 하게 되면
	// 배경색이 그대로 살아있는 이미지가 출력된다.
	memDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&maskDC,0,0,SRCAND);
	memDC.BitBlt(0,0,SLICEWIDTH,BMPHEIGHT,&srcDC,0,0,SRCPAINT);

	pDC->BitBlt( nX, nY,SLICEWIDTH,BMPHEIGHT,&memDC,0,0,SRCCOPY);	 

	if( pstrText != NULL )
	{
		RECT		cRect;

		cRect.left = nX;
		cRect.top = nY;
		cRect.right = m_nWidth/4;
		cRect.bottom = m_nHeight + nY;

		pDC->SetTextColor( m_FontColor );
		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( pstrText, &cRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}

	oriDC.SelectObject(poldoriDC);
	srcDC.SelectObject(poldsrcDC);
	maskDC.SelectObject(poldmaskDC);
	memDC.SelectObject(poldmemDC);
}
