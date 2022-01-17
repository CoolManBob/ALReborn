// Plugin_TextureInfo.h: interface for the CPlugin_TextureInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGIN_TEXTUREINFO_H__2E2012F3_A3F8_4953_8411_20031CBBB05C__INCLUDED_)
#define AFX_PLUGIN_TEXTUREINFO_H__2E2012F3_A3F8_4953_8411_20031CBBB05C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UITileList_PluginBase.h"
#include "myengine.h"

struct	IndexPattern
{
	INT32	nIndex[ TD_DEPTH ];
};


const	INT32	TEXTURE_UNIT_SIZE	= 64;

class CGeometryPatternTexture
{
public:
	enum
	{
		TEXTURE_NONE	=	0,
		TEXTURE_64		=	1,
		TEXTURE_128		=	2,
		TEXTURE_256		=	4,
		TEXTURE_512		=	8,
		TEXTURE_1024	=	16
	};

	INT32				m_nType			;
	RwImage			*	m_pImage		;
	IndexPattern	*	m_pArrayPattern	;
	INT32				m_nCount		;

	CGeometryPatternTexture():m_nType( TEXTURE_NONE ),m_pImage( NULL ), m_pArrayPattern( NULL ) , m_nCount( 0 ){}
	~CGeometryPatternTexture(){ Release(); }

	FLOAT				GetStartU		( INT32 nIndex );
	FLOAT				GetEndU			( INT32 nIndex );
	FLOAT				GetStartV		( INT32 nIndex );
	FLOAT				GetEndV			( INT32 nIndex );

	BOOL	Create( AgcmMap * pCmMap , ApWorldSector * pSector );
	void	Release();

protected:
	INT32	GetLeft	( INT32 nIndex );
	INT32	GetTop	( INT32 nIndex );
};

class CPlugin_TextureInfo : public CUITileList_PluginBase  
{
public:
	CPlugin_TextureInfo();
	virtual ~CPlugin_TextureInfo();

	void	RecalcTriangleInfo();

	ApWorldSector	*	m_pCurrentSector;
	INT32				m_nCount		;
	virtual BOOL	OnMouseMoveGeometry	( RwV3d * pPos );

	virtual BOOL	OnSelectedPlugin	();
	virtual BOOL	OnDeSelectedPlugin	();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugin_TextureInfo)
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CPlugin_TextureInfo)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PLUGIN_TEXTUREINFO_H__2E2012F3_A3F8_4953_8411_20031CBBB05C__INCLUDED_)
