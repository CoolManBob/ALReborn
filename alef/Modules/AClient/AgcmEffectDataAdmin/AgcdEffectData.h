#ifndef __AGCD_EFFECT_DATA_H__
#define __AGCD_EFFECT_DATA_H__

#include <rwcore.h>

//#define D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM				50
//#define D_AGCD_USE_EEFECT_SET_DATA_SOUND_NAME_LENGTH	16
//#define D_AGCD_USE_EFFECT_SET_DATA_CUST_DATA_SIZE		32

#define D_AGCD_EFFECT_DATA_SCRIPT_CUST_DATA_ANIM_POINT	"AnimationPoint="

class AgcdUseEffectSetDataRotation
{
public:
	RwV3d	m_stRight;
	RwV3d	m_stUp;
	RwV3d	m_stAt;

	AgcdUseEffectSetDataRotation()
	{
		m_stRight.x = 1.0f;
		m_stRight.y = 0.0f;
		m_stRight.z = 0.0f;

		m_stUp.x = 0.0f;
		m_stUp.y = 1.0f;
		m_stUp.z = 0.0f;

		m_stAt.x = 0.0f;
		m_stAt.y = 0.0f;
		m_stAt.z = 1.0f;
	}
};

/*typedef struct
{
	CHAR			m_szSoundName[D_AGCD_USE_EEFECT_SET_DATA_SOUND_NAME_LENGTH];

	UINT32			m_ulEID;
	RwV3d			m_v3dOffset;
	FLOAT			m_fScale;
	INT32			m_lParentNodeID;
	UINT32			m_ulStartGap;

	AgcdUseEffectSetDataRotation	*m_pstRotation;

	// ex) "8(a1|b2|c0)"
	CHAR			m_szCustData[D_AGCD_USE_EFFECT_SET_DATA_CUST_DATA_SIZE];

	UINT32			m_ulConditionFlags;
//	UINT32			m_ulSSConditionFlags;
//	UINT32			m_ulStatusFlags;
} AgcdUseEffectSetData;*/

/*typedef struct
{
	AgcdUseEffectSetData	*m_astData[D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM];
	UINT32					m_ulConditionFlags;							// 검색용으로 쓰인다.(m_astData[]에 조건을 가진게 있는지?)

	UINT32					m_ulCustomFlags;

} AgcdUseEffectSet;*/

class AgcdUseEffectSetData
{
	enum { AGCD_USE_EFFECT_SET_DATA_MAX_LINK_EFFECT = 5 };

public:
	UINT32			m_ulIndex;
	INT32			m_lType;

	CHAR			*m_pszSoundName;

	INT32			m_lID;
	UINT32			m_ulEID;
	RwV3d			*m_pv3dOffset;
	FLOAT			m_fScale;
	INT32			m_lParentNodeID;
	UINT32			m_ulStartGap;

	RwRGBA			m_rgbScale;
	FLOAT			m_fParticleNumScale;

	BOOL			m_bAtomicEmitter;
	BOOL			m_bClumpEmitter;

	AgcdUseEffectSetDataRotation	*m_pcsRotation;

	// ex) "8(a1|b2|c0)"
	CHAR			*m_pszCustData;

	UINT32			m_ulConditionFlags;
	UINT32			m_ulCustomFlags;

	// 2005.08.22. steeple
	INT32			m_lTargetCID;

	// 2007.09.13. steeple
	INT32			m_alSubEffect[AGCD_USE_EFFECT_SET_DATA_MAX_LINK_EFFECT];

	AgcdUseEffectSetData()
	{
		m_ulIndex			= 0;
		m_lType				= 0;

		m_pszSoundName		= NULL;

		m_lID				= 0;
		m_ulEID				= 0;
		m_pv3dOffset		= NULL;
		m_fScale			= 1.0f;
		m_lParentNodeID		= 0;
		m_ulStartGap		= 0;

		m_pcsRotation		= NULL;

		m_pszCustData		= NULL;

		m_ulConditionFlags	= 0;

		m_rgbScale.red		= 255;
		m_rgbScale.green	= 255;
		m_rgbScale.blue		= 255;
		m_rgbScale.alpha	= 255;
		m_fParticleNumScale	= 1.0f;

		m_bAtomicEmitter	= FALSE;
		m_bClumpEmitter		= FALSE;

		m_lTargetCID		= 0;

		ZeroMemory(m_alSubEffect, sizeof(m_alSubEffect));
	}
private:
	AgcdUseEffectSetData(AgcdUseEffectSetData *pcsData)
	{
		//@{ kday 20050325
		ASSERT( !"NOTAVAILABLE" );
		//@} kday
		memcpy(this, pcsData, sizeof(this));
	}
};

class AgcdUseEffectSetList
{
public:
	AgcdUseEffectSetData	m_csData;
	AgcdUseEffectSetList	*m_pcsNext;

	AgcdUseEffectSetList()
	{
		m_pcsNext	= NULL;
	}
};

class AgcdUseEffectSet
{
public:

	AgcdUseEffectSetList	*m_pcsHead;

	UINT32					m_ulConditionFlags;	// 검색용으로 쓰인다.
	UINT32					m_ulCustomFlags;	// 사용자정의

	//. 2005. 09. 09 Nonstopdj
	//. INI에서 읽어온 Type을 저장.
	//. ModelTool에서 CopyCharacter에서 사용.
	INT						m_iOriginType;		

	AgcdUseEffectSet()
	{
		m_pcsHead			= NULL;
		m_ulConditionFlags	= 0;
		m_ulCustomFlags		= 0;
		m_iOriginType		= 0;
	}
};

#endif // __AGCD_EFFECT_DATA_H__