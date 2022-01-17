// ShowFrm.h
// -----------------------------------------------------------------------------
//   _____ _                    ______                    _     
//  / ____| |                  |  ____|                  | |    
// | (___ | |__   ___ __      __ |__   _ __ _ __ ___     | |__  
//  \___ \| '_ \ / _ \\ \ /\ / /  __| | '__| '_ ` _ \    | '_ \ 
//  ____) | | | | (_) |\ V  V /| |    | |  | | | | | | _ | | | |
// |_____/|_| |_|\___/  \_/\_/ |_|    |_|  |_| |_| |_|(_)|_| |_|
//                                                              
//                                                              
//
// show RwFrame
//
// -----------------------------------------------------------------------------
// Originally created on 02/22/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_SHOWFRM_20050222
#define _H_SHOWFRM_20050222

#include "rwcore.h"
#include "PickUser.h"
#include "EffUt_Geo.h"
USING_EFFUT;

// -----------------------------------------------------------------------------
enum eFrmGeoType
{
	eShowFrm_box	= 0,
	eShowFrm_sphere	,

	eShowFrm_num	,
};
class ShowFrm
{
	static CEffUt_GeoSphere<VTX_PD>	GEO_SPHERE;
	static CEffUt_GeoBox<VTX_PD>	GEO_BOX;
	
private:
	// do not use
	ShowFrm(const ShowFrm& cpy){cpy;};
	ShowFrm& operator = (const ShowFrm& cpy){cpy; return *this;};
public:
	// Construction/Destruction
	explicit ShowFrm(const RwV3d& pos
		, eFrmGeoType eGeoType=eShowFrm_sphere
		, RwUInt32 ulColr=0xffffffff);
	virtual	~ShowFrm();

	// Accessors
	RwFrame*	bGetPtrFrm();
	RwMatrix*	bGetPtrLTM();
	RwV3d*		bGetPtrPos();
	RwUInt32	bGetColr() { return m_ulColr; }

	// Interface methods
	void		bMoveTo(const RwV3d& newpos);
	void		bSetDir(const RwV3d& dst);
	void		bSetScale(RwReal scale);
	void		bRot(float dx, float dy);
	void		bSetIdentity(void);

	void		bSetColr(RwUInt32 ulColr) { m_ulColr = ulColr; };
	void		bAddChildFrame(RwFrame* pFrm);
	
	int			bRender( RwUInt32* pUlColr = NULL);
	BOOL		bPick( CPickUser& cPick, RwReal* pLen=NULL );

private:
	// Data members
	eFrmGeoType	m_eGeoType;
	RwUInt32	m_ulColr;
	RwFrame*	m_pFrm;

	// Implementation methods

};
typedef ShowFrm SHOWFRM, *LPSHOWFRM;

typedef std::vector<LPSHOWFRM>		ShowFrmVec;
typedef ShowFrmVec::iterator		ShowFrmVecItr;
typedef ShowFrmVec::const_iterator	ShowFrmVecCItr;

#endif // _H_SHOWFRM_20050222
// -----------------------------------------------------------------------------
// ShowFrm.h - End of file
// -----------------------------------------------------------------------------

