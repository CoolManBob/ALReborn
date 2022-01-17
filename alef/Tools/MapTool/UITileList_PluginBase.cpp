// UITileList_PluginBase.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "UITileList_PluginBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUITileList_PluginBase

CUITileList_PluginBase::CUITileList_PluginBase() : m_bSave( FALSE ) , m_strShortName( "NoName" )
{
}

CUITileList_PluginBase::~CUITileList_PluginBase()
{
}


BEGIN_MESSAGE_MAP(CUITileList_PluginBase, CWnd)
	//{{AFX_MSG_MAP(CUITileList_PluginBase)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileList_PluginBase message handlers
