// AgcuCamPathWork.h
// -----------------------------------------------------------------------------
//                             _____                 _____        _   _     _   _   _            _        _     
//     /\                     / ____|               |  __ \      | | | |   | | | | | |          | |      | |    
//    /  \    __ _  ___ _   _| |      __ _ _ __ ___ | |__) | __ _| |_| |__ | | | | | | ___  _ __| | __   | |__  
//   / /\ \  / _` |/ __| | | | |     / _` | '_ ` _ \|  ___/ / _` | __| '_ \| | | | | |/ _ \| '__| |/ /   | '_ \ 
//  / ____ \| (_| | (__| |_| | |____| (_| | | | | | | |    | (_| | |_| | | |  V _ V  | (_) | |  |   <  _ | | | |
// /_/    \_\\__, |\___|\__,_|\_____|\__,_|_| |_| |_|_|     \__,_|\__|_| |_|\__/ \__/ \___/|_|  |_|\_\(_)|_| |_|
//            __/ |                                                                                             
//           |___/                                                                                              
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 01/19/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUCAMPATHWORK_20050119
#define _H_AGCUCAMPATHWORK_20050119

#define CW_MAX_SPLINE_NODE	100

#include "AgcmCamera2.h"
// =============================================================================
// AgcuPathWork
// =============================================================================
class AgcuPathWork
{
protected:
	enum	eWorkType
	{
		eWT_SingleSpline	 = 0,
		eWT_DoubleSpline	,
	};
public:
	// Construction/Destruction
	AgcuPathWork(eWorkType eType, RwUInt32 ulLife);

	//@{ 2006/06/23 burumal
	// 순수가상함수 오류수정
	/*
	virtual	~AgcuPathWork() = 0;
	*/
	virtual	~AgcuPathWork();
	//@}

	// Operators

	// Accessors

	// Interface methods
	virtual RwInt32		bOnIdle(RwMatrix& matOut, RwUInt32 ulDifTick) { return 0; };
	virtual RwInt32		bOnIdleOffset(RwMatrix& matOut, RwUInt32 ulOffset) { return 0; };
	virtual	void		bReset(void);
	virtual	void		bForward(void);
	virtual	void		bBackward(void);
	virtual	void		bLoop(void);
	virtual	void		bNoLoop(void);

	RwInt32				bOnIdleSetKeyTime(RwMatrix& matOut, RwUInt32 ulSetTick/*ulSetTick > 0 or ulSetTick < life*/);

protected:
	// Data members
	const eWorkType	m_eType;

	RwUInt32	m_ulLife;
	RwUInt32	m_ulAccum;			//accumulated time

	union
	{
		RwUInt32	m_ulFlags;
		struct{
			RwUInt32	m_1ulState		: 1;	//0 : begin, 1 : end
			RwUInt32	m_1ulForward	: 1;	//0 : backward, 1 : forward
			RwUInt32	m_1ulLoop		: 1;	//0 : no loop, 1 : loop
		}st;
	}m_unionBaseFlag;

	// Implementation methods
	virtual	void		tTimeUpdate(RwUInt32 ulDifTick);
	virtual	RwUInt32	tKeyTime(void);

public:
	static AgcuPathWork * Load( BYTE * pByte );
	static AgcuPathWork * CreateSplinePath(LPCSTR fname/*.sab*/);
	// this allocate path work structure internally
	// you should destroy manually the returned pointer
	// before call this function , read the .SAB file into binary stream,
	// and call with that stream pointer.

	// ex) if you want read *.sab file directly..
	// 
	// FILE * pFile = fopen( "test.sab" , "rb" );
	// INT32 nFileSize;
	// if( 0 == fseek( pFile , 0 , SEEK_END ) )
	// {
	//		nFileSize = ftell( pFile );
	// }
	// BYTE * pBinaryStream  = new BYTE( nFileSize );
	// AgcuPathWork * pSpline = AgcuPathWork::Load( pBinaryStream );
	// delete [] pBinaryStream;
	

	RwUInt32	GetLife		(){ return m_ulLife	; }
	RwUInt32	GetAccumTime(){ return m_ulAccum; }
	VOID		SetPos( RwReal pos/* 0.f ~ 1.f */ )	{ m_ulAccum += (RwInt32)((RwReal)(m_ulLife) * pos); }
};

