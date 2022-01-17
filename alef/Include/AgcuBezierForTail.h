// AgcuBezierForTail.h: interface for the AgcuBezierForTail class.
//

// color : 0 ~ input alpha
// tutv  : 0.0f ~ 1.0f

//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUBEZIERFORTAIL_H__46821B8F_E508_4F7B_97C1_C1E40FB12535__INCLUDED_)
#define AFX_AGCUBEZIERFORTAIL_H__46821B8F_E508_4F7B_97C1_C1E40FB12535__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffD3dVtx.h"
#include "AgcdEffCtrl.h"
#include "AgcuEffUtil.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

//extern D3DVTX_PCT	g_testLine[300];

///////////////////////////////////////////////////////////////////////////////
// bezier curve
// http://astronomy.swin.edu.au/~pbourke/curves/bezier/
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class AgcuBezierForTail  
{
public:
	AgcuBezierForTail(){};
	~AgcuBezierForTail(){};

	RwInt32	bBezier( VTX_TYPE arryOut[]
				   , RwInt32 nNumOfSampling
				   , const AgcdEffCtrl_Tail::TAILPOINT&	lastTailPoint
				   , const AgcdEffCtrl_Tail::TailPointList& listTailPoint
				   , RwUInt32 ulColr = 0x80808080
				   )
	{
		const RwReal	FALPHA	= static_cast<RwReal>(DEF_GET_ALPHA(ulColr));
		const RwUInt32	NALPHA	= static_cast<RwUInt32>(DEF_GET_ALPHA(ulColr));
		if( !arryOut )
		{
			Eff2Ut_ERR( "arryOut == NULL @ AgcuBezierForTail::bBezier" );
			return -1;
		}

		switch( listTailPoint.size() )
		{
		case 0:
			//no polygon
			break;
		case 1:
			{//only one rect
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint = &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*listTailPoint.begin()));

				arryOut[0].m_dwColr = ulColr;
				DEF_SET_ALPHA(arryOut[0].m_dwColr,0);
				arryOut[0].m_v3Pos.x	= lpTailPoint->m_v3dP1.x,
				arryOut[0].m_v3Pos.y	= lpTailPoint->m_v3dP1.y,
				arryOut[0].m_v3Pos.z	= lpTailPoint->m_v3dP1.z;
				arryOut[0].m_v2Tutv.x	= 0.f,
				arryOut[0].m_v2Tutv.y	= 0.f;

				arryOut[1].m_dwColr = ulColr;
				DEF_SET_ALPHA(arryOut[1].m_dwColr,0);
				arryOut[1].m_v3Pos.x	= lpTailPoint->m_v3dP2.x,
				arryOut[1].m_v3Pos.y	= lpTailPoint->m_v3dP2.y,
				arryOut[1].m_v3Pos.z	= lpTailPoint->m_v3dP2.z;
				arryOut[1].m_v2Tutv.x	= 0.f,
				arryOut[1].m_v2Tutv.y	= 1.f;
								
				arryOut[2].m_dwColr = ulColr;
				DEF_SET_ALPHA(arryOut[2].m_dwColr,NALPHA);
				arryOut[2].m_v3Pos.x	= lastTailPoint.m_v3dP1.x,
				arryOut[2].m_v3Pos.y	= lastTailPoint.m_v3dP1.y,
				arryOut[2].m_v3Pos.z	= lastTailPoint.m_v3dP1.z;
				arryOut[2].m_v2Tutv.x	= 1.f,
				arryOut[2].m_v2Tutv.y	= 0.f;

				arryOut[3].m_dwColr = ulColr;
				DEF_SET_ALPHA(arryOut[3].m_dwColr,NALPHA);
				arryOut[3].m_v3Pos.x	= lastTailPoint.m_v3dP1.x,
				arryOut[3].m_v3Pos.y	= lastTailPoint.m_v3dP1.y,
				arryOut[3].m_v3Pos.z	= lastTailPoint.m_v3dP1.z;
				arryOut[3].m_v2Tutv.x	= 1.f,
				arryOut[3].m_v2Tutv.y	= 1.f;
			}
			break;
		case 2:
			{
				AgcdEffCtrl_Tail::TailPointListCItr	cit_curr
					= listTailPoint.begin();

				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint0 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint1 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));

				vBezier3( arryOut, nNumOfSampling//*2/5
						, &lpTailPoint0->m_v3dP1, &lpTailPoint0->m_v3dP2
						, &lpTailPoint1->m_v3dP1, &lpTailPoint1->m_v3dP2
						, &lastTailPoint.m_v3dP1, &lastTailPoint.m_v3dP2
						, ulColr );
			}break;
		case 3:
			{
				AgcdEffCtrl_Tail::TailPointListCItr	cit_curr
					= listTailPoint.begin();

				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint0 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint1 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint2 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));

				vBezier4( arryOut, nNumOfSampling//*3/5
						, &lpTailPoint0->m_v3dP1, &lpTailPoint0->m_v3dP2
						, &lpTailPoint1->m_v3dP1, &lpTailPoint1->m_v3dP2
						, &lpTailPoint2->m_v3dP1, &lpTailPoint2->m_v3dP2
						, &lastTailPoint.m_v3dP1, &lastTailPoint.m_v3dP2
						, ulColr  );
			}break;
		case 4:
			{
				AgcdEffCtrl_Tail::TailPointListCItr	cit_curr
					= listTailPoint.begin();

				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint0 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint1 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint2 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint3 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));

				vBezier5( arryOut, nNumOfSampling//*4/5
						, &lpTailPoint0->m_v3dP1, &lpTailPoint0->m_v3dP2
						, &lpTailPoint1->m_v3dP1, &lpTailPoint1->m_v3dP2
						, &lpTailPoint2->m_v3dP1, &lpTailPoint2->m_v3dP2
						, &lpTailPoint3->m_v3dP1, &lpTailPoint3->m_v3dP2
						, &lastTailPoint.m_v3dP1, &lastTailPoint.m_v3dP2
						, ulColr  );
			}break;
		case 5:
			{
				AgcdEffCtrl_Tail::TailPointListCItr	cit_curr
					= listTailPoint.begin();

				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint0 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint1 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint2 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint3 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));
				++cit_curr;
				const AgcdEffCtrl_Tail::TAILPOINT* lpTailPoint4 
					= &(static_cast<AgcdEffCtrl_Tail::TAILPOINT>(*cit_curr));

				vBezier6( arryOut, nNumOfSampling
						, &lpTailPoint0->m_v3dP1, &lpTailPoint0->m_v3dP2
						, &lpTailPoint1->m_v3dP1, &lpTailPoint1->m_v3dP2
						, &lpTailPoint2->m_v3dP1, &lpTailPoint2->m_v3dP2
						, &lpTailPoint3->m_v3dP1, &lpTailPoint3->m_v3dP2
						, &lpTailPoint4->m_v3dP1, &lpTailPoint4->m_v3dP2
						, &lastTailPoint.m_v3dP1, &lastTailPoint.m_v3dP2
						, ulColr  );
			}break;
		}
		return 0;
	};

