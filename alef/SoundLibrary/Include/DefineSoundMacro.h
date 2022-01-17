#ifndef __DEFINE_SOUND_MACRO_H__
#define __DEFINE_SOUND_MACRO_H__



#include <string>
#include "rwcore.h"
#include "fmod.hpp"



#pragma comment( lib, "fmodex_vc.lib" )



// 동시출력 가능한 사운드채널 수
#define MAX_SOUND_CHANNELS											200

// 비정상적인 사운드 인덱스
#define INVALID_SOUND_INDEX											0

// 비정상적인 사운드 채널
#define INVALID_SOUND_CHANNEL										-1

// 기본 벡터 사이즈
#define INITIAL_VECTOR_SIZE											100

// 벡터초과시 증가량
#define INCREASE_VECTOR_SIZE										20

// 도플러효과 비율
#define DOPPLER_SCALE												1.0

// 거리비율
#define DISTANCE_FACTOR												1.0

// 음향소실 비율
#define ROLLOFF_SCALE												0.5

// 사운드 파일명 최대 길이
#define SOUND_STRING_LENGTH_FILENAME								256



// 사운드 종류
enum SOUND_TYPE
{
	SOUND_TYPE_INVALID = 0,
	SOUND_TYPE_2D_SOUND,
	SOUND_TYPE_2D_SOUND_LOOPED,
	SOUND_TYPE_3D_SOUND,
	SOUND_TYPE_3D_SOUND_LOOPED,
};


#define IDS_SOUND_PATH												"Sound\\"
#define IDS_SOUND_PATH_BGM											"Sound\\BGM\\"
#define IDS_SOUND_PATH_EFFECT										"Sound\\Effect\\"
#define IDS_SOUND_PATH_UI											"Sound\\UI\\"

#define SCALE_FACTOR_P_TO_R											100.0f
#define SCALE_FACTOR_R_TO_P											0.01f

// FMOD_VECTOR <-> SoundVector
#define VECTOR3_SET_FMODVECTOR( FV, V3 )							{ FV.x = V3.x; FV.y = V3.y; FV.z = V3.z; }

#define SoundVector													RwV3d
#define SoundMatrix													RwMatrix
#define SoundNode													RwFrame

const static RwV3d vZeroVector										= { 0.0f, 0.0f, 0.0f };


#endif