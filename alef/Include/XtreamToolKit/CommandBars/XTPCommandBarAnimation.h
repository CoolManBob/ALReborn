// XTPCommandBarAnimation.h: interface for the CXTPCommandBarAnimation class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPRIBBONANIMATION_H__)
#define __XTPRIBBONANIMATION_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE
class CXTPCommandBar;

#define XTP_TID_ANIMATION 0xACD43

class _XTP_EXT_CLASS CXTPCommandBarAnimation
{
protected:
	struct CAnimateInfo
	{
		CAnimateInfo();
		~CAnimateInfo();

		HBITMAP hbmSrc;
		BYTE* pSrcBits;

		HBITMAP hbmSrcBack;
		BYTE* pSrcBackBits;

		RECT rcPaint;
		int m_nAnimation;
		CArray<RECT, RECT&> arrExclude;
	};

public:
	CXTPCommandBarAnimation(CXTPCommandBar* pCommandBar);
	~CXTPCommandBarAnimation();

public:
	void OnPaint(CPaintDC& paintDC);
	void RedrawRect(LPCRECT lpRect, BOOL bAnimate);
	void OnAnimate();
	void OnDestroy();

protected:
	void RemoveAnimations();
	void AnimateRect(CClientDC& dc, CAnimateInfo* pai);
	void RemoveIntersections(LPCRECT rcPaint, BOOL bAddExclude);

	void AlphaBlendU(PBYTE pDest, PBYTE pSrcBack, int cx, int cy, PBYTE pSrc, BYTE byAlpha);
	void AddAnimation(CClientDC& dc, CAnimateInfo* pai);

	BOOL IsAnimationEnabled() const;

public:
	CXTPCommandBar* m_pParent;
	CBitmap m_bmpCache;
	BOOL m_bInvalidate;
	UINT_PTR m_nTimer;

	BOOL m_bAnimation;
	BOOL m_bDoubleBuffer;


protected:
	CArray<CAnimateInfo*, CAnimateInfo*> m_arrAnimation;
};

//}}AFX_CODEJOCK_PRIVATE


#endif // !defined(__XTPRIBBONANIMATION_H__)