private:
	RwInt32 vBezier3( VTX_TYPE arryOut[]
				   , RwInt32 nNumOfSampling
				   , const RwV3d* pv3dt0, const RwV3d* pv3db0
				   , const RwV3d* pv3dt1, const RwV3d* pv3db1
				   , const RwV3d* pv3dt2, const RwV3d* pv3db2
				   , RwUInt32 ulColr = 0x80808080
				   )
	{
		const RwReal	FALPHA	= static_cast<RwReal>(DEF_GET_ALPHA(ulColr));
		const RwUInt32	NALPHA	= static_cast<RwUInt32>(DEF_GET_ALPHA(ulColr));

		RwReal	fstep		= 1.f/static_cast<RwReal>(nNumOfSampling);
		RwReal	fU			= 0.f;
		RwReal	fOneMinusU	= 0.f;

		RwV3d	pos0		= { 0.f, 0.f, 0.f };
		RwV3d	pos1		= { 0.f, 0.f, 0.f };
		RwV3d	pos2		= { 0.f, 0.f, 0.f };

		LPD3DVTX_PCT pVtx	= &arryOut[0];
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3dt0->x,
		pVtx->m_v3Pos.y	= pv3dt0->y,
		pVtx->m_v3Pos.z	= pv3dt0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 0.f;
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3db0->x,
		pVtx->m_v3Pos.y	= pv3db0->y,
		pVtx->m_v3Pos.z	= pv3db0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 1.f;


		//nNumOfSampling -= 1;
		for( RwInt32 i=1; i<nNumOfSampling; ++i )
		{
			fU			+= fstep;
			fOneMinusU	 = 1.f - fU;

			//top
			RwV3dScale( &pos0, pv3dt0, fOneMinusU*fOneMinusU );
			RwV3dScale( &pos1, pv3dt1, 2.f*fU*fOneMinusU );
			RwV3dScale( &pos2, pv3dt2, fU*fU );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 0.f;
			
			//bottom
			RwV3dScale( &pos0, pv3db0, fOneMinusU*fOneMinusU );
			RwV3dScale( &pos1, pv3db1, 2.f*fU*fOneMinusU );
			RwV3dScale( &pos2, pv3db2, fU*fU );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 1.f;
		}
		
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3dt2->x,
		pVtx->m_v3Pos.y	= pv3dt2->y,
		pVtx->m_v3Pos.z	= pv3dt2->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 0.f;
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3db2->x,
		pVtx->m_v3Pos.y	= pv3db2->y,
		pVtx->m_v3Pos.z	= pv3db2->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 1.f;

		return 0;
	};
	RwInt32 vBezier4( VTX_TYPE arryOut[]
				   , RwInt32 nNumOfSampling
				   , const RwV3d* pv3dt0, const RwV3d* pv3db0
				   , const RwV3d* pv3dt1, const RwV3d* pv3db1
				   , const RwV3d* pv3dt2, const RwV3d* pv3db2
				   , const RwV3d* pv3dt3, const RwV3d* pv3db3
				   , RwUInt32 ulColr = 0x80808080 )
	{
		const RwReal	FALPHA	= static_cast<RwReal>(DEF_GET_ALPHA(ulColr));		
		const RwUInt32	NALPHA	= static_cast<RwUInt32>(DEF_GET_ALPHA(ulColr));
		RwReal	fstep		= 1.f/static_cast<RwReal>(nNumOfSampling);
		RwReal	fU			= 0.f;
		RwReal	fOneMinusU	= 0.f;

		RwV3d	pos0		= { 0.f, 0.f, 0.f };
		RwV3d	pos1		= { 0.f, 0.f, 0.f };
		RwV3d	pos2		= { 0.f, 0.f, 0.f };
		RwV3d	pos3		= { 0.f, 0.f, 0.f };

		LPD3DVTX_PCT pVtx	= &arryOut[0];
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3dt0->x,
		pVtx->m_v3Pos.y	= pv3dt0->y,
		pVtx->m_v3Pos.z	= pv3dt0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 0.f;
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3db0->x,
		pVtx->m_v3Pos.y	= pv3db0->y,
		pVtx->m_v3Pos.z	= pv3db0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 1.f;


		RwReal	fU2,	fOneMinusU2,
				fU3,	fOneMinusU3;

		//nNumOfSampling -= 1;
		for( RwInt32 i=1; i<nNumOfSampling; ++i )
		{
			fU			+= fstep;
			fOneMinusU	 = 1.f - fU;
			
			fU2	= fU*fU;
			fU3	= fU2*fU;

			fOneMinusU2	= fOneMinusU*fOneMinusU;
			fOneMinusU3 = fOneMinusU2*fOneMinusU;
			
			RwV3dScale( &pos0, pv3dt0, fOneMinusU3 );
			RwV3dScale( &pos1, pv3dt1, 3.f*fU*fOneMinusU2 );
			RwV3dScale( &pos2, pv3dt2, 3.f*fU2*fOneMinusU );
			RwV3dScale( &pos3, pv3dt3, fU3 );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );
			RwV3dAdd( &pos0, &pos0, &pos3 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 0.f;
			
			
			RwV3dScale( &pos0, pv3db0, fOneMinusU3 );
			RwV3dScale( &pos1, pv3db1, 3.f*fU*fOneMinusU2 );
			RwV3dScale( &pos2, pv3db2, 3.f*fU2*fOneMinusU );
			RwV3dScale( &pos3, pv3db3, fU3 );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );
			RwV3dAdd( &pos0, &pos0, &pos3 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 1.f;
		}
		
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3dt3->x,
		pVtx->m_v3Pos.y	= pv3dt3->y,
		pVtx->m_v3Pos.z	= pv3dt3->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 0.f;
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3db3->x,
		pVtx->m_v3Pos.y	= pv3db3->y,
		pVtx->m_v3Pos.z	= pv3db3->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 1.f;

		return 0;
	};
	RwInt32 vBezier5( VTX_TYPE arryOut[]
				   , RwInt32 nNumOfSampling
				   , const RwV3d* pv3dt0, const RwV3d* pv3db0
				   , const RwV3d* pv3dt1, const RwV3d* pv3db1
				   , const RwV3d* pv3dt2, const RwV3d* pv3db2
				   , const RwV3d* pv3dt3, const RwV3d* pv3db3
				   , const RwV3d* pv3dt4, const RwV3d* pv3db4
				   , RwUInt32 ulColr = 0x80808080 )
	{
		const RwReal	FALPHA	= static_cast<RwReal>(DEF_GET_ALPHA(ulColr));
		const RwUInt32	NALPHA	= static_cast<RwUInt32>(DEF_GET_ALPHA(ulColr));

		RwReal	fstep		= 1.f/static_cast<RwReal>(nNumOfSampling);
		RwReal	fU			= 0.f;
		RwReal	fOneMinusU	= 0.f;

		RwV3d	pos0		= { 0.f, 0.f, 0.f };
		RwV3d	pos1		= { 0.f, 0.f, 0.f };
		RwV3d	pos2		= { 0.f, 0.f, 0.f };
		RwV3d	pos3		= { 0.f, 0.f, 0.f };
		RwV3d	pos4		= { 0.f, 0.f, 0.f };

		LPD3DVTX_PCT pVtx	= &arryOut[0];
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3dt0->x,
		pVtx->m_v3Pos.y	= pv3dt0->y,
		pVtx->m_v3Pos.z	= pv3dt0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 0.f;
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3db0->x,
		pVtx->m_v3Pos.y	= pv3db0->y,
		pVtx->m_v3Pos.z	= pv3db0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 1.f;

		RwReal	fU2,	fOneMinusU2,
				fU3,	fOneMinusU3,
				fU4,	fOneMinusU4;

		//nNumOfSampling -= 1;
		for( RwInt32 i=1; i<nNumOfSampling; ++i )
		{
			fU			+= fstep;
			fOneMinusU	 = 1.f - fU;
			
			fU2	= fU*fU;
			fU3	= fU2*fU;
			fU4	= fU3*fU;

			fOneMinusU2	= fOneMinusU*fOneMinusU;
			fOneMinusU3 = fOneMinusU2*fOneMinusU;
			fOneMinusU4 = fOneMinusU3*fOneMinusU;


			RwV3dScale( &pos0, pv3dt0, fOneMinusU4 );
			RwV3dScale( &pos1, pv3dt1, 4.f*fU*fOneMinusU3 );
			RwV3dScale( &pos2, pv3dt2, 6.f*fU2*fOneMinusU2 );
			RwV3dScale( &pos3, pv3dt3, 4.f*fU3*fOneMinusU );
			RwV3dScale( &pos4, pv3dt4, fU4 );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );
			RwV3dAdd( &pos0, &pos0, &pos3 );
			RwV3dAdd( &pos0, &pos0, &pos4 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 0.f;
			
			
			RwV3dScale( &pos0, pv3db0, fOneMinusU4 );
			RwV3dScale( &pos1, pv3db1, 4.f*fU*fOneMinusU3 );
			RwV3dScale( &pos2, pv3db2, 6.f*fU2*fOneMinusU2 );
			RwV3dScale( &pos3, pv3db3, 4.f*fU3*fOneMinusU );
			RwV3dScale( &pos4, pv3db4, fU4 );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );
			RwV3dAdd( &pos0, &pos0, &pos3 );
			RwV3dAdd( &pos0, &pos0, &pos4 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 1.f;
		}
		
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3dt4->x,
		pVtx->m_v3Pos.y	= pv3dt4->y,
		pVtx->m_v3Pos.z	= pv3dt4->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 0.f;
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3db4->x,
		pVtx->m_v3Pos.y	= pv3db4->y,
		pVtx->m_v3Pos.z	= pv3db4->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 1.f;

		return 0;
	};

	RwInt32 vBezier6( VTX_TYPE arryOut[]
				   , RwInt32 nNumOfSampling
				   , const RwV3d* pv3dt0, const RwV3d* pv3db0
				   , const RwV3d* pv3dt1, const RwV3d* pv3db1
				   , const RwV3d* pv3dt2, const RwV3d* pv3db2
				   , const RwV3d* pv3dt3, const RwV3d* pv3db3
				   , const RwV3d* pv3dt4, const RwV3d* pv3db4
				   , const RwV3d* pv3dt5, const RwV3d* pv3db5
				   , RwUInt32 ulColr = 0x80808080 )
	{
		const RwReal	FALPHA	= static_cast<RwReal>(DEF_GET_ALPHA(ulColr));
		const RwUInt32	NALPHA	= static_cast<RwUInt32>(DEF_GET_ALPHA(ulColr));
		RwReal	fstep		= 1.f/static_cast<RwReal>(nNumOfSampling);
		RwReal	fU			= 0.f;
		RwReal	fOneMinusU	= 0.f;

		RwV3d	pos0		= { 0.f, 0.f, 0.f };
		RwV3d	pos1		= { 0.f, 0.f, 0.f };
		RwV3d	pos2		= { 0.f, 0.f, 0.f };
		RwV3d	pos3		= { 0.f, 0.f, 0.f };
		RwV3d	pos4		= { 0.f, 0.f, 0.f };
		RwV3d	pos5		= { 0.f, 0.f, 0.f };

		LPD3DVTX_PCT pVtx	= &arryOut[0];
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3dt0->x,
		pVtx->m_v3Pos.y	= pv3dt0->y,
		pVtx->m_v3Pos.z	= pv3dt0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 0.f;

		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,0);
		pVtx->m_v3Pos.x	= pv3db0->x,
		pVtx->m_v3Pos.y	= pv3db0->y,
		pVtx->m_v3Pos.z	= pv3db0->z;
		pVtx->m_v2Tutv.x	= 0.f;
		pVtx->m_v2Tutv.y	= 1.f;

		RwReal	fU2,	fOneMinusU2,
				fU3,	fOneMinusU3,
				fU4,	fOneMinusU4,
				fU5,	fOneMinusU5;

		

		//nNumOfSampling -= 1;
		for( RwInt32 i=1; i<nNumOfSampling; ++i )
		{
			fU			+= fstep;
			fOneMinusU	 = 1.f - fU;

			fU2	= fU*fU;
			fU3	= fU2*fU;
			fU4	= fU3*fU;
			fU5	= fU4*fU;

			fOneMinusU2	= fOneMinusU*fOneMinusU;
			fOneMinusU3 = fOneMinusU2*fOneMinusU;
			fOneMinusU4 = fOneMinusU3*fOneMinusU;
			fOneMinusU5 = fOneMinusU4*fOneMinusU;

			
			RwV3dScale( &pos0, pv3dt0, fOneMinusU5 );
			RwV3dScale( &pos1, pv3dt1, 5.f*fU*fOneMinusU4 );
			RwV3dScale( &pos2, pv3dt2, 10.f*fU2*fOneMinusU3 );
			RwV3dScale( &pos3, pv3dt3, 10.f*fU3*fOneMinusU2 );
			RwV3dScale( &pos4, pv3dt4, 5.f*fU4*fOneMinusU );
			RwV3dScale( &pos5, pv3dt5, fU5 );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );
			RwV3dAdd( &pos0, &pos0, &pos3 );
			RwV3dAdd( &pos0, &pos0, &pos4 );
			RwV3dAdd( &pos0, &pos0, &pos5 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 0.f;
			
					
			RwV3dScale( &pos0, pv3db0, fOneMinusU5 );
			RwV3dScale( &pos1, pv3db1, 5.f*fU*fOneMinusU4 );
			RwV3dScale( &pos2, pv3db2, 10.f*fU2*fOneMinusU3 );
			RwV3dScale( &pos3, pv3db3, 10.f*fU3*fOneMinusU2 );
			RwV3dScale( &pos4, pv3db4, 5.f*fU4*fOneMinusU );
			RwV3dScale( &pos5, pv3db5, fU5 );

			RwV3dAdd( &pos0, &pos0, &pos1 );
			RwV3dAdd( &pos0, &pos0, &pos2 );
			RwV3dAdd( &pos0, &pos0, &pos3 );
			RwV3dAdd( &pos0, &pos0, &pos4 );
			RwV3dAdd( &pos0, &pos0, &pos5 );

			++pVtx;
			pVtx->m_dwColr = ulColr;
			DEF_SET_ALPHA(pVtx->m_dwColr,static_cast<unsigned int>(fU*FALPHA));
			pVtx->m_v3Pos.x	= pos0.x,
			pVtx->m_v3Pos.y	= pos0.y,
			pVtx->m_v3Pos.z	= pos0.z;
			pVtx->m_v2Tutv.x= fU;
			pVtx->m_v2Tutv.y= 1.f;
		}
		
		
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3dt5->x,
		pVtx->m_v3Pos.y	= pv3dt5->y,
		pVtx->m_v3Pos.z	= pv3dt5->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 0.f;
		
		++pVtx;
		pVtx->m_dwColr = ulColr;
		DEF_SET_ALPHA(pVtx->m_dwColr,NALPHA);
		pVtx->m_v3Pos.x	= pv3db5->x,
		pVtx->m_v3Pos.y	= pv3db5->y,
		pVtx->m_v3Pos.z	= pv3db5->z;
		pVtx->m_v2Tutv.x	= 1.f;
		pVtx->m_v2Tutv.y	= 1.f;

		return 0;
	};


};

