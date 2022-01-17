// RegionToolDlg.h : header file
//

#if !defined(AFX_REGIONTOOLDLG_H__77D4A208_90AE_44DC_BC91_0188DD49CA40__INCLUDED_)
#define AFX_REGIONTOOLDLG_H__77D4A208_90AE_44DC_BC91_0188DD49CA40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRegionToolDlg dialog
#include "RegionViewStatic.h"
#include "ApModule.h"
#include "ApModuleManager.h"

#include "ApmMap.h"
#include "ApmObject.h"
#include "ApmEventManager.h"
#include "AgpmEventNature.h"
#include "AgpmTimer.h"
#include "AgpmCharacter.h"
#include "AgpmEventSpawn.h"
#include "AgcmEventSpawnDlg.h"

#include "AgpmEventNature.h"
//#include "AgcmEventNature.h"
//#include "AgcmEventNatureDlg.h"

#include "RegionMenuDlg.h"

#define RM_RK_REPOSITORY					"\\\\alefserver\\RM\\RKADMIN.INI"
#define RM_RK_DIRECTORY						"\\\\59.150.48.172\\g$\\RM"
#define RM_RK_DIRECTORY_BIN					"$RK\\BIN\\"
#define RM_RK_USER_ID						"RM_ADMIN"
#define RM_RK_USER_PASSWORD					"9875"


// 마고자 (2004-11-02 오후 9:15:19) : 
// 모듈 메니져 ..
class MyEngine : public ApModuleManager
{
public:
	// ....
	MyEngine()
	{
	}

	virtual	BOOL	OnRegisterModule		();
};

// ModuleManager
extern MyEngine	g_MyEngine;

class CRegionToolDlg : public CDialog
{
public:

// Construction
public:
	CRegionToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRegionToolDlg)
	enum { IDD = IDD_REGIONTOOL_DIALOG };
	CRegionViewStatic	m_ctlRegion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionToolDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	CRegionMenuDlg	*	m_pRegionMenu;

	char	m_strWorkingFolder[ MAX_PATH ];
	CString	m_strUserName;

	BOOL DownloadResource(CHAR *pszPath);

// Implementation
public:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRegionToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//-----------------------------------------------------------
extern ApmMap				* g_pcsApmMap;
extern ApmObject			* g_pcsApmObject;
extern ApmEventManager		* g_pcsApmEventManager;
extern AgpmEventNature		* g_pcsAgpmEventNature;

extern ApmEventManagerDlg	* g_pcsApmEventManagerDlg;
extern AgpmEventSpawn		* g_pcsAgpmEventSpawn;
extern AgcmEventSpawnDlg	* g_pcsAgcmEventSpawnDlg;

extern AgpmTimer			* g_pcsAgpmTimer;
extern AgpmCharacter		* g_pcsAgpmCharacter;

extern AgpmItem				* g_pcsAgpmItem;
extern AgpmSkill			* g_pcsAgpmSkill;
extern AgpmDropItem			* g_pcsAgpmDropItem;
extern AgcmFactorsDlg		* g_pcsAgcmFactorsDlg;
extern AgpmAI2				* g_pcsAgpmAI2;

extern AgpmGrid				* g_pcsAgpmGrid;	
extern AgpmFactors			* g_pcsAgpmFactors;
extern AgpmPathFind			* g_pcsAgpmPathFind;	
//-------------------------------------------------------------

//extern AgcmEventNature		*	g_pcsAgcmEventNature	;
//extern AgcmEventNatureDlg	*	g_pcsAgcmEventNatureDlg	;

extern BOOL	g_bTeleportOnlyMode;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONTOOLDLG_H__77D4A208_90AE_44DC_BC91_0188DD49CA40__INCLUDED_)
