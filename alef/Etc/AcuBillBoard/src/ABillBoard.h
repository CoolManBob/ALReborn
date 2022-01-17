#ifndef _ABILLBOARD_H_
#define _ABILLBOARD_H_

#include "rwcore.h"
#include "rpworld.h"
#include "skeleton.h"

enum E_TFormat
{
	TFORMAT_BMP, 
	TFORMAT_RAS, 
	TFORMAT_PNG, 
	TFORMAT_TIF
};

class AcuBillBoard
{
public:
	
	RwChar		m_szTPath[256];		//Texture의 Path				
	E_TFormat	m_eTFormat;			//Texture의 포맷				
	RwChar		m_szTName[256];		//Texture의 이름(확장자제외)
	RwV3d		m_v3dOriginalPos;	//빌보드의 생성 위치 
	RwReal		m_TexCoords_U;		//u좌표
	RwReal		m_TexCoords_V;		//v좌표
	

public:
	AcuBillBoard();				//생성자 
	~AcuBillBoard();			//소멸자
	RpAtomic*	CreateBBAtomic( );					//한번 거칠때마다 m_nUseBB가 하나씩 증가한다. 
	RpAtomic*	CreateDoubleSideBoardAtomic();		//양면 플레인 만들때 쓰임 
	RwBool		RenderIdle( RpAtomic* Atomic, RwCamera* Camera, RwReal fRate1 = 1.0f, RwReal fRate2 = 1.0f );	//카메라 방향에 맞게 빌보드를 돌려주는 일을 한다. 
	
	void		SetTexturePath( RwChar sz_SetPath[30] );
	void		SetTextureName( RwChar sz_SetTName[30] );

};



#endif