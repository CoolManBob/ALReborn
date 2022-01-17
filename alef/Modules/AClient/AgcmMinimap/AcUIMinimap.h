// AcUIMinimap.h: interface for the AcUIMinimap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUIMINIMAP_H__BB23B70C_035A_4A27_AD02_ADE2A23C00F3__INCLUDED_)
#define AFX_ACUIMINIMAP_H__BB23B70C_035A_4A27_AD02_ADE2A23C00F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcUIBase.h"

class AgcmMinimap;
class AgpmCharacter;

#define	ACUIMINIMAP_TEXTURE_SIZE	192
#define ACUIMINIMAP_NEXT_DURATION	60

class AcUIMinimap : public AcUIBase  
{
public:
	AcUIMinimap();
	virtual ~AcUIMinimap();

	INT32				m_nXPos				;	// 좌표를 픽셀값으로 가지고 있음..
	INT32				m_nZPos				;

	// 방향 빙빙..
	UINT32				m_uLastChangeTime	;
	FLOAT				m_fCurrentTurnY		;

	AgcmMinimap	*		m_pAgcmMinimap		;
	AgpmCharacter *		m_pAgpmCharacter	;

	// 텍스쳐 정보..
	INT32				m_nDivisionIndex	;
	INT32				m_nPartIndex		;

	FLOAT				m_fStartX			;
	FLOAT				m_fStartZ			;
	FLOAT				m_fWidth			;

	FLOAT				m_fDuration			;
	RsMouseStatus		m_prevMouseState	;

public:

	// 모듈 포인터 지정해둠.
	void	SetMinimapModule( AgcmMinimap * pMinimapModule ){ m_pAgcmMinimap= pMinimapModule;}
	void	SetCharacterModule( AgpmCharacter * pCharacterModule) { m_pAgpmCharacter= pCharacterModule;}
	void	PositionUpdate( VOID );

	// 오퍼레이션

	BOOL		RenderMinimap		();
	RwTexture *	RenderDivision		( INT32 nDivisionIdnex , INT32 nPartIndex );
	// 렌더한다.

	// 메시지처리.
	virtual BOOL OnInit			()	;
	virtual	void OnWindowRender	()	;
	virtual void OnClose		()	;

	virtual	BOOL OnMouseMove	( RsMouseStatus *ms	);
	virtual	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	virtual	BOOL OnRButtonUp	( RsMouseStatus *ms	);
};

#endif // !defined(AFX_ACUIMINIMAP_H__BB23B70C_035A_4A27_AD02_ADE2A23C00F3__INCLUDED_)
