// XTPCommandBarAnimation.cpp : implementation file
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

#include "stdafx.h"

#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPCommandBarAnimation.h"
#include "XTPCommandBar.h"
#include "XTPPaintManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPCommandBarAnimation::CAnimateInfo::CAnimateInfo()
{
	hbmSrc = 0;
	pSrcBits = 0;
	hbmSrcBack = 0;
	pSrcBackBits = 0;
	SetRectEmpty(&rcPaint);
	m_nAnimation = 0;
}

CXTPCommandBarAnimation::CAnimateInfo::~CAnimateInfo()
{
	DeleteObject(hbmSrc);
	DeleteObject(hbmSrcBack);
}

CXTPCommandBarAnimation::CXTPCommandBarAnimation(CXTPCommandBar* pCommandBar)
{
	m_bInvalidate = FALSE;
	m_pParent = pCommandBar;
	m_nTimer = 0;

	m_bAnimation = FALSE;
	m_bDoubleBuffer = FALSE;
}

CXTPCommandBarAnimation::~CXTPCommandBarAnimation()
{
	RemoveAnimations();
}

void CXTPCommandBarAnimation::RemoveAnimations()
{
	for (int i = 0; i < m_arrAnimation.GetSize(); i++)
	{
		delete m_arrAnimation[i];
	}

	m_arrAnimation.RemoveAll();

	if (m_nTimer && m_pParent->GetSafeHwnd())
	{
		m_pParent->KillTimer(XTP_TID_ANIMATION);
	}
	m_nTimer = 0;
}

void CXTPCommandBarAnimation::AlphaBlendU(PBYTE pDest, PBYTE pSrcBack, int cx, int cy, PBYTE pSrc, BYTE byAlpha)
{
	const BYTE byDiff = (BYTE)(255 - byAlpha);

	for (int i = 0; i < cx * cy * 4; i++)
	{
		pDest[0] = (BYTE)((pSrcBack[0] * byDiff + pSrc[0] * byAlpha) >> 8);

		pDest += 1;
		pSrcBack += 1;
		pSrc += 1;
	}
}

#define MAXANIMATION 6

BOOL CXTPCommandBarAnimation::IsAnimationEnabled() const
{
	return m_bAnimation || m_pParent->GetPaintManager()->m_bEnableAnimation;
}

void CXTPCommandBarAnimation::AnimateRect(CClientDC& dc, CAnimateInfo* pai)
{
	CRect rcPaint = pai->rcPaint;
	int cx = rcPaint.Width();
	int cy = rcPaint.Height();

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	if (pai->m_nAnimation == MAXANIMATION)
	{
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(memDC, m_bmpCache);

		dc.BitBlt(rcPaint.left, rcPaint.top, cx, cy,
			&memDC, rcPaint.left, rcPaint.top, SRCCOPY);

		::SelectObject(memDC, hOldBitmap);
	}
	else
	{
		BYTE* pDestBits = NULL;
		HBITMAP hbmDest = CXTPImageManager::Create32BPPDIBSection(dc, cx, cy, &pDestBits);

		AlphaBlendU(pDestBits, pai->pSrcBackBits, cx, cy, pai->pSrcBits, (BYTE)(255 * pai->m_nAnimation / MAXANIMATION));

		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(memDC, hbmDest);

		dc.BitBlt(rcPaint.left, rcPaint.top, cx, cy, &memDC, 0, 0, SRCCOPY);

		::SelectObject(memDC, hOldBitmap);
		DeleteObject(hbmDest);
	}

	pai->m_nAnimation++;
}

void CXTPCommandBarAnimation::OnAnimate()
{
	CClientDC dc(m_pParent);

	for (int i = 0; i < m_arrAnimation.GetSize(); i++)
	{
		CAnimateInfo* pai = m_arrAnimation[i];

		for (int j = 0; j < pai->arrExclude.GetSize(); j++)
			dc.ExcludeClipRect(&pai->arrExclude[j]);

		AnimateRect(dc, pai);

		dc.SelectClipRgn(NULL);

		if (pai->m_nAnimation > MAXANIMATION)
		{
			m_arrAnimation.RemoveAt(i);
			delete pai;
			i--;
		}
	}

	if (m_arrAnimation.GetSize() == 0 && m_nTimer)
	{
		m_pParent->KillTimer(XTP_TID_ANIMATION);
		m_nTimer = 0;
	}
}

void CXTPCommandBarAnimation::RemoveIntersections(LPCRECT rcPaint, BOOL bAddExclude)
{
	for (int i = 0; i < m_arrAnimation.GetSize(); i++)
	{
		CAnimateInfo* paiOld = m_arrAnimation[i];

		CRect rcIntersect;
		if (rcIntersect.IntersectRect(&paiOld->rcPaint, rcPaint))
		{
			if (rcIntersect == paiOld->rcPaint)
			{
				m_arrAnimation.RemoveAt(i);
				delete paiOld;
				i--;
			}
			else if (bAddExclude)
			{
				paiOld->arrExclude.Add((RECT&)*rcPaint);
			}
		}
	}
}

void CXTPCommandBarAnimation::AddAnimation(CClientDC& dc, CAnimateInfo* pai)
{
	RemoveIntersections(&pai->rcPaint, FALSE);

	m_arrAnimation.Add(pai);

	if (m_nTimer == 0)
	{
		m_nTimer = m_pParent->SetTimer(XTP_TID_ANIMATION, 50, NULL);
	}

	AnimateRect(dc, pai);
}

