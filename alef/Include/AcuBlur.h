// AcuBlur.h: interface for the AcuBlur class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUBLUR_H__4BBE18F4_5EE3_4804_B268_57226644B401__INCLUDED_)
#define AFX_ACUBLUR_H__4BBE18F4_5EE3_4804_B268_57226644B401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "rwcore.h"
#include "rpworld.h"

#define NUM_CAMERA_TEXTURES	2

typedef struct
{
	RwReal x;
	RwReal y;
	RwReal z;
	RwReal u;
	RwReal v;
} AcuBlurRenderedVertexFormat;

class AcuBlur  
{
private:
	void *		BlurVertexShader;
	void *		BlurPixelShader;

	AcuBlurRenderedVertexFormat RenderedVertex[4];

	RpClump  *	Clump;

	RwCamera *	TexCamera[NUM_CAMERA_TEXTURES];
	RwTexture *	CameraTexture[NUM_CAMERA_TEXTURES];

	RwInt32		BlurPasses;
	RwInt32		BlurType;

public:
	AcuBlur();
	virtual ~AcuBlur();

	RwCamera *	CreateTextureCamera(RwBool wantZRaster);
	RwBool		PShaderOpen(void);
	void		BoxFilterSetup();
	void		VBoxFilterSetup();
	void		HBoxFilterSetup();
	void		Blur(int srcTexture);
	void		MakeBlur(RwRaster *pstRaster);
	void		PShaderClose(void);

};

#endif // !defined(AFX_ACUBLUR_H__4BBE18F4_5EE3_4804_B268_57226644B401__INCLUDED_)
