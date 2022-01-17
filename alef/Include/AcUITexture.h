#ifndef		_ACUITEXTURE_H_
#define		_ACUITEXTURE_H_

#include "AcUIBase.h"

//외부에 있는 Texture Pointer를 받아서 Render만 해준다. ( Texture 생성, 소멸등은 관리하지 않는다. )

enum 
{
	UICM_TEXTURE_MAX_MESSAGE						= UICM_BASE_MAX_MESSAGE
};

class AcUITexture : public AcUIBase
{
public:
	AcUITexture( void );
	virtual ~AcUITexture( void );

protected:
	RwTexture*										m_pTexture;		// 이 값이 NULL이면 그리지 않는다.
	UINT8											m_cAlphaValue;	// Im2D Render시 Alpha Value
	DWORD											m_lColorValue;	// Im2D Render시 Color Value 

public:
	void				SetTexture					( RwTexture* pTexture, UINT32 lValidWidth = 0, UINT32 lValidHeight = 0 );
	void				SetAlphaValue				( UINT8 cAlphaValue );
	void				SetColorValue				( DWORD lColorValue );
	RwTexture*			GetTexturePointer			( void );
		
//virtual fuction
public:
	virtual void		OnWindowRender				( void );
};

#endif //_ACUITEXTURE_H_