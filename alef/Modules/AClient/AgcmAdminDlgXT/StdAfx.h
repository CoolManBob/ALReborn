// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2E8C7874_0460_4155_ABDF_B853D5C59668__INCLUDED_)
#define AFX_STDAFX_H__2E8C7874_0460_4155_ABDF_B853D5C59668__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxmt.h>			// MFC Multi-Thread

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



///////////////////////////////////////////////////////////////
// User Define
// GDI+
#include <gdiplus.h>

// Extreme ToolKit Pro
#include <XTToolkitPro.h>
#include <PropertyGrid/XTPPropertyGridIncludes.h>

#define ADMIN_CONFIG_FILENAME	"INI/AdminConfig.ini"
#define ADMIN_CONFIG_SECTION_MOVE_PLACE	"MovePlace"

#define MASTERY_STRING "Mastery"
#define MOVE_PLACE_NAME_LENGTH	32

#define TEXTURE_ITEM_BASIC_PATH		"Texture\\Item\\";
#define TEXTURE_SKILL_BASIC_PATH	"Texture\\Skill\\";

typedef enum eTextureType
{
	TEXTURE_TYPE_ITEM = 0,
	TEXTURE_TYPE_SKILL,
} eTextureType;

// STD
#include <list>

// ApDefine
#include "ApDefine.h"

// AuIniManager
#include "AuIniManager.h"

// ApgdAdmin
#include "AgpdAdmin.h"

// API
#include "AgcmAdminDlgXT.h"
#include "AgcmAdminDlgXT_API.h"

// Dialog
#include "AgcmAdminDlgXT_Main.h"
#include "AgcmAdminDlgXT_Search.h"
#include "AgcmAdminDlgXT_Character.h"
#include "AgcmAdminDlgXT_Help.h"
#include "AgcmAdminDlgXT_Move.h"
#include "AgcmAdminDlgXT_Ban.h"
#include "AgcmAdminDlgXT_Item.h"
#include "AgcmAdminDlgXT_AdminList.h"
#include "AgcmAdminDlgXT_Notice.h"
#include "AgcmAdminDlgXT_CharItem.h"

#include "AgcmAdminDlgXT_Game.h"

// Message Queue
#include "AgcmAdminDlgXT_IOCP.h"
#include "AgcmAdminDlgXT_MessageQueue.h"

// Resource Loader
#include "AgcmAdminDlgXT_ResourceLoader.h"

// Manager
#include "AgcmAdminDlgXT_IniManager.h"
#include "AgcmAdminDlgXT_Manager.h"


//////////////////////////////////////////////////////////////////
// Class definition for Property Grid::SetValue, For Extreme ToolKit Pro
class CXTPSetValueGridItem : public CXTPPropertyGridItem
{
public:
	CXTPSetValueGridItem(CString strCaption, CString strValue);

	virtual void SetValue(CString strValue);
};

class AgcdAdminMovePlace
{
public:
	char m_szPlaceName[MOVE_PLACE_NAME_LENGTH+1];
	float m_fX, m_fY, m_fZ;
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2E8C7874_0460_4155_ABDF_B853D5C59668__INCLUDED_)