///////////////////////////////////////////////////////////////////////////////
// catmull-rom curve
// gameprogramming gems 1 page 355
///////////////////////////////////////////////////////////////////////////////
template<class VTX_TYPE>
class AgcuCatmull_Rom_Tail
{
	const FLOAT COF05;//0.5f
	const FLOAT COF10;//1.0f
	const FLOAT COF15;//1.5f
	const FLOAT COF20;//2.0f
	const FLOAT COF25;//2.5f



public:
	AgcuCatmull_Rom_Tail() 
		: COF05(0.5f) 
		, COF10(1.0f) 
		, COF15(1.5f) 
		, COF20(2.0f) 
		, COF25(2.5f) 
	{
	};

	RwInt32	bCatmull_Rom( VTX_TYPE arrOut[]
		, const AgcdEffCtrl_Tail::TailPointList& listTailPoint
		, const AgcdEffCtrl_Tail::TAILPOINT& currTailPoint
		, int nNumSamplingPerSegment	//각 테일 포인트사이당 샘플링 갯수. 작게 하자..
		, RwUInt32 ulColr = 0x80808080 
		)
	{
		int nSize	= static_cast<int>(listTailPoint.size());

		switch( nSize )
		{
		case 0:
			//do nothing
			break;

		case 1:
			{
			//draw only one rect
			AgcuBezierForTail<VTX_TYPE>	agcuBezier;

			agcuBezier.bBezier( arrOut
				, nNumSamplingPerSegment
				, currTailPoint
				, listTailPoint
				, ulColr);
			}
			break;

		case 2:
			{
			//draw vBezier3
			AgcuBezierForTail<VTX_TYPE>	agcuBezier;

			agcuBezier.bBezier( arrOut
				, nNumSamplingPerSegment*2
				, currTailPoint
				, listTailPoint
				, ulColr );
			}
			break;

		case 3:
			{
			const float fustep	= 1.f / ( static_cast<float>(nNumSamplingPerSegment) );
			const float ftexUStep	= 1.f / ( static_cast<float>(nNumSamplingPerSegment*nSize) );
			float		fCurrTexU	= 0.f;

			AgcdEffCtrl_Tail::TailPointListCItr it_curr	= listTailPoint.begin();

			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint1	= NULL;
			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint2	= NULL;
			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint3	= NULL;
			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint4	= NULL;

			//1st segment
			it_curr	= listTailPoint.begin();
			pTailPoint1	= &(*it_curr);
			pTailPoint2	= &(*it_curr);
			pTailPoint3	= &(*(++it_curr));
			pTailPoint4	= &(*(++it_curr));

			vCatMull_Rom( arrOut
				, pTailPoint1
				, pTailPoint2
				, pTailPoint3
				, pTailPoint4
				, nNumSamplingPerSegment
				, fustep
				, fCurrTexU
				, ftexUStep
				, ulColr
				);
			
			//2nd segment
			it_curr	= listTailPoint.begin();
			pTailPoint1	= &(*it_curr);
			pTailPoint2	= &(*(++it_curr));
			pTailPoint3	= &(*(++it_curr));
			pTailPoint4	= &currTailPoint;

			vCatMull_Rom( arrOut
				, pTailPoint1
				, pTailPoint2
				, pTailPoint3
				, pTailPoint4
				, nNumSamplingPerSegment
				, fustep
				, fCurrTexU
				, ftexUStep
				, ulColr
				);
			
			//last segment
			it_curr	= listTailPoint.begin();
			pTailPoint1	= &(*(++it_curr));
			pTailPoint2	= &(*(++it_curr));
			pTailPoint3	= &currTailPoint;
			pTailPoint4	= &currTailPoint;

			vCatMull_Rom( arrOut
				, pTailPoint1
				, pTailPoint2
				, pTailPoint3
				, pTailPoint4
				, nNumSamplingPerSegment
				, fustep
				, fCurrTexU
				, ftexUStep
				, ulColr
				);
			}
			break;

		default:
			{
			const float fustep	= 1.f / ( static_cast<float>(nNumSamplingPerSegment) );
			const float ftexUStep	= 1.f / ( static_cast<float>(nNumSamplingPerSegment*nSize) );
			float		fCurrTexU	= 0.f;
			AgcdEffCtrl_Tail::TailPointListCItr it_curr	= listTailPoint.begin();

			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint1	= NULL;
			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint2	= NULL;
			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint3	= NULL;
			const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint4	= NULL;

			//1st segment
			it_curr	= listTailPoint.begin();
			pTailPoint1	= &(*it_curr);
			pTailPoint2	= &(*it_curr);
			pTailPoint3	= &(*(++it_curr));
			pTailPoint4	= &(*(++it_curr));

			vCatMull_Rom( arrOut
				, pTailPoint1
				, pTailPoint2
				, pTailPoint3
				, pTailPoint4
				, nNumSamplingPerSegment
				, fustep
				, fCurrTexU
				, ftexUStep
				, ulColr
				);

			//middle segment
			it_curr	= listTailPoint.begin();
			for( int i=0; i<nSize-3; ++i, it_curr = listTailPoint.begin() )
			{
				for( int j=0; j<i; ++j )
					++it_curr;
				pTailPoint1	= &(*it_curr);
				pTailPoint2	= &(*(++it_curr));
				pTailPoint3	= &(*(++it_curr));
				pTailPoint4	= &(*(++it_curr));

				vCatMull_Rom( arrOut
					, pTailPoint1
					, pTailPoint2
					, pTailPoint3
					, pTailPoint4
					, nNumSamplingPerSegment
					, fustep
					, fCurrTexU
					, ftexUStep
					, ulColr
					);
			}
			

			//before the last segment
			AgcdEffCtrl_Tail::TailPointListCRItr	it_rcurr
				= listTailPoint.rbegin();
			pTailPoint4	= &currTailPoint;
			pTailPoint3	= &(*it_rcurr);
			pTailPoint2	= &(*(++it_rcurr));
			pTailPoint1	= &(*(++it_rcurr));

			vCatMull_Rom( arrOut
				, pTailPoint1
				, pTailPoint2
				, pTailPoint3
				, pTailPoint4
				, nNumSamplingPerSegment
				, fustep
				, fCurrTexU
				, ftexUStep
				, ulColr
				);
			
			//last segment
			it_rcurr	= listTailPoint.rbegin();
			pTailPoint4	= &currTailPoint;
			pTailPoint3	= &currTailPoint;
			pTailPoint2	= &(*it_rcurr);
			pTailPoint1	= &(*(++it_rcurr));

			vCatMull_Rom( arrOut
				, pTailPoint1
				, pTailPoint2
				, pTailPoint3
				, pTailPoint4
				, nNumSamplingPerSegment
				, fustep
				, fCurrTexU
				, ftexUStep
				, ulColr
				);
			}
			break;
		};

		return 0;
	};

private:

