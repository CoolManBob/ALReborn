#ifndef __CUSTOM_PROGRESS_BAR_H__
#define __CUSTOM_PROGRESS_BAR_H__

#include "Resource.h"

class CCustomStatic
{
private:
	wchar_t				m_strText[999];
	RECT				m_cRect;

public:
	CCustomStatic();
	~CCustomStatic();
	bool init( RECT *pcRect );
	bool SetText( wchar_t *pstrText, CWnd *pcWnd );
	wchar_t *GetText();
	bool DrawText( CDC *pDC );
	RECT *GetRect();
};

class CCustomProgressCtrl : public CView
{
private:
	CRITICAL_SECTION	m_cCriticalSection;

	CBitmap				m_cBMPProgress;
	CBitmap				m_cBMPProgress1;
	CBitmap				m_cBMPProgress2;

	HBITMAP				m_hBitmap; 
	BITMAP				m_BmpInfo; 
	CBitmap				m_Bitmap;

	RECT				m_cRect;
	CDC					*m_hpParentDC;

	int					m_iMaxCount;
	int					m_iCurrentCount;
	int					m_nWidth;
	int					m_nSliceWidth;
	int					m_nHeight;

	int					m_nMode;

public:
	CCustomProgressCtrl();
	~CCustomProgressCtrl();

	void lock();
	void unlock();

	bool SetProgressRect( RECT *pcRect );

	void OnDraw(CDC* pDC);
	void LoadBitmap(CString strFileName);
	void Draw(CDC *pDC);

	bool Init( RECT *pcRect, int iMaxCount );
	bool SetCurrentCount( int iCount );
	bool SetMaxCount( int iMaxCount );
	int GetCurrentCount();
	int GetMaxCount();
	RECT *GetRect();
	DECLARE_MESSAGE_MAP()
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#endif