#if !defined(AFX_AURWIMAGE_H__0395952C_6D9E_49B0_ACF4_88E96A4C12FF__INCLUDED_)
#define AFX_AURWIMAGE_H__0395952C_6D9E_49B0_ACF4_88E96A4C12FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"
#include "rpworld.h"

class AuRwImage  
{
public:
	AuRwImage() : m_pImage(NULL), m_pRaster(NULL), m_nCount(0)	{		}
	virtual ~AuRwImage()										{		}

	BOOL		Load( char * pFileName );
	BOOL		Create( INT32 nWidth , INT32 nHeight , INT32 nDepth , BOOL bAllocPixel = TRUE );
	void		Destroy();

	BOOL		CreateSubImage( AuRwImage * pDestImage , int x , int y , int cx , int cy );
	
	RwRaster*	CheckOutRaster( INT32 nFlag = rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888  );
	void		CheckInRaster();		// 레스터를 생성하여 카피하여 리턴.. 받은포인터는 제거해주어야함.

	BOOL		Set( RwImage* pImage )		{	return (m_pImage = pImage) ? TRUE : FALSE;	}
	BOOL		Copy( RwImage * pImage )	{	return RwImageCopy( m_pImage, pImage ) ? TRUE : FALSE;	}
	RwImage*	GetImagePointer()	{	return m_pImage;	}
	RwRaster*	GetRaster()			{	return m_pRaster;	}
	RwRaster*	GetSubRaster( int x, int y, int cx, int cy, INT32 nFlag = rwRASTERTYPECAMERATEXTURE  | rwRASTERFORMAT8888 );
	INT32		GetCheckOutCount()	{	return m_nCount;	}
	int			GetDepth()			{	return m_pImage ? RwImageGetDepth( m_pImage ) : 0;	}
	int			GetHeight()			{	return m_pImage ? RwImageGetHeight( m_pImage ) : 0;	}
	int			GetWidth()			{	return m_pImage ? RwImageGetWidth( m_pImage ) : 0;	}

	RwImage*	MakeMask()							{	return RwImageMakeMask( m_pImage );			}
	RwImage*	ApplyMask( RwImage * pImage )		{	return RwImageApplyMask( m_pImage, pImage );	}
	RwImage*	SetFromRaster( RwRaster* pRaster )	{	return RwImageSetFromRaster( m_pImage, pRaster );	}

protected:
	RwImage*	m_pImage;
	RwRaster*	m_pRaster;
	INT32		m_nCount;	// CheckOut Count
};

#endif // !defined(AFX_AURWIMAGE_H__0395952C_6D9E_49B0_ACF4_88E96A4C12FF__INCLUDED_)