	RwInt32 vCatMull_Rom(  VTX_TYPE*& ptrRefOut
		, const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint1
		, const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint2
		, const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint3
		, const AgcdEffCtrl_Tail::TAILPOINT*	pTailPoint4
		, int nSamplingNum
		, float fustep
		, float &fTexU
		, float fTexUStep
		, RwUInt32 ulColr = 0x80808080 
		)
	{	
		const RwReal	FALPHA	= static_cast<RwReal>(DEF_GET_ALPHA(ulColr));
		const RwUInt32	NALPHA	= static_cast<RwUInt32>(DEF_GET_ALPHA(ulColr));
		ptrRefOut->m_dwColr = ulColr;
		DEF_SET_ALPHA(ptrRefOut->m_dwColr,static_cast<unsigned char>(fTexU*FALPHA));
		ptrRefOut->m_v3Pos	= (*(D3DXVECTOR3*)( &pTailPoint2->m_v3dP1 ));
		ptrRefOut->m_v2Tutv.x	= fTexU;
		ptrRefOut->m_v2Tutv.y	= 0.f;
		++ptrRefOut;
		ptrRefOut->m_dwColr = ulColr;
		DEF_SET_ALPHA(ptrRefOut->m_dwColr,static_cast<unsigned char>(fTexU*FALPHA));
		ptrRefOut->m_v3Pos	= (*(D3DXVECTOR3*)( &pTailPoint2->m_v3dP2 ));
		ptrRefOut->m_v2Tutv.x	= fTexU;
		ptrRefOut->m_v2Tutv.y	= 1.f;
		++ptrRefOut;

		nSamplingNum		-= 1;
		float	fu	= fustep;
		fTexU+=fTexUStep;
		for( int i=1; i<nSamplingNum; ++i, fu+=fustep, fTexU+=fTexUStep )
		{

			ptrRefOut->m_dwColr = ulColr;
			DEF_SET_ALPHA(ptrRefOut->m_dwColr,static_cast<unsigned char>(fTexU*FALPHA));
			D3DXVec3CatmullRom( &ptrRefOut->m_v3Pos
				, (D3DXVECTOR3*)( &pTailPoint1->m_v3dP1 )
				, (D3DXVECTOR3*)( &pTailPoint2->m_v3dP1 )
				, (D3DXVECTOR3*)( &pTailPoint3->m_v3dP1 )
				, (D3DXVECTOR3*)( &pTailPoint4->m_v3dP1 )
				, fu );
			ptrRefOut->m_v2Tutv.x	= fTexU;
			ptrRefOut->m_v2Tutv.y	= 0.f;
			++ptrRefOut;

			ptrRefOut->m_dwColr = ulColr;
			DEF_SET_ALPHA(ptrRefOut->m_dwColr,static_cast<unsigned char>(fTexU*FALPHA));
			D3DXVec3CatmullRom( &ptrRefOut->m_v3Pos
				, (D3DXVECTOR3*)( &pTailPoint1->m_v3dP2 )
				, (D3DXVECTOR3*)( &pTailPoint2->m_v3dP2 )
				, (D3DXVECTOR3*)( &pTailPoint3->m_v3dP2 )
				, (D3DXVECTOR3*)( &pTailPoint4->m_v3dP2 )
				, fu );
			ptrRefOut->m_v2Tutv.x	= fTexU;
			ptrRefOut->m_v2Tutv.y	= 1.f;
			++ptrRefOut;
		}

		ptrRefOut->m_dwColr = ulColr;
		DEF_SET_ALPHA(ptrRefOut->m_dwColr,static_cast<unsigned char>(fTexU*FALPHA));
		ptrRefOut->m_v3Pos	= (*(D3DXVECTOR3*)( &pTailPoint3->m_v3dP1 ));
		ptrRefOut->m_v2Tutv.x	= fTexU;
		ptrRefOut->m_v2Tutv.y	= 0.f;
		++ptrRefOut;
		ptrRefOut->m_dwColr = ulColr;
		DEF_SET_ALPHA(ptrRefOut->m_dwColr,static_cast<unsigned char>(fTexU*FALPHA));
		ptrRefOut->m_v3Pos	= (*(D3DXVECTOR3*)( &pTailPoint3->m_v3dP2 ));
		ptrRefOut->m_v2Tutv.x	= fTexU;
		ptrRefOut->m_v2Tutv.y	= 1.f;
		++ptrRefOut;
		fTexU+=fTexUStep;
		return 0;

	};

