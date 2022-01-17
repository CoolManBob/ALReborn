#pragma once

class CRenderOption
{
public:
	BOOL	m_bShowCameraPos;
	BOOL	m_bShowCollisionClump;
	BOOL	m_bShowPickClump;	
	BOOL	m_bShowBlocking;
	BOOL	m_bShowFPS;
	BOOL	m_bShowClumpPolygons;
	BOOL	m_bShowClumpVertices;
	BOOL	m_bShowTemp;
	BOOL	m_bShowWireFrame;
	BOOL	m_bShowBoundingSphere;
	BOOL	m_bMatD3DFx;
	BOOL	m_bShowTextBoarder;
	BOOL	m_bShowHitRange;
	BOOL	m_bShowSiegeWarCollObj;
	BOOL	m_bEnableSound;
	INT		m_iKeyFrameRate;
	BOOL	m_bCheckSaveFile;
	CString	m_strTerrain;
	DWORD	m_dwResourceTreeWidth;

	CRenderOption()
	{
		m_bShowCameraPos		= FALSE;
		m_bShowCollisionClump	= FALSE;
		m_bShowPickClump		= FALSE;		
		m_bShowBlocking			= FALSE;
		m_bShowFPS				= FALSE;
		m_bShowClumpPolygons	= FALSE;
		m_bShowClumpVertices	= FALSE;
		m_bShowTemp				= FALSE;
		m_bShowWireFrame		= FALSE;
		m_bShowBoundingSphere	= FALSE;
		m_bMatD3DFx				= FALSE;
		m_bShowTextBoarder		= FALSE;
		m_iKeyFrameRate			= 0;
		m_bCheckSaveFile		= FALSE;
		m_bShowHitRange			= FALSE;
		m_bShowSiegeWarCollObj	= FALSE;
		m_bEnableSound			= TRUE;
		m_strTerrain			= AMT_BOTTOM_CLUMP_PATH_NAME;
		m_dwResourceTreeWidth	= 0;
	}
};

class CRenderOptionDlg : public CDialog
{
// Construction
public:
	CRenderOptionDlg(CRenderOption *pcsOption, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRenderOptionDlg)
	
	enum { IDD = IDD_RENDER_OPTION };

	CRenderOption m_CurrentRenderOption;

	//}}AFX_DATA

protected :
	CRenderOption* m_pcsDestRenderOption;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderOptionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenderOptionDlg)
	virtual void OnOK();
	virtual void OnBtnClickedTerrainTexture();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bMatD3DFx;
	BOOL	m_bShowTextBoarder;

public:	
	int		m_iKeyFrameRate;
	BOOL	m_bCheckSaveFile;
};