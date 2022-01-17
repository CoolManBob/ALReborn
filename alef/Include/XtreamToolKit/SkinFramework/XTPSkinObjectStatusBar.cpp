// XTPSkinObjectStatusBar.cpp: implementation of the CXTPSkinObjectStatusBar class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
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

#include "Common/XTPDrawHelpers.h"

#include "XTPSkinObjectStatusBar.h"
#include "XTPSkinDrawTools.h"
#include "XTPSkinManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectStatusBar, CXTPSkinObjectFrame)

CXTPSkinObjectStatusBar::CXTPSkinObjectStatusBar()
{
	m_strClassName = _T("STATUSBAR");
}

CXTPSkinObjectStatusBar::~CXTPSkinObjectStatusBar()
{

}


BEGIN_MESSAGE_MAP(CXTPSkinObjectStatusBar, CXTPSkinObjectFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectStatusBar message handlers

void CXTPSkinObjectStatusBar::RefreshMetrics()
{
	CXTPSkinObjectFrame::RefreshMetrics();

	PostMessage(WM_SETTINGCHANGE);
}

int CXTPSkinObjectStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObject::OnCreate(lpCreateStruct) == -1)
		return -1;

	SendMessage(WM_SETTINGCHANGE);

	return 0;
}
