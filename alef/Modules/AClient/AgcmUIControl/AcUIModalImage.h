#pragma once

#include "AgcEngine.h"
#include "AgcmSound.h"
#include "AgcmFont.h"
#include "AcUIButton.h"
#include "AcUIEdit.h"

enum 
{
	UICM_MODALIMAGE_MAX_MESSAGE						= UICM_BASE_MAX_MESSAGE
};

class AcUIModalImage : public AgcModalWindow
{
public:
	AcUIModalImage( void );
	virtual ~AcUIModalImage( void );

	// AcUIImage 용.
	virtual void			OnAddImage				( RwTexture* pTexture );

	// Parent Override.
	virtual	void 			OnPostRender			( RwRaster *raster );	// 월드 렌더한 후.
	virtual void 			OnWindowRender			( void );
	virtual	void 			OnClose					( void );				// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
	virtual	BOOL 			OnLButtonDown			( RsMouseStatus *ms	);
	virtual	BOOL 			OnMouseMove				( RsMouseStatus *ms	);

	INT32					AddImage				( char * filename );
	RwTexture*				GetImage				( INT32 index );
	INT32					AddOnlyThisImage		( char* filename );		// List에 모든 이미지를 지우고 넘어온 이미지만 담는다. 
	void					DestroyTextureList		( void );

public:
	static AgcmSound*		m_pAgcmSound;
	static AgcmFont*		m_pAgcmFont;

	BOOL					m_bPassPostMessage;							// Post Message를 Parent에게 넘길것인가?

protected:
	AuList< RwTexture* >							m_listTexture;
};
