#if !defined(AFX_RTMENUREGION_H__5381F28E_DA8E_454E_94BE_42500E8F945B__INCLUDED_)
#define AFX_RTMENUREGION_H__5381F28E_DA8E_454E_94BE_42500E8F945B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RTMenuRegion.h : header file
//

#include "RegionMenuDlg.h"

#define TEMPLATE_INDEX_MAX	256
/////////////////////////////////////////////////////////////////////////////
// CRTMenuRegion window
class CRTMenuBase;

class CRTMenuRegion : public CRTMenuBase
{
protected:
	BOOL	m_bChangedRegion	;
	BOOL	m_bChangedTemplate	;
public:
	BOOL	IsChangedRegion		() { return m_bChangedRegion	; }
	BOOL	IsChangedTemplate	() { return m_bChangedTemplate	; }

// Construction
public:
	CRTMenuRegion();

// Attributes
public:
	enum CONTROL_ID
	{
		IDC_MENUREGION_LIST	= 1001	,
		IDC_MENUREGION_BUTTON_EDIT	,
		IDC_MENUREGION_BUTTON_ADD	,
		IDC_MENUREGION_BUTTON_REMOVE,
		IDC_MENUREGION_BUTTON_DELETE
	};
	CListBox	m_ctlTemplate;
	
	CButton		m_wndButtonEdit		;
	CButton		m_wndButtonAdd		;
	CButton		m_wndButtonRemove	;
	CButton		m_wndButtonDelete	;

	void	SetWindowControlPosition();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTMenuRegion)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual BOOL	Create( CRect * pRect , CWnd * pParent );

	void	UpdateList();

	BOOL	LockRegion();
	BOOL	LockTemplate();

// Implementation
public:
	virtual ~CRTMenuRegion();

	virtual	char *	GetMenuName();
	virtual	BOOL OnDeleteKeyDown();
	virtual void OnPaintOnMap		( CDC * pDC );
	virtual BOOL OnLButtonDownMap	( CPoint &point , int x , int z );
	virtual BOOL OnLButtonUpMap		( CPoint &point , int x , int z );
	virtual BOOL OnMouseMoveMap		( CPoint &point , int x , int z );

	virtual BOOL OnLoadData		();
	virtual BOOL OnSaveData		();

	void OnSelchangeTemplate();

	////////////////////////////////
	// 템플릿 관련..

	/*
	AuList< ApmMap::RegionTemplate > m_listTemplate;

	BOOL						AddTemplate( ApmMap::RegionTemplate * pTemplate );
	BOOL						RemoveTempate( int nIndex );
	ApmMap::RegionTemplate *	GetTemplate( int nIndex );
	*/

	int	m_nCurrentTemplate;
	BOOL	SetCurrentTemplate( int nIndex );
	int		GetCurrentTemplate()		{	return m_nCurrentTemplate;	}

	AuList< ApmMap::RegionElement > m_listElement;
	AuNode< ApmMap::RegionElement > * m_pSelectedElement;
	
	BOOL						AddRegion( ApmMap::RegionElement * pElement )
	{
		return AddRegion( pElement->nIndex , pElement->nStartX , pElement->nStartZ , pElement->nEndX , pElement->nEndZ , pElement->nKind );
	}
	BOOL						AddRegion( int nIndex , INT32 nStartX , INT32 nStartZ , INT32 nEndX , INT32 nEndZ , INT32 nKind = ApmMap::RK_BOX );
	AuNode< ApmMap::RegionElement > *	GetRegion( INT32 nOffsetX , INT32 nOffsetZ , INT32 nSelectedTemplate = -1 );
	BOOL						RemoveRegion( AuNode< ApmMap::RegionElement > * pNode );
	void						RemoveSelected();

	BOOL	Save( char * pFileName , BOOL bEncryption = FALSE);
	BOOL	Load( char * pFileName , BOOL bDecryption = FALSE);

	//////////////////////////////////////////////////////////////////////////
	// Selection 계산용..
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


	static BOOL CBProgress( char * pStr , int nPos , int nMax , void * pData );

	// Generated message map functions
protected:
	//{{AFX_MSG(CRTMenuRegion)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTMENUREGION_H__5381F28E_DA8E_454E_94BE_42500E8F945B__INCLUDED_)
