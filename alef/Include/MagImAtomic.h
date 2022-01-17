// MagImAtomic.h: interface for the MagImAtomic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MagImAtomic_H__CE2E8A31_FA32_4005_A660_A011F4F4A0EA__INCLUDED_)
#define AFX_MagImAtomic_H__CE2E8A31_FA32_4005_A660_A011F4F4A0EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"

// Im3D를 위한 아토믹..

class MagImAtomic  
{
protected:
	RwIm3DVertex	*	m_pVertex	;
	INT32				m_nVertex	;	// 갯수..
	RwImVertexIndex	*	m_pIndex	;
	INT32				m_nIndex	;	// 갯수..

	UINT32				m_nTransformFlag;	// 트랜스폼 플래그..
	RwPrimitiveType		m_nPrimitiveType;	// 렌더 플래그..

	RwTexture		*	m_pTexture	;
	INT32				m_nTexture	;	// 설정한 텍스쳐 인덱스를 저장해둠.
	RwFrame			*	m_pFrame	;
	
	BOOL				m_bVisible	;
public:
	MagImAtomic();
	virtual ~MagImAtomic();

	// Creation...
	BOOL	CreateAtomic(	INT32			nVertex	,
							INT32			nIndex	,
							UINT32			nTrans	= rwIM3D_VERTEXRGBA | rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ	,
							RwPrimitiveType	nType	= rwPRIMTYPETRILIST	);

	BOOL	SetTexture	(	RwTexture	*	pTexture	, INT32 nTexture = -1 );	// Ref Count를 증가시킨다.
	BOOL	IsAvailable	() { if( m_pVertex ) return TRUE ; else return FALSE; }

	BOOL	DestroyAtomic();

	// Get/Set inline Functions..
	RwIm3DVertex	*	GetVertexBuffer	()	{ return m_pVertex;	}
	INT32				GetVertexCount	()	{ return m_nVertex;	}
	RwImVertexIndex	*	GetIndexBuffer	()	{ return m_pIndex;	}
	INT32				GetIndexCount	()	{ return m_nIndex;	}

	UINT32				GetTransFormFlag()	{ return m_nTransformFlag;	}
	RwPrimitiveType		GetPrimitiveType()	{ return m_nPrimitiveType;	}
	UINT32				SetTransFormFlag( UINT32			nFlag )	{ return m_nTransformFlag =	nFlag; }
	RwPrimitiveType		SetPrimitiveType( RwPrimitiveType	nType )	{ return m_nPrimitiveType =	nType; }
	
	RwFrame			*	GetFrame		()	{ return m_pFrame	;	}
	RwTexture		*	GetTexture		()	{ return m_pTexture	;	}
	INT32				GetTextureIndex	()	{ return m_nTexture	;	}

	BOOL				GetVisible		()	{ return m_bVisible;	}
	BOOL				SetVisible		( BOOL bVisible ) { return m_bVisible = bVisible; }

	// Render Functions..
	BOOL	Render();
};

#endif // !defined(AFX_MagImAtomic_H__CE2E8A31_FA32_4005_A660_A011F4F4A0EA__INCLUDED_)
