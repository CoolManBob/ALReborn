#ifndef		_ACUPARTICLEDRAW_H_
#define		_ACUPARTICLEDRAW_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuParticleDrawD" )
#else
#pragma comment ( lib , "AcuParticleDraw" )
#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>

#include "RwCore.h"
//#include "RpWorld.h"
//#include "ApBase.h"

typedef struct
{
	D3DXVECTOR3		position;
	D3DCOLOR		color;
	float			u,v;
} VERTEX_PARTICLE;
#define		D3DFVF_PARTICLE (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// 하드웨어 particle지원시..사용
typedef struct 
{
    D3DXVECTOR3 position;
    float       pointsize;
    D3DCOLOR    color;
} VERTEX_PARTICLE_PSIZE;
#define D3DFVF_PARTICLE_PSIZE (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_PSIZE)

enum	ParticleBillboardType
{
	PARTICLE_BILLBOARD = 0,
	PARTICLE_YBILLBOARD = 1,		// y축 빌보드
};

// 제약: particle의 uv값은 항상 0 1 로 set되므로 텍스쳐 1장으로 set하자
//		 PointSprite로 처리시에는 64를 최대로 하자.. 대부분 64까지 지원한다..
class	AcuParticleDraw
{
public:
	AcuParticleDraw();
	~AcuParticleDraw();

	static	void	Init();
	static	void	Release();

	static	void	CameraMatrixUpdate();

	static	void	DrawStart(RwTexture*	pTexture,ParticleBillboardType	eBillboardType);
	static	void	AccumulateBuffer(RwV3d*		pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight);
	static	void	DrawEnd();

	static	void	AccumulateBufferRotationSelf(RwV3d*		pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight,FLOAT	rot_val);
	static	void	AccumulateBufferRotationMatrix(RwV3d*		pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight,RwMatrix*	rot_matrix);
	
	// 넘길 group의 한계 pointsize를 넘긴다.. 모를 경우 64를 넘기면 된다..
	static	void	DrawStartPoint(RwTexture*	pTexture,INT32	iMaxPointSize);
	static	void	AccumulateBufferPoint(RwV3d*		pPos,DWORD	color,FLOAT	fSize);
	static	void	DrawEndPoint();

	static	void	SetWorldCamera(RwCamera*	pCam);
	
public:
	// Vertex Buffer
	static		VERTEX_PARTICLE*			m_pBuffer;
	static		INT16*						m_pIndexBuffer;
	static		VERTEX_PARTICLE_PSIZE*		m_pBuffer_PSize;

	static		INT32		m_iMaxBufferVerticeIndex;
	static		INT32		m_iCurBufferVerticeIndex;
	static		INT32		m_iCurBuffer;

	static		FLOAT		m_fWeightPSize[64];				// Point 를 사용시에 billboard 방식과 size일치 시키기 위한 계수

	static		INT32		m_iMaxBufferPSize;
	static		INT32		m_iCurBufferPSize;
	static		INT32		m_iCountPSize[64];
	static		INT32		m_iMaxPSize;

	static		FLOAT				m_fMaxPointSize;		// H/W 가 PSIZE로 처리 가능한 size
	static		RwCamera*			m_pCamera;

	static		IDirect3DDevice9*	m_pCurD3D9Device;
	static		BOOL				m_bEnablePSize;			// MaxPSize가 8보다 작다면 disable

	//DrawStart시마다 갱신..
	static		RwTexture*				m_pDrawTexture;
	static		ParticleBillboardType	m_eBillboardType;
		
	static		RwMatrix			m_matBillboard;
	static		RwMatrix			m_matBillboardY;

	static		D3DMATRIX			m_matD3DIdentity;
};

#endif