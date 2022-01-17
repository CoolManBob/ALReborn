#if !defined(AFX_PLUGIN_BITMAPEXPORT_H__2A868B88_0BA5_4833_9506_211D0FB1F72D__INCLUDED_)
#define AFX_PLUGIN_BITMAPEXPORT_H__2A868B88_0BA5_4833_9506_211D0FB1F72D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Plugin_BitmapExport.h : header file
//
#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include "HeightMap.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugin_BitmapExport window

#define BITMAP_DETAIL	256
#define BITMAP_BUTTONHEIGHT	20
#define BITMAP_BUTTONWIDTH	70
#define BITMAP_UI_MARGIN	5

class CPlugin_BitmapExport : public CUITileList_PluginBase
{
// Construction
public:
	CPlugin_BitmapExport();

// Attributes
public:
	enum
	{
		IDC_BUTTON_PASTEIN	= 1001,
		IDC_BUTTON_COPY		= 1002,
		IDC_BUTTON_PROPERTY	= 1003,
		IDC_BUTTON_GENERATE	= 1004,

		IDC_BUTTON_VERTEX_COLOR_GET	= 1005,
		IDC_BUTTON_VERTEX_COLOR_SET = 1006
	};

	CButton	m_buttonPasteIn	;
	CButton	m_buttonCopy	;

	CButton	m_buttonSetting	;
	CButton	m_buttonGenerate;

	CButton m_buttonVertexColorSet;
	CButton m_buttonVertexColorGet;

public:
	// 이미지 데이타.
	CDC		m_memDC		;
	CBitmap	m_memBitmap	;

	FLOAT	m_fHeightMin;
	FLOAT	m_fHeightMax;
	FLOAT	m_fFilter	;

	// Generate Info
	INT32	m_nGenerateIteration;
	FLOAT	m_fGenerateFilter	;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_BitmapExport)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	BOOL SetClipBoard();
	BOOL GetClipBoard();

	void GetHeightInfo();
	void SetHeightInfo();

	static BOOL	__DivisionSetVertexColorCallback ( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	__DivisionGetVertexColorCallback ( DivisionInfo * pDivisionInfo , PVOID pData );

	void GetVertexColorMap();
	void SetVertexColorMap();
	
	virtual ~CPlugin_BitmapExport();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_BitmapExport)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHeightmapPropteryDlg dialog

class CHeightmapPropteryDlg : public CDialog
{
// Construction
public:
	CHeightmapPropteryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHeightmapPropteryDlg)
	enum { IDD = IDD_HEIGHTMAP_PROPERTY };
	float	m_fGeometryFilter;
	int		m_nMaxHeight;
	int		m_nMinHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeightmapPropteryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHeightmapPropteryDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CGenerateGeometryDlg dialog

class CGenerateGeometryDlg : public CDialog
{
// Construction
public:
	CGenerateGeometryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenerateGeometryDlg)
	enum { IDD = IDD_GENERATE_GEOMETRY };
	float	m_fFilter;
	int		m_nIteration;
	//}}AFX_DATA

	CHeightMap	m_stHeightMap;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenerateGeometryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenerateGeometryDlg)
	afx_msg void OnGenerate();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGIN_BITMAPEXPORT_H__2A868B88_0BA5_4833_9506_211D0FB1F72D__INCLUDED_)
