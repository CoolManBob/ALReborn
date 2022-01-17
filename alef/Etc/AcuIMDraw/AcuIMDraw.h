#ifndef		_ACUIMDRAW_H_
#define		_ACUIMDRAW_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuIMDrawD" )
#else
#pragma comment ( lib , "AcuIMDraw" )
#endif
#endif

#include "RwCore.h"
#include "RpWorld.h"

#include "ApBase.h"

#include "AgcuBasisGeo.h"

// debug용 으로 쓰세요~ ,, 실제 게임에 쓰이는 거라면 따로 만드시는게 좋을 겁니다. 매번 vertex set하므로(camera z관련해서)..
class	AcuIMDraw
{
public:
	AcuIMDraw();
	~AcuIMDraw();

	static	void	Draw2DLine(RwV2d*	p1,RwV2d*	p2,DWORD color = 0xffffffff);				// 2D line을 그린다.

	static	void 	DrawLine(RwV3d*	p1,RwV3d* p2,DWORD color = 0xffffffff);
	//vertex array를 넘긴다. pnum은 점개수 .. vertex array는 점 연결 목록이다. 1 - 2 - 3 - 4 - 5 이렇게 그려짐
	static	void 	DrawLines(RwV3d*	p1,INT32 pNum,DWORD color = 0xffffffff);		

	static	void	DrawBox(RwBBox*	pBox,DWORD color = 0xffffffff);
	static	void	DrawSphere(RwSphere*	pSphere,DWORD color = 0xffffffff);

	
	//@{ kday 20051006
	// ;)
	// colr == AARRGGBB;
	static RwInt32 RenderLine( const RwV3d& v1, const RwV3d& v2, RwUInt32 colr = 0xffffffff );
	static RwInt32 RenderLineStrip( const RwV3d varr[], RwInt32 num, RwUInt32 colr = 0xffffffff );
	//@} kday
	
public:
	static RwCamera*		m_pCamera;

	static void	SetWorldCamera(RwCamera*	pCam);
};


//@{ kday 20050304
#include "AgcuVtxType.h"
#include <list>
#include <algorithm>
namespace AXISVIEW
{
	namespace PRIVATE
	{
		struct	Axis
		{
			RwMatrix	mat;
			RwUInt32	colr;

			Axis()
			{
			};
			Axis( const RwMatrix& mat, const RwUInt32 colr=0xffffffff )
			{
				Axis::mat	= mat;
				Axis::colr	= colr;
			}
			Axis( const RwV3d& right, const RwV3d& up, const RwV3d& at, const RwV3d& pos, const RwUInt32 colr=0xffffffff )
			{
				Axis::mat.right	= right;
				Axis::mat.up	= up;
				Axis::mat.at	= at;
				Axis::mat.pos	= pos;
				
				Axis::colr	= colr;
			};
		};

		struct	VtxAxis
		{
			RwV3d		pos;
			RwUInt32	diff;
		};

		inline VOID RenderAxis(const Axis& axis)
		{
			const	RwReal	LEN	= 500.F;
			static VtxAxis	VTX[4]
			= {
				{{ 0.f, 0.f, 0.f }, 0xffffffff},	//orgin-white
				{{ LEN, 0.f, 0.f }, 0xffff0000},	//x-red
				{{ 0.f, LEN, 0.f }, 0xff00ff00},	//y-green
				{{ 0.f, 0.f, LEN }, 0xff0000ff},	//z-blue
			};
			static WORD	INDEX[6] = { 1,0,	2,0,	3,0, };

			VTX[0].diff	= axis.colr;

			
			USING_AGCUGEOUD;
			AgcuRenderStateForWire Setrenderstate;

			RwD3D9SetFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE);
			RwD3D9SetTransformWorld ( &axis.mat );
			RwD3D9DrawIndexedPrimitiveUP (
				D3DPT_LINELIST
				, 0LU
				, 4
				, 3
				, INDEX
				, VTX
				, sizeof(VtxAxis)
				);
		};

		class fnctrRender
		{
		public:
			void operator () (const Axis& axis)
			{
				RenderAxis(axis);
			}
		};
	};

	inline VOID RenderAxisContainer()
	{
		RwUInt32	lighting	= 0LU;
		RwD3D9GetRenderState(D3DRS_LIGHTING, (PVOID)&lighting);
		RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);

		using namespace PRIVATE;
		extern std::list<Axis> g_axisContainer;
		std::for_each( g_axisContainer.begin(), g_axisContainer.end(), fnctrRender() );

		RwD3D9SetRenderState(D3DRS_LIGHTING, lighting);
	}

	inline VOID RenderFrame(RwFrame* pFrm, DWORD colr)
	{
		using namespace PRIVATE;
		Axis	axis( *RwFrameGetLTM(pFrm), colr );
		fnctrRender rnd;
		rnd(axis);
	}

	inline VOID AddAxis(const RwMatrix& mat, const RwUInt32	colr = 0xffffffff)
	{
		using namespace PRIVATE;
		extern std::list<Axis> g_axisContainer;
		Axis ins(mat, colr);
		g_axisContainer.push_back( ins );
	}
	inline VOID	AddAxis(RwFrame* frm, const RwUInt32	colr = 0xffffffff)
	{
		AddAxis( *RwFrameGetLTM(frm), colr );
	}
	inline VOID	AddAxis(const RwV3d& right, const RwV3d& up, const RwV3d& at, const RwV3d& pos, const RwUInt32 colr = 0xffffffff)
	{
		using namespace PRIVATE;
		extern std::list<Axis> g_axisContainer;
		Axis ins(right, up, at, pos, colr);
		g_axisContainer.push_back( ins );
	}

	inline VOID ClearAxis()
	{
		using namespace PRIVATE;
		extern std::list<Axis> g_axisContainer;
		g_axisContainer.clear();
	}
};
//@} kday
#endif