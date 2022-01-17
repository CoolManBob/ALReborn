//@{ Jaewon 20040730
// created.
// Image-space post-processing effects module.
//@} Jaewon

#ifndef __AGCMPOSTFX_H__
#define __AGCMPOSTFX_H__

#include "AgcModule.h"
#include <vector>
#include <d3dx9effect.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmPostFXD" )
#else
#pragma comment ( lib , "AgcmPostFX" )
#endif
#endif

#define NUM_PARAMS 2

//@{ Jaewon 20050401
class AgcmRender;
//@} Jaewon

class AgcmPostFX : public AgcModule
{
protected:
	// a struct that encapsulates aspects of a render target post fx
	struct Technique
	{
		// technique name
		char m_name[64];
		// post fx technique handle
		D3DXHANDLE m_hTechnique;
		// handle to the post-fx source texture
//		D3DXHANDLE m_hTexSource;
		// names of changeable parameters
		char m_szParamName[NUM_PARAMS][MAX_PATH];
		// handles to the changeable parameters
		D3DXHANDLE m_hParam[NUM_PARAMS];
		// size of the parameter.
		// indicates how many components of float4 are used.
		int m_nParamSize[NUM_PARAMS]; 

		// parameter default
		D3DXVECTOR4 m_vParamDef[NUM_PARAMS];

		//@{ Jaewon 20041011
		// this technique does a dependent texture read?
		D3DXHANDLE m_bDepedentRead;
		//@} Jaewon

		Technique();
		~Technique();
		void init(LPD3DXEFFECT pFX, D3DXHANDLE hTechnique);
	};

	// a struct that represents an instance of 
	// a post fx to be applied to the scene
	struct TechniqueInstance
	{
		D3DXVECTOR4 m_vParam[NUM_PARAMS];
		int m_nFxIndex;

		TechniqueInstance() : m_nFxIndex(-1)
		{
			memset(m_vParam, 0, sizeof(m_vParam));
		}
	};

	struct RenderTargetChain
	{
		int m_nNext;
		//bool m_bFirstRender;
		//bool m_bLastRender;
		RwRaster* m_pRenderTarget[2];

		RenderTargetChain();
		~RenderTargetChain();

		bool init(int width, int height);
		void flip();
		
		RwRaster* target() const;
		RwRaster* source() const;
	};

	struct AutoExposureControl
	{
		// automatic exposure control on/off
		BOOL m_bAutoExposureControl;
		// current & target exposure level
		float m_fCurrentExposureLevel;
		float m_fTargetExposureLevel;
		// interval between successive target exposure updates in seconds
		float m_fTargetUpdateInterval;
		// a feedback coefficient for exposure control
		float m_fFeedbackK;

		AutoExposureControl()
			: m_bAutoExposureControl(TRUE), m_fCurrentExposureLevel(2.0f),
			m_fTargetExposureLevel(2.0f), m_fTargetUpdateInterval(0.1f),
			m_fFeedbackK(3.0f)
		{}
	};

public:
	AgcmPostFX();
	virtual ~AgcmPostFX();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();

	void OnCameraStateChange(CAMERASTATECHANGETYPE ctype);

	// on/off
	void On();
	void Off(bool doNotDestroyTargets = false);
	BOOL isOn() const { return m_bOn; }

	BOOL isAutoExposureControlOn() { return m_autoExposure.m_bAutoExposureControl; }
	void AutoExposureControlOn() { m_autoExposure.m_bAutoExposureControl = TRUE; }
	void AutoExposureControlOff();

	// set a active post fx pipeline(sequence of technique instances).
	BOOL setPipeline(const char* pipeline, BOOL bCheck = TRUE);
	// check whether the specified pipeline is valid
	// on the current hardware.
	BOOL isPipelineValid(const char* pipeline) const;
	//@{ Jaewon 20041011
	// get the current pipeline sequence.
	// return FALSE if there is no active pipeline.
	BOOL getPipeline(char* pipeline, int nMax) const;
	//@} Jaewon

	// apply post fx chains.
	void render();

	// change the main camera's render target to the offscreen buffer.
	void changeRenderTarget();

