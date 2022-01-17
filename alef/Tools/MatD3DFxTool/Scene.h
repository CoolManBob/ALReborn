//@{ Jaewon 20040823
// created.
// a scene management class for MatD3DFxTool.
//@} Jaewon

#ifndef __MATD3DFXTOOL_SCENE_H__
#define __MATD3DFXTOOL_SCENE_H__

#include "rwcore.h"
#include "rpworld.h"
#include "RpSkin.h"
//@{ Jaewon 20041215
#include "AcuRtAmbOcclMap.h"
//@} Jaewon

class Scene
{
public:
	//@{ Jaewon 20041102
	// model statistics
	struct TextureStats
	{
		RwChar m_name[64];
		RwUInt32 m_texWidth, m_texHeight, m_texBPP;
	};
	struct MaterialStats
	{
		TextureStats *m_pTexStats;
	};
	struct AtomicStats
	{
		RwUInt32 m_nTris;
		RwUInt32 m_nMats;
		RwUInt32 m_nBones;
		RwUInt32 m_nMatsBeforeSplit;

		MaterialStats m_material[100];
	};
	struct Stats
	{
		RwUInt32 m_nTris;
		RwUInt32 m_nAtomics;
		RwUInt32 m_nTexs;

		AtomicStats m_atomic[100];
		TextureStats m_texture[30];
	};
	//@} Jaewon

	// constructor
	Scene();
	// destructor
	~Scene();

	// set a new fov & update the camera according to it.
	void updateCamera();
	// reset the camera.
	void resetCamera();

	// update the animation, if any.
	void updateAnimation(float fElapsedTime);

	// load from a rws file.
	bool loadRWS(const char *pathname);
	// save to a rws file.
	bool saveRWS(const char *pathname) const;

	// load a animation from a rws.
	bool loadAnimRWS(const char *pathname);

	// access methods
	RpWorld *getWorld() const { return m_pWorld; }
	RwCamera *getCamera() const { return m_pCamera; }
	RpClump *getClump() const { return m_pClump; }
	//@{ Jaewon 20040924
	RpLight *getAmbientLight() const { return m_pLightAmbient; }
	RpLight *getMainLight() const { return m_pMainLight; }
	//@} Jaewon
	RtAnimAnimation *getAnimation() const { return m_pAnim; }
	void setNearClip(float fNearClip) { m_fNearClip = fNearClip; }
	void setFarClip(float fFarClip) { m_fFarClip = fFarClip; }
	void setFOV(float fFOV) { m_fFOV = fFOV; }
	RwReal getCurrentViewWindow() const { return m_fCurrentViewWindow; }

	//@{ Jaewon 20041103
	const char *getStatsString();
	//@} Jaewon

	//@{ Jaewon 20041201
	// create a texture atlas.
	unsigned int mergeTextures(unsigned int width, unsigned int height);
	//@} Jaewon

	//@{ Jaewon 20041214
	// for testing ambient occlusion map modules
	//@{ Jaewon 20050105
	// uvOnly parameter added.
	//@{ Jaewon 20050112
	// AmbOcclMapSizeScale parameter added.
	enum AmbOcclMapSizeScale { AOMSS_QUATER=0, AOMSS_HALF, AOMSS_ONE, AOMSS_DOUBLE, AOMSS_QUADRUPLE };
	bool createAmbOcclMaps(bool uvOnly = false, AmbOcclMapSizeScale scale = AOMSS_ONE);
	//@} Jaewon
	//@} Jaewon
	bool clearAmbOcclMaps();
	bool destroyAmbOcclMaps();
	bool computeAmbOcclMaps(RtAmbOcclMapIlluminateProgressCallBack progressCB = NULL);
	bool saveAmbOcclMaps(const char *pathname);
	bool createAmbOcclMapUVs();

	//@{ Jaewon 20050708
	// Load a background texture.
	bool loadBackTex(const char *pathname);
	RwTexture *getBackTex() const { return m_pBackTex; }
	//@} Jaewon

	RtAmbOcclMapLightingSession m_lightingSession;
	//@} Jaewon

	RwSphere m_clumpBoundingSphere;

	//@{ Jaewon 20041102
	// model statistics
	Stats m_stats;
	//@} Jaewon

	//@{ Jaewon 20041202
	// path of the loaded clump
	char m_acPath[MAX_PATH];
	//@} Jaewon

	//@{ Jaewon 20050407
	// The user doesn't want to specify an animation.
	bool m_notWantAnim;
	//@} Jaewon
private:
	RpWorld *m_pWorld;

	RwCamera *m_pCamera;
	RwReal m_fNearClip;
	RwReal m_fFarClip;
	RwReal m_fFOV;
	RwReal m_fCurrentViewWindow;
	RwMatrix	m_matBackup;

	//@{ Jaewon 20050708
	// A background texture
	RwTexture *m_pBackTex;
	// The path of the loaded background texture
	char m_acBackTexPath[MAX_PATH];
	//@} Jaewon

	RpLight *m_pLightAmbient;

	RpLight *m_pMainLight;

	RpClump *m_pClump;
	RtAnimAnimation* m_pAnim;

	// get the hanim frame hierarchy of the clump.
	RpHAnimHierarchy *getHierarchy(RpClump *pClump) const;
	// initialize a newly loaded clump.
	void initClump(RpClump* pClump);
	// compute the bounding sphere of the clump.
	void computeClumpBoundingSphere();
};

#endif

