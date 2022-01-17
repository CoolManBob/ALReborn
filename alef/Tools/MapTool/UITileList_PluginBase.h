#if !defined(AFX_UITILELIST_PLUGINBASE_H__3A408ABC_639D_4FB6_AFFC_E4F31F4DFB69__INCLUDED_)
#define AFX_UITILELIST_PLUGINBASE_H__3A408ABC_639D_4FB6_AFFC_E4F31F4DFB69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_PluginBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUITileList_PluginBase window

class ApWorldSector;

class CUITileList_PluginBase : public CWnd
{
public:
	struct PBOption
	{
		bool	bShowSphere			;
		bool	bExcludeWhenExport	;

		PBOption():	bShowSphere			( false ),
					bExcludeWhenExport	( false ){}
	};

	PBOption	m_stOption;
	CString		m_strShortName;
	
// Construction
public:
	CUITileList_PluginBase();

	BOOL	m_bSave;
	void	SetSaveData		() { m_bSave = TRUE; }
	void	ClearSaveFlag	() { m_bSave = FALSE; }

// Virtual..
	virtual	BOOL OnSelectedPlugin		(){ return TRUE; }
	virtual	BOOL OnDeSelectedPlugin		(){ return TRUE; }

	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos ){ return TRUE; }
	virtual	BOOL OnLButtonUpGeometry	( RwV3d * pPos ){ return TRUE; }
	virtual	BOOL OnLButtonDblClkGeometry( RwV3d * pPos ){ return TRUE; }
	virtual	BOOL OnRButtonDownGeometry	( RwV3d * pPos ){ return TRUE; }
	virtual	BOOL OnRButtonUpGeometry	( RwV3d * pPos ){ return TRUE; }
	virtual	BOOL OnRButtonDblClkGeometry( RwV3d * pPos ){ return TRUE; }

	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos ){ return TRUE; }

	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms ){ return FALSE; }	// 처리하지 않음을 의미함..
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms ){ return FALSE; }
	virtual	BOOL Window_OnLButtonDblClk	( RsMouseStatus *ms ){ return FALSE; }
	virtual	BOOL Window_OnRButtonDown	( RsMouseStatus *ms ){ return FALSE; }
	virtual	BOOL Window_OnRButtonUp		( RsMouseStatus *ms ){ return FALSE; }
	virtual	BOOL Window_OnRButtonDblClk	( RsMouseStatus *ms ){ return FALSE; }
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms ){ return FALSE; }
	virtual BOOL Window_OnMouseWheel	( BOOL bForward		){ return FALSE; }

	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks ){ return FALSE; }

	virtual	BOOL OnSelectObject			( RpClump * pClump ){ return FALSE; };

	virtual BOOL OnWindowRender			(){ return TRUE;}
	virtual BOOL OnIdle					( UINT32 ulClockCount ){ return TRUE;}

	virtual BOOL OnQuerySaveData		( char * pStr ){ return FALSE; }
	virtual BOOL OnSaveData				(){ return TRUE; }
	virtual BOOL OnLoadData				(){ return TRUE; }
	virtual	BOOL OnCleanUpData			(){ return TRUE; }

	virtual BOOL OnExport				( BOOL bServer ){ return FALSE; }

	// 업데이트용 ..
	
	// 지형이 바뀌었을때..
	virtual void OnChangeSectorGeometry	( ApWorldSector * pSector ){}
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_PluginBase)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUITileList_PluginBase();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileList_PluginBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_PLUGINBASE_H__3A408ABC_639D_4FB6_AFFC_E4F31F4DFB69__INCLUDED_)