	// d3d stuff
	void onLostDevice();
	void onResetDevice();

	//@{ Jaewon 20040921
	// to control radius & center of circle based effects.
	void setRadius(float radius);
	void setCenter(float u, float v);
	//@} Jaewon

	//@{ Jaewon 20041122
	// in order to save video memory when post fx is off
	void createRenderTargets();
	void destroyRenderTargets();
	bool isReady() const { return m_pSceneRenderTarget != NULL; }
	//@} Jaewon

	//@{ Jaewon 20050223
	// Save the back buffer of the next frame to the render target chain source.
	void saveNextFrame()
	{ m_bSaveNextFrame = true; }
	//@} Jaewon
	//@{ Jaewon 20050329
	// Save the current content of the back buffer.
	void saveImmediateFrame();
	//@} Jaewon

	//@{ Jaewon 20050915
	RwRaster *getSavedFrame() const
	{ return m_pTransitionSource; }
	//@} Jaewon

	// technique objects for post fx
	std::vector<Technique*> m_technics;

	// currently active sequence of technique instances
	std::vector<TechniqueInstance*> m_pipeline;

	// effects
	LPD3DXEFFECT m_pd3dEffect;
protected:
	// camera raster for post-processing
	RwRaster *m_pSceneRenderTarget;
	RwRaster *m_pOriginalRenderTarget;

	// render target chain
	RenderTargetChain m_RTChain;
	
	// PostFX module on/off
	BOOL m_bOn;

	// all hardware requirements for the fx are satisfied?
	BOOL m_bValid;

	// handle to the saved scene texture
	D3DXHANDLE m_hTexScene;
	//@{ Jaewon 20040921
	// handle to the previous result
	D3DXHANDLE m_hTexPrev;
	// list of resource loaded textures
	std::vector<LPDIRECT3DBASETEXTURE9> m_textureLoaded;
	//@} Jaewon
	// handle to the window size parameter
	D3DXHANDLE m_hWindowSize;
	// handle to the elapsed time in seconds
	D3DXHANDLE m_hElapsedTime;

	// vertex declaration for post fx
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecl;
	// vertex buffer for post fx
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;

	AutoExposureControl m_autoExposure;
	// 64x64, 16x16, 4x4, 1x1 render targets for scene averaging
	RwRaster *m_pAverageRenderTarget[4];
	// pointer to the averaging technique
	Technique *m_pAveragingTechnic;
	// pointer to the exposure control technique
	Technique *m_pExposureControlTechnic;
	// render targets for the luminance feedback control
	RenderTargetChain m_lumFeedbackRTChain;
	// handle to the current exposure texture
	D3DXHANDLE m_hTexCurrentExposure;

	//@{ Jaewon 20040921
	// radius & center for circle based effects
	D3DXHANDLE m_hRadius;
	D3DXHANDLE m_hCenter;
	//@} Jaewon

	//@{ Jaewon 20041011
	// for dependent reads on ps 1.1 ~ 1.3
	RwRaster *m_pSquareRenderTarget;
	D3DXHANDLE m_hTexSquare;
	D3DXHANDLE m_hSquareRatio;
	//@} Jaewon

	//@{ Jaewon 20041014
	// to prevent z-related bugs of some graphics card in low-res window mode.
	RwRaster *m_pZBuffer;
	RwRaster *m_pOriginalZBuffer;
	//@} Jaewon

	//@{ Jaewon 20041122
	// in order to save video memory when post fx is off
	bool m_bDoNotCreate;
	//@} Jaewon

	//@{ Jaewon 20050223
	// Save the back buffer of the next frame to the render target chain source.
	bool m_bSaveNextFrame;
	// Handle to the transition source texture
	D3DXHANDLE m_hTexTransSource;
	//@} Jaewon

	//@{ Jaewon 20050304
	// Source raster for transition fx
	RwRaster *m_pTransitionSource;
	//@} Jaewon

	//@{ Jaewon 20050401
	AgcmRender *m_pAgcmRender;
	//@} Jaewon

	// apply the feedback loop between current & target luminances.
	void controlExposure();
	// get the 1x1 average image of the original scene.
	void averageScene();
};

#endif