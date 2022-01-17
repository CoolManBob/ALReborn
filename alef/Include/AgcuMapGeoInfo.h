// AgcuMapGeoInfo.h
// -----------------------------------------------------------------------------
//                            __  __              _____            _____        __           _     
//     /\                    |  \/  |            / ____|          |_   _|      / _|         | |    
//    /  \    __ _  ___ _   _| \  / | __ _ _ __ | |  __  ___  ___   | |  _ __ | |_  ___     | |__  
//   / /\ \  / _` |/ __| | | | |\/| |/ _` | '_ \| | |_ |/ _ \/ _ \  | | | '_ \|  _|/ _ \    | '_ \ 
//  / ____ \| (_| | (__| |_| | |  | | (_| | |_) | |__| |  __/ (_) |_| |_| | | | | | (_) | _ | | | |
// /_/    \_\\__, |\___|\__,_|_|  |_|\__,_| .__/ \_____|\___|\___/|_____|_| |_|_|  \___/ (_)|_| |_|
//            __/ |                       | |                                                      
//           |___/                        |_|                                                      
//
// terrain's geometry
//
// -----------------------------------------------------------------------------
// Originally created on 07/20/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUMAPGEOINFO_20050720
#define _H_AGCUMAPGEOINFO_20050720

#include "ApmMap.h"
#include "AgcmMap.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include <rwcore.h>
#include <vector>

// -----------------------------------------------------------------------------

template<class T>
class CSecondArray
{
public:
	CSecondArray(const int row=0, const int column=0);

	void bSetOrder(int row, int column);
	T& operator [](int i);
	const T& operator [] (int i)const;

	// array[i][j] => array(i,j)
	T& operator ()(int i, int j);
	const T& operator ()(int i, int j) const;

private:
	typedef std::vector<T>	CONTAINER;
	int			row;
	int			column;
	CONTAINER	buff;
};
// -----------------------------------------------------------------------------
template <class T>
CSecondArray<T>::CSecondArray(const int row, const int column)
: row(row), column(column), buff(row*column)
{};

// -----------------------------------------------------------------------------
template <class T>
void CSecondArray<T>::bSetOrder(int row, int column)
{
	ASSERT( row > 0 && column > 0 );
	this->row = row;
	this->column = column;

	buff.clear();
	buff.resize(row*column);
}

// -----------------------------------------------------------------------------
template <class T>
T& CSecondArray<T>::operator [](int i)
{
	ASSERT( i < row*column );
	return buff[i];
}
// -----------------------------------------------------------------------------
template <class T>
const T& CSecondArray<T>:: operator [] (int i)const
{
	ASSERT( i < row*column );
	return buff[i];
}

// a[i][j] => a(i,j)
// -----------------------------------------------------------------------------
template <class T>
T& CSecondArray<T>::operator ()(int i, int j)
{
	ASSERT( i<row && j<column );
	ASSERT( buff.size() == (size_t)(row*column) );
	return buff[i*column + j];
}
// -----------------------------------------------------------------------------
template <class T>
const T& CSecondArray<T>::operator ()(int i, int j) const
{
	ASSERT( i<row && j<column );
	ASSERT( buff.size() == (size_t)(row*column) );
	return buff[i*column + j];
}


// -----------------------------------------------------------------------------
class AgcuMapGeoInfo
{
	enum eLineSlopeType
	{
		// +x축과 +z축을 1사분면으로 하는 데카르트 직교좌표계를 기준으로 함
		e_l2r		= 0,	// left to right	: +x
		e_r2l		,		// right to left	: -x
		e_t2b		,		// top to bottom	: -z
		e_b2t		,		// bottom to top	: +z
		e_lt2rb		,		// left_top to right_bottom
		e_rb2lt		,		// right_bottom to left_top
		e_rt2lb		,		// right_top to left_bottom
		e_lb2rt		,		// left_bottom to right_top

		e_justpoint	,		// not line but point

		e_slopeNum	,
	};

	typedef std::vector<POINT>					CONTAINER_LINETILE;
	typedef CONTAINER_LINETILE::const_iterator	LINETILE_CITR;

	typedef std::vector<RwV3d>					CONTAINER_JOINT;
public:
	// Construction/Destruction
	explicit AgcuMapGeoInfo(ApmMap* pApmMap=NULL, AgcmMap* pAgcmMap=NULL, ApmObject* pApmObject=NULL, const RwReal tilesize=400.f);
	virtual	~AgcuMapGeoInfo();

	// Operators

	// Accessors
	const POINT&	bGetPtStart()const { return m_ptStart; };
	const POINT&	bGetPtDest()const { return m_ptDest; };
	const RwV3d&	bGetStart()const { return m_vStart; };
	const RwV3d&	bGetDest()const { return m_vDest; };

	const RwV3d&	bGetLeftBottom()const { return m_vLeftBottom; };
	const RwReal	bGetTileSize()const { return m_tilesize; };

	const RwReal	bGetHeight(RwInt32 x, RwInt32 z)const { return m_heightmap(z,x); };

