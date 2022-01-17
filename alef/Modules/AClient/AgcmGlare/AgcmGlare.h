// AgcmGlare.h: interface for the AgcmGlare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMGLARE_H__2BBCFC13_3B0E_4ABE_BA5F_DEC08A04DEBB__INCLUDED_)
#define AFX_AGCMGLARE_H__2BBCFC13_3B0E_4ABE_BA5F_DEC08A04DEBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

#include <d3d9.h>

#include "rwcore.h"
#include "rpworld.h"

#include "AgcEngine.h"
#include "AgcmRender.h"
#include "AcuBlur.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmGlareD" )
#else
#pragma comment ( lib , "AgcmGlare" )
#endif
#endif

class AgcmGlare : public AgcModule  
{
private:
	RwCamera *			m_pCamera;
	AgcmRender *		m_pcsAgcmRender;

	RwRaster *			m_pRaster;
	RwRaster *			m_pRaster2;

	AcuBlur				m_csBlur;

	BOOL				m_bEnableGlare;

public:
	AgcmGlare();
	virtual ~AgcmGlare();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	static BOOL	CBPostRender(PVOID pvData, PVOID pvClass, PVOID pvCustData);
};

#endif // !defined(AFX_AGCMGLARE_H__2BBCFC13_3B0E_4ABE_BA5F_DEC08A04DEBB__INCLUDED_)
