#ifndef __AGCM_UDA_DLG_H__
#define __AGCM_UDA_DLG_H__

#include "ApBase.h"

#include <rwcore.h>
#include <rpworld.h>
#include <rplodatm.h>
#include <rtcharse.h>
#include <rttiff.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rphanim.h>
#include <rtanim.h>
#include <rpskin.h>
#include <rpusrdat.h>

#include "Apmodule.h"
#include "AgcmRender.h"
/*
#ifndef	_WINDLL
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUDADlgD" )
#else
#pragma comment ( lib , "AgcmUDADlg" )
#endif // 	_DEBUG
#endif
#endif //  _WINDLL*/

class AFX_EXT_CLASS AgcmUDADlg : public ApModule
{
protected:
	AgcmRender	*m_pcsAgcmRender;

public:
	ApModuleDefaultCallBack		m_fnCBChangeAtomic;
	ApModuleDefaultCallBack		m_fnCBApply;
	PVOID						m_pvClass;

	AgcmUDADlg();
	virtual ~AgcmUDADlg();

	static AgcmUDADlg	*GetInstance();

	AgcmRender			*GetAgcmRenderModule()	{return m_pcsAgcmRender;}

protected:
	BOOL	OnAddModule();

	// function
public:
	BOOL	OpenMainUDADlg(BOOL bEnable = TRUE);
	VOID	CloseMainUDADlg();
	BOOL	IsOpenMainUDADlg();

	// RenderType을 설장할 변수의 주소를 넘겨준다.
	VOID	SetRenderType(INT32	*plRenderType, RpAtomic *pstAtomic = NULL);
	VOID	EnableMainUDADlgControl(BOOL bEnable = TRUE);
	VOID	SetCallbacks(PVOID pvClass, ApModuleDefaultCallBack fnCBChangeAtomic, ApModuleDefaultCallBack fnCBApply)
	{
		m_pvClass			= pvClass;
		m_fnCBChangeAtomic	= fnCBChangeAtomic;
		m_fnCBApply			= fnCBApply;
	}
};

#endif