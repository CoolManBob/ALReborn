#if !defined(AFX_RTMENUSPAWN_H__208AA3B3_AD9F_4C0C_8AE6_684A1BB8CBD7__INCLUDED_)
#define AFX_RTMENUSPAWN_H__208AA3B3_AD9F_4C0C_8AE6_684A1BB8CBD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RTMenuSpawn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRTMenuSpawn window
class CRTMenuBase;
class ApdObject;

class CRTMenuSpawn : public CRTMenuBase
{
// Construction
public:
	CRTMenuSpawn();

// Attributes
public:
	BOOL	m_bFirstTimeNewObjectFileCreation;

// Operations
public:
	virtual BOOL	Create( CRect * pRect , CWnd * pParent );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTMenuSpawn)
	//}}AFX_VIRTUAL

	void	RefreshDivisionData();
// Implementation
public:
	virtual	char *	GetMenuName();
	virtual ~CRTMenuSpawn();

	virtual void OnPaintOnMap		( CDC * pDC );

	virtual BOOL OnLButtonDownMap	( CPoint &point , int x , int z );
	virtual BOOL OnLButtonUpMap		( CPoint &point , int x , int z );
	virtual BOOL OnMouseMoveMap		( CPoint &point , int x , int z );
	virtual BOOL OnLButtonDblDownMap( CPoint &point , int x , int z );

	virtual	BOOL OnDeleteKeyDown();
	virtual BOOL OnQuerySaveData();
protected:
	BOOL	m_bUnsavedData;

	void	OnPaintOnMap_Origianal( CDC * pDC );
	void	OnPaintOnMap_New( CDC * pDC );

public:
	int	GetObjectCountInBBox( AuBOX * pBox );


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
	INT32		m_nRangeDragMode;
	int			m_SelectedPosX1;
	int			m_SelectedPosZ1;
	int			m_SelectedPosX2;
	int			m_SelectedPosZ2;
	
	CPoint		m_pointLastPress;

	ApdObject	*	m_pcsSelectedObject;
	ApdObject	*	m_pcsOnMouseObject;

	ApdObject	*	GetSpawnObject( int x , int z );

	
	BOOL	LockDivision	( UINT32 uDivisionIndex );
	BOOL	UnlockDivision	( UINT32 uDivisionIndex );

	AuNode< ApmMap::RegionElement > *	GetRegion( INT32 nOffsetX , INT32 nOffsetZ );

	// Generated message map functions
protected:
	//{{AFX_MSG(CRTMenuSpawn)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTMENUSPAWN_H__208AA3B3_AD9F_4C0C_8AE6_684A1BB8CBD7__INCLUDED_)
