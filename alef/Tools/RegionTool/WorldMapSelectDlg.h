#pragma once
#include "afxwin.h"
#include <vector>

// CWorldMapDisplayStatic

class CWorldMapDisplayStatic : public CStatic
{
	DECLARE_DYNAMIC(CWorldMapDisplayStatic)

public:
	CWorldMapDisplayStatic();
	virtual ~CWorldMapDisplayStatic();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

public:
	inline BOOL	SetWorldMap( int nWorldMapIndex , FLOAT fStartX , FLOAT fStartZ , FLOAT fEndX , FLOAT fEndZ );

protected:
	inline int	PosToScreenX( FLOAT fX );
	inline int	PosToScreenY( FLOAT fZ );

	void	ReleaseImage()		{	if( m_pImage ) delete m_pImage ; m_pImage = NULL;	}
	void	AllocImage()		{	ReleaseImage(); m_pImage = new CxImage ;			}

protected:
	int			m_nWorldMap;
	CxImage*	m_pImage;
	CBmp		m_bmpMap;

	FLOAT		m_fStartX;
	FLOAT		m_fStartZ;
	FLOAT		m_fEndX;
	FLOAT		m_fEndZ;

	FLOAT		m_fStepSize;

	CPoint		m_pointOffset;
};

// CWorldMapSelectDlg 대화 상자입니다.

class CWorldMapSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldMapSelectDlg)

public:
	CWorldMapSelectDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorldMapSelectDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORLDMAP_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedSaveMapList();
	afx_msg void OnLbnSelchangeList();

public:
	ApmMap::WorldMap * GetMap( int nMap );

	void	SetListChange()		{ m_bListGetChanged = true; }
	bool	IsListChanged()		{ return m_bListGetChanged; };

	void	UpdateList();

public:
	std::vector< ApmMap::WorldMap >	m_arrayWorldMap;
	
	INT32		m_nMapIndex;
	bool		m_bListGetChanged;
	CListBox	m_ctlList;
	CWorldMapDisplayStatic	m_ctlDisplay;
};
