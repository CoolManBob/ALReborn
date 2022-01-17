// AgcuCamConstrain.h: interface for the AgcuCamConstrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCUCAMCONSTRAIN_H__54F67B85_BD1C_4CE5_B05D_10A939D76F28__INCLUDED_)
#define AFX_AGCUCAMCONSTRAIN_H__54F67B85_BD1C_4CE5_B05D_10A939D76F28__INCLUDED_

#include "AgcmCamera2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//-----------------------------------------------------------------------------
//
// AgcdCamConstrain_Terrain
//
//-----------------------------------------------------------------------------
class AgcdCamConstrain_Obj;
class AgcdCamConstrain_Terrain
{
	struct stTile{
		union
		{
			struct
			{
				RwV3d	m_ltop;
				RwV3d	m_rtop;
				RwV3d	m_lbtm;
				RwV3d	m_rbtm;
			};
			RwV3d		m_av[4];
		};

		bool	m_bTurnEdge;
		stTile();
	};
#ifdef _DEBUG
	enum { e_maxtile = 4000 };
#else
	enum { e_maxtile = 100 };
#endif //_DEBUG

	ApSafeArray<stTile, e_maxtile> m_tilelist;
	//stTile	m_tilelist[e_maxtile];
	
	int		m_nCnt;
public:
	void	bPreChkTerrain( AgcdCamConstrain_Obj* pAgcdComConstrain_Obj, AgcmMap* pAgcmMap, const RwV3d& eye, const RwV3d& at );
	RwBool	bChkTerrain( RwReal* pfDist, const RwV3d& eye, const RwV3d& at, const RwV3d& uv3Ray );

public:
	AgcdCamConstrain_Terrain();
	virtual ~AgcdCamConstrain_Terrain();
};
//-----------------------------------------------------------------------------
//
// AgcdCamConstrain_Obj
//
//-----------------------------------------------------------------------------
class AgcdCamConstrain_Obj
{
#ifdef _DEBUG
	enum { e_maxObj = 4000 };
#else
	enum { e_maxObj = 100 };
#endif //_DEBUG
	ApSafeArray<RwInt32, e_maxObj> m_objlist;
	int		m_nCnt;


	BOOL	m_bIntsct;
	RwReal	m_fDist;
public:
	AgcdCamConstrain_Obj();
	virtual ~AgcdCamConstrain_Obj();

	void	bInit(RwReal fDist);

	BOOL	bIntersect()const {return m_bIntsct; };
	RwReal	bDistance()const {return m_fDist; };

	//intersection : line.start = lookat, line.end = eye;
	//랜더웨어 함수를 쓰므로 라인을 거꾸로.
	static RpAtomic* CBIntersectObj(  RpIntersection* intersection	
									, RpWorldSector* sector
									, RpAtomic* atom
									, RwReal dist
									, VOID* pThis);
	static RpCollisionTriangle* CBCmpDist( RpIntersection* intersection
										, RpCollisionTriangle* collTri
										, RwReal dist
										, void* pThis);
	BOOL	bIntsct(ApmObject* pApmObject, AgcmObject* pAgcmObject, RpIntersection* intersection);
private:
	BOOL	vCheckedObj(RwInt32 nIndex, RwInt32 nObjID);
public:

	//for debugging
	void	bRenderObjCollWire();
	void	bRenderObjSphere();
	void	bAddObjSphere(const RwSphere* psphere, RwUInt32 colr=0xffffff00);

	friend class AgcdCamConstrain_Terrain;
};

//-----------------------------------------------------------------------------
//
// AgcuCamConstrain
//
//-----------------------------------------------------------------------------
class AgcuCamConstrain  
{
	AgcdCamConstrain_Terrain	m_agcdTerrain;
	AgcdCamConstrain_Obj		m_agcdObj;
public:
	void	bConstrainDistance(AgcmCamera2* pAgcmCamera2);
	void	bConstrainPitch(AgcmCamera2* pAgcmCamera2);

private:
	AgcuCamConstrain();
public:
	~AgcuCamConstrain();

	static AgcuCamConstrain& bGetInst();
};

#endif // !defined(AFX_AGCUCAMCONSTRAIN_H__54F67B85_BD1C_4CE5_B05D_10A939D76F28__INCLUDED_)
