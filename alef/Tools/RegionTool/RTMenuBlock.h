#pragma once

#include <vector>

class CRTMenuBase;
// RTMenuDistance

class CRTMenuBlock : public CRTMenuBase
{
	DECLARE_DYNAMIC(CRTMenuBlock)

public:
	int		m_nBrushSize;
	BOOL	m_bUnsavedData;

public:
	CRTMenuBlock();
	virtual ~CRTMenuBlock();
	virtual	char *	GetMenuName();
	virtual BOOL	Create( CRect * pRect , CWnd * pParent );

public:
	BOOL	m_bLButtonDownMap;
	virtual BOOL OnLButtonDownMap	( CPoint &point , int x , int z );
	virtual BOOL OnLButtonUpMap		( CPoint &point , int x , int z );
	virtual BOOL OnMouseMoveMap		( CPoint &point , int x , int z );
	//virtual void OnPaintOnMap		( CDC * pDC );
	virtual BOOL OnSaveData		();
	virtual BOOL OnQuerySaveData();

	virtual void OnActiveRegionMenu		();
	virtual void OnDeActiveRegionMenu	();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