void CXTPCommandBarAnimation::OnDestroy()
{
	m_bmpCache.DeleteObject();
	RemoveAnimations();
}

void CXTPCommandBarAnimation::RedrawRect(LPCRECT lpRect, BOOL bAnimate)
{
	if (lpRect == NULL)
	{
		m_bmpCache.DeleteObject();
	}

	if (lpRect == NULL || m_bmpCache.GetSafeHandle() == 0 || !IsAnimationEnabled())
	{
		m_bInvalidate = TRUE;
		m_pParent->InvalidateRect(lpRect, FALSE);
		return;
	}

	CClientDC dc(m_pParent);
	CXTPClientRect rc(m_pParent);
	CRect rcPaint(lpRect);

	CDC dcDraw;
	dcDraw.CreateCompatibleDC(&dc);

	CBitmap bmpDraw;
	bmpDraw.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	HBITMAP hOldBitmapDraw = (HBITMAP)::SelectObject(dcDraw, bmpDraw);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(rcPaint);
	dcDraw.SelectClipRgn(&rgn);

	m_pParent->DrawCommandBar(&dcDraw, rcPaint);

	dcDraw.SelectClipRgn(0);

	CDC dcCache;
	dcCache.CreateCompatibleDC(&dc);
	HBITMAP hOldBitmapCache = (HBITMAP)::SelectObject(dcCache, m_bmpCache);

	if (!bAnimate)
	{
		RemoveIntersections(&rcPaint, TRUE);

		dcCache.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
			&dcDraw, rcPaint.left, rcPaint.top, SRCCOPY);

		dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
			&dcDraw, rcPaint.left, rcPaint.top, SRCCOPY);
	}
	else
	{
		CAnimateInfo* pai = new CAnimateInfo;
		pai->rcPaint = rcPaint;
		pai->m_nAnimation = 1;
		pai->hbmSrc = CXTPImageManager::Create32BPPDIBSection(dc, rcPaint.Width(), rcPaint.Height(), &pai->pSrcBits);
		pai->hbmSrcBack = CXTPImageManager::Create32BPPDIBSection(dc, rcPaint.Width(), rcPaint.Height(), &pai->pSrcBackBits);

		CDC dcSrc;
		dcSrc.CreateCompatibleDC(&dc);

		CDC dcSrcBack;
		dcSrcBack.CreateCompatibleDC(&dc);

		HBITMAP hOldBitmapSrc = (HBITMAP)::SelectObject(dcSrc, pai->hbmSrc);
		HBITMAP hOldBitmapSrcBack = (HBITMAP)::SelectObject(dcSrcBack,  pai->hbmSrcBack);

		dcSrc.BitBlt(0, 0, rcPaint.Width(), rcPaint.Height(), &dcDraw, rcPaint.left, rcPaint.top, SRCCOPY);

		dcSrcBack.BitBlt(0, 0, rcPaint.Width(), rcPaint.Height(), &dcCache, rcPaint.left, rcPaint.top, SRCCOPY);

		dcCache.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(), &dcSrc, 0, 0, SRCCOPY);

		::SelectObject(dcSrcBack, hOldBitmapSrcBack);
		::SelectObject(dcSrc, hOldBitmapSrc);

		if (memcmp(pai->pSrcBits, pai->pSrcBackBits, rcPaint.Width() * rcPaint.Height() * 4) == 0)
		{
			delete pai;
		}
		else
		{
			AddAnimation(dc, pai);
		}
	}

	::SelectObject(dcCache, hOldBitmapCache);
	::SelectObject(dcDraw, hOldBitmapDraw);
}

void CXTPCommandBarAnimation::OnPaint(CPaintDC& paintDC)
{
	CXTPClientRect rc(m_pParent);
	CRect rcPaint(paintDC.m_ps.rcPaint);

	if (m_bmpCache.GetSafeHandle())
	{
		BITMAP bmpInfo;
		m_bmpCache.GetBitmap(&bmpInfo);

		if (bmpInfo.bmHeight != rc.Height() || bmpInfo.bmWidth != rc.Width())
			m_bmpCache.DeleteObject();
	}

	if (!m_bDoubleBuffer && !IsAnimationEnabled())
	{
		m_bmpCache.DeleteObject();

		CXTPBufferDC memDC(paintDC);
		m_pParent->DrawCommandBar(&memDC, rcPaint);

		RemoveAnimations();
	}
	else if (m_bInvalidate || m_bmpCache.GetSafeHandle() == 0)
	{
		CDC memDC;
		memDC.CreateCompatibleDC(&paintDC);

		if (!m_bmpCache.GetSafeHandle())
		{
			m_bmpCache.CreateCompatibleBitmap(&paintDC, rc.Width(), rc.Height());
			rcPaint = rc;
		}

		CBitmap* pOldBitmap = memDC.SelectObject(&m_bmpCache);

		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcPaint);
		memDC.SelectClipRgn(&rgn);

		m_pParent->DrawCommandBar(&memDC, rcPaint);

		memDC.SelectClipRgn(0);

		paintDC.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(),
			&memDC, rcPaint.left, rcPaint.top, SRCCOPY);

		memDC.SelectObject(pOldBitmap);

		RemoveAnimations();
	}
	else
	{
		CXTPCompatibleDC memDC(&paintDC, &m_bmpCache);
		paintDC.BitBlt(0, 0, rc.right, rc.bottom, &memDC, 0, 0, SRCCOPY);
	}

	m_bInvalidate = FALSE;
}
