// AgcModule.h:
// 클라이언트용 모듈 클래스.
// 작성 : 정재욱
// 일자 : 2002/04/04
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMODULE_H__FD36A322_47D9_11D6_9402_000000008510__INCLUDED_)
#define AFX_AGCMODULE_H__FD36A322_47D9_11D6_9402_000000008510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment ( lib , "imm32" )		// IME 라이브러리 삽입
#pragma comment ( lib , "Winmm" )		// 마고자 (2004-01-06 오후 6:40:53) : 멀미 라이브러리. timeGettime 땜에 삽입.

#include "ApModule.h"
#include "rwcore.h"
#include "RpWorld.h"
#include "AuLua.h"

extern "C" {
	typedef struct 
	{
		const char *name;
		int (*func)(lua_State *);
	} luaDef;
}

// 클라이언트엔진은 ApcEngine클래스를 중심으로하여 여러 모듈을 연결시켜서 이루어진다. 
// 모듈은 4가지 타입( 데이타,UI,FullUI,Network )을 가지며 독립적으로 작동한다.
enum	CAMERASTATECHANGETYPE
{
	CSC_INIT = 1,
	CSC_RESIZE,
	CSC_NEAR,
	CSC_FAR,
	CSC_ACTIVATE
};

struct RsKeyStatus;
struct RsMouseStatus;
class AgcModule : public ApModule  
{
public:
	// 클라이언트 모듈 타입의 정의.
	enum	MODULETYPE
	{
		DATAMODULE		= 0x01,
		WINDOWUIMODULE	= 0x02,
		FULLUIMODULE	= 0x04,
		NETWORKMODULE	= 0x08
	};

public:
	AgcModule();
	virtual ~AgcModule();

	virtual	void OnCameraStateChange(CAMERASTATECHANGETYPE ChangeType)	{		}
	virtual	void OnLuaInitialize( AuLua* pLua )							{		}
	virtual	void OnTerminate()											{		}	// OnClose는 UI 모듈용이고 , Terminate는 기타 모듈 용. 종료 처리이다.

	void		SetType( UINT32 type )		{	m_uType	= type;			}
	BOOL		IsRegistered()				{	return m_bRegistered;	}

	VOID		LockRender();
	VOID		UnlockRender();

	VOID		LockFrame();
	VOID		UnlockFrame();

	RpWorld*	GetWorld();
	RwCamera*	GetCamera();
	RpLight*	GetAmbientLight();
	RpLight*	GetDirectionalLight();

public:
	UINT32		m_uType;		// 모듈 타입을 저장 비트 플래그를 써서 여러개의 타입을 동시에 가질수 있음
	UINT32		m_uPriority;	// 창UI모듈 전용 , 중요도를설정
	BOOL		m_bRegistered;	// 등록된 모듈인지 점검.
};

#endif // !defined(AFX_AGCMODULE_H__FD36A322_47D9_11D6_9402_000000008510__INCLUDED_)