	VOID			bSetModule( ApmMap* pApmMap, AgcmMap* pAgcmMap
							  , ApmObject* pApmObject, AgcmObject* pAgcmObject);

	// Interface methods
	RwInt32		bSetRect( const RwV3d& pt1, const RwV3d& pt2
						, const RwInt32 TILELIMIT, const RwInt32 TILEOFFSET);
	BOOL		bLineBlock_Tile(void);
	BOOL		bLineBlock_Obj(void);

	void		bGetTileCenter(RwV3d& center, RwInt32 x, RwInt32 z, bool bHeight=false, RwReal beforeHeight=0.f)const;
	BOOL		bTileChk(const POINT& pt, const RwV3d* pV3dFrom=NULL, RwReal* tileCenterHeight=NULL)const;
	BOOL		bTileChk(RwInt32 x, RwInt32 z, const RwV3d* pV3dFrom=NULL, RwReal* tileCenterHeight=NULL)const;

#ifdef _DEBUG
	VOID		bDbg_RenderHeightMap(RwUInt32 colr);
	VOID		bDbg_RenderLineTile(RwUInt32 colr);
	VOID		bDbg_RenderColliObj(void);
	VOID		bDbg_RenderLineTileIntsct(void);
#endif //_DEBUG

private:
	// Data members
	ApmMap*				m_pApmMap;
	AgcmMap*			m_pAgcmMap;
	ApmObject*			m_pApmObject;
	AgcmObject*			m_pAgcmObject;

	const RwReal		m_tilesize;
	const RwReal		m_ridableheight;

	RwInt32				m_width;
	RwInt32				m_height;
	POINT				m_ptStart;
	POINT				m_ptDest;
	POINT				m_ptLeftBottom;

	RwV3d				m_vLeftBottom;
	RwV3d				m_vStart;
	RwV3d				m_vDest;
	RwV2d				m_vDir_S2D_ProjToXZ;	//project dirction vector(m_vDest - m_vStart) to XZ_plane

	CSecondArray<RwReal>	m_heightmap;
	
	CONTAINER_LINETILE		m_lineTile;
	CONTAINER_JOINT			m_joints;

	// Implementation methods
	void	vClampDest( RwInt32 dx, RwInt32 dz, const RwInt32 TILELIMIT
					  , const RwInt32 TILEOFFSET);
	void	vBuildHeightMap(void);
	RwInt32	vBuildLineTile(void);
	bool	vSphereCheck(const RwV3d& tileCenter);

	eLineSlopeType vSlopeType(const POINT& ptStart, const POINT& ptEnd)const;
	eLineSlopeType vSlopeType(const RwV3d& ptStart, const RwV3d& ptEnd)const;

	typedef RwInt32 (AgcuMapGeoInfo::*mfptrGetLinesForChk)(RwV3d arrPos[3], const POINT& ptTile)const;
	mfptrGetLinesForChk
			m_fptrGetLinesForChk[e_slopeNum];
	RwInt32	vGetLinesForChk_l2r		(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_r2l		(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_t2b		(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_b2t		(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_lt2rb	(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_rb2lt	(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_rt2lb	(RwV3d arrPos[3], const POINT& ptTile)const;
	RwInt32	vGetLinesForChk_lb2rt	(RwV3d arrPos[3], const POINT& ptTile)const;

	BOOL	vIsAvailableTileIndex(const POINT& ptTileIndex)const;
	BOOL	vIsAvailableTileIndex(RwInt32 x, RwInt32 z)const;

	RwInt32 vGetIntsctBetweenProjRayAndTile(RwV3d& intsct, const POINT& ptTile, RwReal lastHeight, BOOL bOnlyXZAvaiable=TRUE);
	RwInt32	vGetColliObjIDListInTile(const POINT& pt, std::list<RwInt32>& idList)const;
	RwInt32 vGetColliObjIDListInTile(RwReal xInWorld, RwReal zInWorld, std::list<RwInt32>& idList)const;
	
	RwInt32	vGetRidableObjIDListInTile(const POINT& pt, std::list<RwInt32>* idList=NULL)const;
	RwInt32 vGetRidableObjIDListInTile(RwReal xInWorld, RwReal zInWorld, std::list<RwInt32>* idList=NULL)const;

	BOOL	vChckColliObj(RpIntersection& rpIntsct, const std::list<RwInt32>& idList, RwReal* dist=NULL)const;
	BOOL	vChckColliObj(RpIntersection& rpIntsctProjected, const std::list<RwInt32>& idList, RwInt32 loop)const;

	BOOL	vChckColliObj_Modified(RpIntersection& intsct, const std::list<RwInt32>& idList)const;

//	BOOL	vChckColliObj_onField(
//	BOOL	vChckColliObj_onRidableObj(
};

#endif // _H_AGCUMAPGEOINFO_20050720
// -----------------------------------------------------------------------------
// AgcuMapGeoInfo.h - End of file
// -----------------------------------------------------------------------------

