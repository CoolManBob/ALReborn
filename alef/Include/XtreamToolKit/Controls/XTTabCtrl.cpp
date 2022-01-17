// XTTabCtrl.cpp : implementation of the CXTTabCtrl class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions
#include "Common/XTPSystemHelpers.h"

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTVC50Helpers.h"
#include "XTTabCtrlButtons.h"
#include "XTTabBase.h"
#include "XTTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTTabCtrl

IMPLEMENT_THEME_REFRESH(CXTTabCtrl, CTabCtrl)
IMPLEMENT_DYNAMIC(CXTTabCtrl, CTabCtrl)

BEGIN_MESSAGE_MAP(CXTTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CXTTabCtrl)
	ON_TABCTRLEX_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTTabCtrl construction/destruction

CXTTabCtrl::CXTTabCtrl()
{
	ImplAttach(this);
}

CXTTabCtrl::~CXTTabCtrl()
{

}

BOOL CXTTabCtrl::EnableToolTips(BOOL bEnable)
{
	return CXTTabCtrlBase::EnableToolTipsEx(bEnable);
}
