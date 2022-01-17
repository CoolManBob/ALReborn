#pragma once

#include "AgcModule.h"
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

class AFX_EXT_CLASS AgcmSetFaceDlg : public AgcModule
{
public:
	static AgcmSetFaceDlg *	GetInstance();

public:
	AgcmSetFaceDlg(void);
	~AgcmSetFaceDlg(void);

	BOOL	OpenMainFaceDlg();
	VOID	CloseMainFaceDlg();
	BOOL	IsOpenMainFaceDlg();

	BOOL	SetFaceAtomic(RpAtomic *pstAtomic);
	VOID	SetCallbacks(PVOID pvClass, ApModuleDefaultCallBack fnCBChangeAtomic, ApModuleDefaultCallBack fnCBApply)
	{
		m_pvClass			= pvClass;
		m_fnCBChangeAtomic	= fnCBChangeAtomic;
		m_fnCBApply			= fnCBApply;
	}

public:
	ApModuleDefaultCallBack		m_fnCBChangeAtomic;
	ApModuleDefaultCallBack		m_fnCBApply;
	PVOID						m_pvClass;
};
