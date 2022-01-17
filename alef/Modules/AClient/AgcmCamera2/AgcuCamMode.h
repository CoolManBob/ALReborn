// AgcuCamMode.h
// -----------------------------------------------------------------------------
//                             _____                 __  __           _          _     
//     /\                     / ____|               |  \/  |         | |        | |    
//    /  \    __ _  ___ _   _| |      __ _ _ __ ___ | \  / | ___   __| | ___    | |__  
//   / /\ \  / _` |/ __| | | | |     / _` | '_ ` _ \| |\/| |/ _ \ / _` |/ _ \   | '_ \ 
//  / ____ \| (_| | (__| |_| | |____| (_| | | | | | | |  | | (_) | (_| |  __/ _ | | | |
// /_/    \_\\__, |\___|\__,_|\_____|\__,_|_| |_| |_|_|  |_|\___/ \__,_|\___|(_)|_| |_|
//            __/ |                                                                    
//           |___/                                                                     
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 01/29/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUCAMMODE_20050129
#define _H_AGCUCAMMODE_20050129

#include "AgcuEffUtil.h"
#include "AgcmCamera2.h"
#include "AcuMath.h"
USING_ACUMATH;
// -----------------------------------------------------------------------------

class AgcuCamMode
{
public:
	enum eState
	{
		eState_Changing	= 0,
		eState_End		,

		eState_Num		,
	};

	enum eMode
	{
		eMode_Free		= 0,
		eMode_Top		,
		eMode_Quart		,

		eMode_1stPerson	,

		eMode_Num		,
	};

	struct stModeData
	{
		RwReal		m_fDftLen;		//cm
		RwReal		m_fSavLen;		//cm
		RwReal		m_fMinLen;		//cm
		RwReal		m_fMaxLen;		//cm
		RwReal		m_fDftPitch;	//deg
		RwReal		m_fSavPitch;	//deg
		RwReal		m_fMinPitch;	//deg
		RwReal		m_fMaxPitch;	//deg
		RwUInt32	m_ulLockFlag;	//flag
	};

private:
	// Construction/Destruction
	AgcuCamMode();
public:
	virtual	~AgcuCamMode();

	// Operators

	// Accessors

	// Interface methods
	static AgcuCamMode&	bGetInst();
	void				bChageMode(AgcmCamera2& cam2, eMode mode);
	const stModeData&	bGetCurrModeData(void);
	eMode				bGetCurrMode(void) { return m_currmode; }
	void				bOnIdle(AgcmCamera2& cam2, float fElapsed);
	BOOL				ReadXMLData( CONST string& strFileName );

	void				ReloadCamSetting();

	stModeData			m_amode[eMode_Num];
protected:
	// Data members
	eState				m_state;
	eMode				m_currmode;

	RwV3d				m_vEyeSubAt;
	RwV3d				m_uvTrans;
	RwReal				m_lenTrans;

	AgcuAccmOnePlusSin	m_accumOnePlusSin;

	// Implementation methods

public:
	struct	CameraInfoSet
	{
		float	m_fCharacterHeight		;	// 케릭터 높이.. 카메라가 중심으로 보는곳.
		float	m_fCharacterFaceHeight	;	// 케릭터 얼굴 높이 ,.. 풀로 줌인 했을때 바라보는곳

		struct	CameraInfo
		{
			float	m_fMinLen				;	// 가장 당겼을때의 카메라 거리
			float	m_fMaxLen				;	// 가장 밀었을때의 카메라 거리
			float	m_fMinPitch				;	// 아래로 최대 내려가는 각도.. 평행이 0도 기준.. 일반적으로 -45도
			float	m_fMaxPitch				;	// 위로 최대로 올라간 각도.. ㅇ리반적으로 45도
		};

		INT32		nCharacterTemplateID		;	// 케릭터 템플릿 아이디. 0 이면 디폴트
		CameraInfo	m_stInfo[ eMode_Num ];
	};

	vector< CameraInfoSet >	m_vecCameraInfo;
	void	AddCameraInfo( CameraInfoSet * pSet );
	void	SetCameraInfo( AgcmCamera2 * pAgcmCamera2 , AgpdCharacter	*pstAgpdCharacter ); // 케릭터 포인터 입력
};

#endif // _H_AGCUCAMMODE_20050129
// -----------------------------------------------------------------------------
// AgcuCamMode.h - End of file
// -----------------------------------------------------------------------------