// =============================================================================
// AgcuPathSingleSpline
// =============================================================================
typedef struct	stSetSpline
{
	RwInt32		numCtrl;
	RwMatrix*	ctrlMatrices;
	bool		closed;
}SETSPLINE, *PSETSPLINE, *LPSETSPLINE;
class AgcuPathSingleSpline : public AgcuPathWork
{
public:
	// Construction/Destruction
	AgcuPathSingleSpline(RwUInt32 ulLife);
	~AgcuPathSingleSpline();

	// Operators

	// Accessors

	// Interface methods
	RwInt32		bOnIdle(RwMatrix& matOut, RwUInt32 ulDifTick);
	RwInt32		bOnIdleOffset(RwMatrix& matOut, RwUInt32 ulOffset);
	RwInt32		bInitSpline(const SETSPLINE& setSpline);
	RwInt32		bInitSpline(AgcmResourceLoader* pAgcmResourceLoader
		, LPSTR szSpline/*fullpath*/
		, const RwMatrix* pmatTrans = NULL);

	void		bSetFixedLookat(const RwV3d* lookat);
	void		bSetActType(RwUInt32 ulType);
	void		bSetAccel(RwUInt32 ulAccel);

private:
	// Data members
	RpSpline*		m_pSpline;
	RwReal			m_fInvsLife;
	const RwV3d*	m_pFixedLookat;

	union
	{
		RwUInt32	m_ulFlags;
		struct{
			RwUInt32	m_4ulType	: 4;
			// 0 : flight
			// 1 : side vector is parallel with xz_plane
			// 2 : up vector is world's up vector (0,1,0) - at and side vector are parallel with xz_plane
			// 3 : fixed lookat with side vector parallel to xz_plane
			// 4 ~ 15 : reserved
			RwUInt32	m_1ulAcel	: 1;
			// 0 : Constant velocity
			// 1 : "Acceleration" into path, "decelerate" out of path
		}st;
	}m_unionFlag;
    
	// Implementation methods
	RwInt32		vGetMatrix_flight(RwMatrix* pmat, RwReal where);
	RwInt32		vGetMatrix_sideParallelXzplane(RwMatrix* pmat, RwReal where);
	RwInt32		vGetMatrix_upWorldUp(RwMatrix* pmat, RwReal where);
	RwInt32		vGetMatrix_fixedLookat(RwMatrix* pmat, RwReal where);
};

// =============================================================================
// AgcuPathDoubleSpline
// =============================================================================
class AgcuPathDoubleSpline : public AgcuPathWork
{
public:
	// Construction/Destruction
	AgcuPathDoubleSpline(RwUInt32 ulLife);
	~AgcuPathDoubleSpline();

	// Operators

	// Accessors

	// Interface methods
	RwInt32		bOnIdle(RwMatrix& matOut, RwUInt32 ulDifTick);
	
	RwInt32		bInitSpline(const SETSPLINE& setSpline);
	RwInt32		bInitSpline(AgcmResourceLoader* pAgcmResourceLoader
						   , LPSTR			 szSplineEye/*fullpath*/
						   , LPSTR			 szSplineAt/*fullpath*/);

	void		bSetActType(RwUInt32 ulType);
	void		bSetAccel(RwUInt32 ulAccel);

private:
	// Data members
	RpSpline*	m_pSplineEye;
	RpSpline*	m_pSplineLookat;
	RwReal		m_fInvsLife;

	union
	{
		RwUInt32	m_ulFlags;
		struct{
			RwUInt32	m_4ulType	: 4;
			// 0 : side vector is parallel with xz_plane
			// 1 : getMatrix from eyeSpline and rotat to look at lookatSpline
			// 2 ~ 15 : reserved
			RwUInt32	m_1ulAcel	: 1;
			// 0 : Constant velocity
			// 1 : "Acceleration" into path, "decelerate" out of path
		}st;
	}m_unionFlag;

	// Implementation methods
	RwInt32		vGetMatrix(RwMatrix* pmat, RwReal where);
	RwInt32		vGetEye(RwV3d& vEye, RwReal where);
	RwInt32		vGetLookat(RwV3d& vLookat, RwReal where);
};

struct	CW_PointInfo
{
public:
	RwV3d		pos		;
	RwMatrix	matrix	;
};

enum SPLINE_TYPE
{
	SINGLESPLINE,
	DOUBLESPLINE
};
enum SINGLE_SPLINE_TYPE
{
	SST_NORMAL	,
	SST_STARE	,
	SST_PARALLEL,
	SST_FLIGHT
};

#endif // _H_AGCUCAMPATHWORK_20050119
// -----------------------------------------------------------------------------
// AgcuCamPathWork.h - End of file
// -----------------------------------------------------------------------------

