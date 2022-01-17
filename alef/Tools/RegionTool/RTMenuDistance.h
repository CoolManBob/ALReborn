#pragma once

#include <vector>

class CRTMenuBase;
// RTMenuDistance

class CRTMenuDistance : public CRTMenuBase
{
	DECLARE_DYNAMIC(CRTMenuDistance)

public:
	CRTMenuDistance();
	virtual ~CRTMenuDistance();
	virtual	char *	GetMenuName();
	virtual BOOL	Create( CRect * pRect , CWnd * pParent );

	vector< AuPOS >	m_vectorPoint;

	float		m_fTime;
	float		m_fDistance;

public:
	virtual BOOL OnLButtonDownMap	( CPoint &point , int x , int z );
	virtual void OnPaintOnMap		( CDC * pDC );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


