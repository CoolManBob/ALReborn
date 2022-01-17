#ifndef ACURPMTEXTURE_H
#define ACURPMTEXTURE_H

#include "rwcore.h"
#include "rpworld.h"
#include "RpUsrdat.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpMTextureD" )
#else
#pragma comment ( lib , "AcuRpMTexture" )
#endif
#endif


#define rwVENDORID_NHN		(0xfffff0L)
#define rwID_MTEXTURE_NHN	(0x01)

#define rwID_MTEXTURE		MAKECHUNKID(rwVENDORID_NHN, rwID_MTEXTURE_NHN)

typedef enum RpMTextureType
{
	rpMTEXTURE_TYPE_NONE = 0,		// Texture가 없다.
	rpMTEXTURE_TYPE_NORMAL,			// Alpha가 없는 Texture를 위에 찍는다. (바로 전에 rpMTEXTURE_TYPE_ALPHA가 있었다면, 해당 Alpha가 먹는다.)
	rpMTEXTURE_TYPE_NORMALALPHA,	// Alpha가 있는 Texture를 위에 찍는다. (바로 전에 rpMTEXTURE_TYPE_ALPHA가 있었다면, 무시된다.)
	rpMTEXTURE_TYPE_ALPHA,			// Alpha를 적용한다. (rpMTEXTURE_TYPE_NORMAL에만 먹는다.)
	rpMTEXTURE_TYPE_BUMP,			// 아직 구현 안되있다. -_-;
} RpMTextureType;


#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

// world camera와 RwMatrix (shadow2의 invcameramatrix)필요 
extern RwCamera*	RPMTEX_WorldCamera		;
extern RwMatrix*	RPMTEX_invShadowMatrix	;

//extern RwMatrix*	RPMTEX_LightCameraViewMatrix;
//extern RwMatrix*	RPMTEX_LightCameraProjMatrix;

extern RwTexture*	RPMTEX_ShadowTexture	;

/* Attach Plugin */
extern RwInt32 
RpMTexturePluginAttach(void);

/* Set Second Texture (First texture is material's texture)*/
extern RwTexture *
RpMTextureMaterialGetTexture(RpMaterial *material, RwInt16 index, RpMTextureType *type);

/* Get Second Texture */
extern RpMaterial *
RpMTextureMaterialSetTexture(RpMaterial *material, RwInt16 index, RwTexture *texture, RpMTextureType type);

/* Enable Multi-texture at Atomic */
extern RpAtomic *
RpMTextureAtomicEnableEffect(RpAtomic *atomic);

/* Enable Multi-texture at Clump */
extern RpClump *
RpMTextureClumpEnableEffect(RpClump *pClump);

/* Enable Multi-Texture at World Sector */
extern RpWorldSector *
RpMTextureWorldSectorEnableEffect(RpWorldSector *worldSector);

extern void 
RpMTextureSetCamera(RwCamera*	camera);

extern void 
RpMTextureSetInvSMatrix(RwMatrix*	matrix);

extern	void 
RpMTextureSetShadowTexture(RwTexture*	tex);

extern	void 
RpMTextureSetBlendMode(RwUInt32  nBlendMode);
// 텍스쳐 블랜드 설정 ... D3DTOP_MODULATE2X / D3DTOP_MODULATE

extern	RwInt32
RpMTextureSetMaxTexturePerPass( RwInt32 nPass );
// 마고자 (2004-04-29 오후 6:26:42) : 맵툴 디버그용 펑션..
// 멀티패스 갯수를 조절한다.. 용도는 1로 설정할경우 , 멀티텍스쳐를
// 사용하지 않는데.. 이걸 보기위해서 사용한다.

extern	RwBool
RpMTextureEnableVertexShader( RwBool bEnable );
// 마고자 (2004-05-??) : 
// 버택스 셰이더 사용하지 않음~

extern	RwBool
RpMTextureIsEnableVertexShader();
// 마고자 (2004-07-19 오후 5:31:11) : 상태 리턴..

extern	RwBool
RpMTextureEnableLinearFog( RwBool bEnable );
extern	RwBool
RpMTextureIsEnableLinearFog();
// 마고자 (2004-05-19 오전 5:09:52) : 
// 툴에서만이라도 리니어 포그 가능하게.

// MTextureOpen 에서 하던것을 따로 호출해주어야 함(shader 코드는 그래픽 카드 체크후 나중에 생성)
extern	void	
RpMTextureCreateShaders();

//extern void 
//RpMTextureSetLightViewMatrix(RwMatrix*	matrix);

//extern void 
//RpMTextureSetLightProjMatrix(RwMatrix*	matrix);

extern	void
RpMTextureDrawStart();
// 지형 그리기 스타트(renderstate별로 누적 시작)

extern void 
RpMTextureDrawEnd(RwBool	bUseShader);
// render state별로 모아논 data를 그린다.(bUseShader가 false이면 shader안쓴다!)

extern void
RpMTextureSetFadeFactor( float fNear , float fFar );

extern void
RpMTextureSetFadeSectorDist ( RwInt32 dist );


//@{ Jaewon 20050512
// Terrain gloss map support
extern RwBool
RpMTextureEnableGlossMap(RwBool enable);
extern RwBool
RpMTextureIsGlossMapEnabled();
//@} Jaewon
#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPMTEXTURE_H */