	RwInt32	vCatMull_Rom( VTX_TYPE*& ptrRefOut
		, const D3DXVECTOR3& v1
		, const D3DXVECTOR3& v2
		, const D3DXVECTOR3& v3
		, const D3DXVECTOR3& v4
		, int nSamplingNum
		, float fustep
		)
	{
		ptrRefOut->m_v3Pos	= v2;

		nSamplingNum		-= 1;
		float	fu	= fustep;
		for( int i=1; i<nSamplingNum; ++i, ++ptrRefOut, fu+=fustep )
		{
			vCatmull_Rom( &ptrRefOut->m_v3Pos, v1, v2, v3, v4, fu );
		}
		ptrRefOut->m_v3Pos	= v3;

		++ptrRefOut;//for next func call
		return 0;
	}

	inline RwInt32 vCatmull_Rom( D3DXVECTOR3* pOut
					, const D3DXVECTOR3& v1
					, const D3DXVECTOR3& v2
					, const D3DXVECTOR3& v3
					, const D3DXVECTOR3& v4
					, float	fu	// 0.0 ~ 1.0;
					)
	{
		//D3DXVec3CatmullRom( pOut, &v1, &v2, &v3, &v4, fu );
		
		const float fu2	= fu*fu;
		const float fu3	= fu2*fu;

		*pOut	= v1 * ( -COF05*fu3 + COF10*fu2 - COF05*fu )
				+ v2 * (  COF15*fu3 - COF25*fu2 + COF10 )
				+ v3 * ( -COF15*fu3 + COF20*fu2 + COF05*fu )
				+ v4 * (  COF05*fu3 - COF05*fu2 );/**/

		return 0;
	};
};

#endif