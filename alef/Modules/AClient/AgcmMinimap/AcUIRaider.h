#pragma once

#include "AcUIMinimap.h"

#define	ACUIRAIDER_TEXTURE_SIZE		128
#define ACUIRAIDER_RANGE_SIZE		384
#define ACUIRAIDER_POINT_SIZE		8

 
enum
{
	POINT_TEX_RED		,
	POINT_TEX_BLUE		,
	POINT_TEX_WHITE		,
	POINT_TEX_PURPLE	,
	POINT_TEX_GREEN		,

	POINT_TEX_COUNT		,
};

class AgpdCharacter;

class AcUIRaider	
	: public	AcUIMinimap
{
public:
	AcUIRaider										( VOID );
	virtual ~AcUIRaider								( VOID );

	virtual BOOL			OnInit					( VOID );
	virtual VOID			OnClose					( VOID );
	virtual VOID			OnWindowRender			( VOID );
	virtual BOOL			OnIdle					( UINT32 ulClockCount );

	virtual	BOOL			OnLButtonDown			( RsMouseStatus *ms	);
	virtual	BOOL			OnLButtonUp				( RsMouseStatus *ms	);
	virtual BOOL			OnMouseMove				( RsMouseStatus *ms );
	virtual	BOOL			OnRButtonUp				( RsMouseStatus *ms	);


	VOID					RaiderPositionUpdate	( VOID );

protected:
	VOID					_RenderRaider			( VOID );
	VOID					_RenderMap				( VOID );
	VOID					_RenderCharacter		( VOID );
	BOOL					_RenderCameraCreate		( VOID );
	BOOL					_RenderTextureCreate	( VOID );
	RwTexture*				_TargetRenderTexture	( INT32 nDivisionIdnex , INT32 nPartIndex );

	INT32					_CharacterType			( AgpdCharacter* pAgpdCharacter );

	VOID					_PointTextureLoad		( VOID );

private:

	RwTexture*				m_pArrPointTexture[ POINT_TEX_COUNT ];

	RwCamera*				m_pRaiderCamera;
	RwTexture*				m_pRaiderTexture;
	RwTexture*				m_pMaskTex;

};
