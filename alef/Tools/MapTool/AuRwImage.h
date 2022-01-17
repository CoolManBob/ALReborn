// AuRwImage.h: interface for the AuRwImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AURWIMAGE_H__0395952C_6D9E_49B0_ACF4_88E96A4C12FF__INCLUDED_)
#define AFX_AURWIMAGE_H__0395952C_6D9E_49B0_ACF4_88E96A4C12FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"
#include "rpworld.h"


class AuRwImage  
{
protected:
	RwImage		*	m_pImage	;
	RwRaster	*	m_pRaster	;
	INT32			m_nCount	;	// CheckOut Count
public:
	AuRwImage():m_pImage( NULL ),m_pRaster( NULL ),m_nCount ( 0 ) {}
	virtual ~AuRwImage()
	{
		Destroy();
	}

	inline	RwImage *	GetImagePointer	() { return m_pImage; }
	inline	INT32		GetCheckOutCount() { return m_nCount; }

	// 마고자가 개량한 펑션들..
	BOOL	Set				( RwImage * pImage )
	{
		Destroy();
		m_pImage	= pImage	;
		return TRUE;
	}
	BOOL	Create			( INT32 nWidth , INT32 nHeight , INT32 nDepth , BOOL bAllocPixel = TRUE );
	BOOL	Load			( char * pFileName );

	BOOL	CreateSubImage	( AuRwImage * pDestImage , int x , int y , int cx , int cy );
	RwRaster *	GetSubRaster( int x , int y , int cx , int cy , INT32 nFlag = rwRASTERTYPECAMERATEXTURE  | rwRASTERFORMAT8888 );

	// Get , Release Raster
	// GetDC , ReleaseDC처럼 .. 사용한다.
	// SubRaster를 얻기 위해서 써야함.
	RwRaster *	GetRaster		() { return m_pRaster; }
	RwRaster *	CheckOutRaster	( INT32 nFlag = rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888  );
		// 레스터를 생성하여 카피하여 리턴.. 받은포인터는 제거해주어야함.
	void		CheckInRaster	();

	
	// 기존 펑션들.. 인크립트.

	inline	BOOL	Destroy()
	{
		if( m_pImage )
		{
			BOOL bRet = RwImageDestroy( m_pImage ) ? TRUE : FALSE;
			m_pImage	= NULL	;
			m_nCount	= 0		;
			return bRet;
		}
		else
			return TRUE;
	}

	inline	BOOL	Copy( RwImage * pImage )
	{
		return RwImageCopy( GetImagePointer() , pImage ) ? TRUE : FALSE;
	}

	inline	int		GetDepth()
	{
		if( GetImagePointer() )
			return RwImageGetDepth( GetImagePointer() );
		else
			return 0;	// ERROR!!
	}

	inline	int		GetHeight()
	{
		if( GetImagePointer() )
			return RwImageGetHeight( GetImagePointer() );
		else
			return 0;	// ERROR!!
	}

	inline	int		GetWidth()
	{
		if( GetImagePointer() )
			return RwImageGetWidth( GetImagePointer() );
		else
			return 0;	// ERROR!!
	}

	inline	RwImage * MakeMask		(						) { return RwImageMakeMask		( GetImagePointer() );			}
	inline	RwImage * ApplyMask		( RwImage * pImage		) { return RwImageApplyMask		( GetImagePointer() , pImage );	}
	inline	RwImage * SetFromRaster	( RwRaster * pRaster	) { return RwImageSetFromRaster	( GetImagePointer() , pRaster);	}
};

#endif // !defined(AFX_AURWIMAGE_H__0395952C_6D9E_49B0_ACF4_88E96A4C12FF__INCLUDED_)
