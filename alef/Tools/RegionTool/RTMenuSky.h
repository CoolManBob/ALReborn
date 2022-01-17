#if !defined(AFX_RTMENUSKY_H__25E8B275_FA58_4F46_81B1_1C65EC008C9E__INCLUDED_)
#define AFX_RTMENUSKY_H__25E8B275_FA58_4F46_81B1_1C65EC008C9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RTMenuSky.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRTMenuSky window
class CRTMenuBase;

#define SKY_MESSAGE_LINE 2
class CRTMenuSky : public CRTMenuBase
{
// Construction
public:
	CRTMenuSky();

protected:
	BOOL	m_bUnsavedData	;
	CString	m_strMessage[ SKY_MESSAGE_LINE ]	;

// Attributes
public:

// Operations
public:
	char *	GetTemplateName( INT32 nTemplateIndex );
	virtual BOOL	Create( CRect * pRect , CWnd * pParent );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTMenuSky)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual	char *	GetMenuName();
	virtual ~CRTMenuSky();
	virtual void OnPaintOnMap		( CDC * pDC );
	virtual BOOL OnLButtonDownMap	( CPoint &point , int x , int z );
	virtual BOOL OnLButtonUpMap		( CPoint &point , int x , int z );
	virtual BOOL OnMouseMoveMap		( CPoint &point , int x , int z );
	virtual BOOL OnLButtonDblDownMap( CPoint &point , int x , int z );

	virtual	BOOL OnDeleteKeyDown();
	virtual BOOL OnQuerySaveData();

	void	DrawAreaSphere( CDC * pDC , FLOAT fX , FLOAT fY , FLOAT fZ , FLOAT fRadius , INT32 nType , CString *pStr );

	virtual BOOL OnLoadData		();
	virtual BOOL OnSaveData		();

public:
	//////////////////////////////////////////////////////////////////////////
	// Selection °è»ê¿ë..
	enum	Mode
	{
		MODE_NONE			,
		MODE_RANGESELECTED	,
		MODE_REGIONMOVE		,
		MODE_MAX
	};
	INT32		m_nRangeDragMode	;
	int			m_SelectedPosX1		;
	int			m_SelectedPosZ1		;
	int			m_SelectedPosX2		;
	int			m_SelectedPosZ2		;
	
	CPoint		m_pointLastPress;

	ApdObject	*	m_pcsSelectedObject		;

	int	GetObjectCountInBBox( AuBOX * pBox );
	
	BOOL	LockDivision	( UINT32 uDivisionIndex );
	BOOL	UnlockDivision	( UINT32 uDivisionIndex );
	BOOL	CheckLockDivision( UINT32 uDivisionIndex , CString * pMsg );

	// Generated message map functions
protected:
	//{{AFX_MSG(CRTMenuSky)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTMENUSKY_H__25E8B275_FA58_4F46_81B1_1C65EC008C9E__INCLUDED_)
