#if !defined(AFX_TEXTURELISTSTATIC_H__D37F9C77_2A6E_4E16_BD73_ED42691ECAEF__INCLUDED_)
#define AFX_TEXTURELISTSTATIC_H__D37F9C77_2A6E_4E16_BD73_ED42691ECAEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextureListStatic.h : header file
//
#include "bmp.h"

#define	TLS_IMAGE_SIZE			50
#define	TLS_BACKGROUNDCOLOR		RGB( 0 , 0 , 0 )
#define	TLS_SELECT_COLOR		RGB( 210 , 185 , 230 )

#define	WM_TEXTURE_DROP_EVENT	( WM_USER + 8362 )

#include "AuList.h"
/////////////////////////////////////////////////////////////////////////////
// CTextureListStatic window

class CTextureListStatic : public CStatic
{
// Construction
public:
	CTextureListStatic();

	class	TextureElement
	{
	public:
		INT32	nIndex				;
		char	strFilename	[ 256 ]	;
		char	strComment	[ 256 ]	;

		INT32	x;
		INT32	y;

		CBmp	bmp;

		TextureElement() : nIndex( -1 )
		{
			strcpy( strFilename	, "" );
			strcpy( strComment	, "" );
		}
	};

	AuList< TextureElement * >	m_listTextureDraw	;
	INT32						m_nIndexSelected	;

// Attributes
public:
	// µå·¡±ë ÀÎÆ÷..
	BOOL				m_bDragging			;
	POINT				m_pointPrev			;
	POINT				m_pointFirst		;
	TextureElement *	m_pSelectedElement	;


	BOOL	AddTexture		( INT32 nIndex , char * pFilename , char * pComment = NULL , INT32 x = 0 , INT32 y = 0 );
	BOOL	RemoveTexture	( INT32	nIndex	);
	void	RemoveAll		();

	INT32	GetTextureIndex	() { return m_nIndexSelected; }

	BOOL	LoadScript		( char *pFilename );
	BOOL	SaveScript		( char *pFilename );

protected:
	INT32	GetEmptyIndex	();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureListStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextureListStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextureListStatic)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTURELISTSTATIC_H__D37F9C77_2A6E_4E16_BD73_ED42691ECAEF__INCLUDED_)
