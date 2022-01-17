#ifndef __ACDEFINE_H__
#define __ACDEFINE_H__

#include <windows.h>
#include "apdefine.h"
#include <rwcore.h>
#include <rpworld.h>

typedef enum _eAcReturnType
{
	E_AC_RETURN_TYPE_NONE = -1,
	E_AC_RETURN_TYPE_FAILURE,
	E_AC_RETURN_TYPE_SUCCESS,
	E_AC_RETURN_TYPE_SKIP,
	E_AC_RETURN_TYPE_START,
	E_AC_RETURN_TYPE_END,
	E_AC_RETURN_TYPE_TRUE,
	E_AC_RETURN_TYPE_FALSE,
	E_AC_MAX_RETURN_TYPE
} eAcReturnType;

typedef PVOID (*AcCallbackData1)(PVOID pvData1);
typedef PVOID (*AcCallbackData2)(PVOID pvData1, PVOID pvData2);
typedef PVOID (*AcCallbackData3)(PVOID pvData1, PVOID pvData2, PVOID pvData3);
typedef PVOID (*AcCallbackData4)(PVOID pvData1, PVOID pvData2, PVOID pvData3, PVOID pvData4);

///////////////////////////////////////////////////////////////////////////////
// Export data header(prefix) - (AC_EXPORT_HD)_(DIRECTORY)_(OBJECT)
// clump
#define AC_EXPORT_HD_CHAR_CHARACTER			"A"
#define AC_EXPORT_HD_CHAR_DEF_ARMOUR		"B"
#define AC_EXPORT_HD_CHAR_CHAR_PICKING_DATA	"C"
#define AC_EXPORT_HD_CHAR_BASE_ITEM			"D"
#define AC_EXPORT_HD_CHAR_SECOND_ITEM		"E"
#define AC_EXPORT_HD_CHAR_FILED_ITEM		"F"
#define AC_EXPORT_HD_CHAR_ITEM_PICKING_DATA	"G"

#define AC_EXPORT_HD_OBJ_OBJECT				"H"
#define AC_EXPORT_HD_OBJ_COLLISION_DATA		"I"
#define AC_EXPORT_HD_OBJ_PICKING_DATA		"J"

#define AC_EXPORT_HD_EFF_CLUMP_BASE			"K"

// animation
#define AC_EXPORT_HD_CHAR_ANIM_ANIMATION	"A"
#define AC_EXPORT_HD_CHAR_ANIM_BLEND_ANIM	"B"
#define AC_EXPORT_HD_CHAR_ANIM_SUB_ANIM		"C"
#define AC_EXPORT_HD_CHAR_ANIM_SKILL_ANIM	"D"

#define AC_EXPORT_HD_OBJ_ANIM_ANIMATION		"E"

#define AC_EXPORT_HD_EFF_ANIM_ANIMATION		"F"
#define AC_EXPORT_HD_EFF_ANIM_SPLINE		"G"

// texture
#define AC_EXPORT_HD_TEX_ITEM_ITEM			"L"
#define AC_EXPORT_HD_TEX_ITEM_SMALL			"M"
#define AC_EXPORT_HD_TEX_ITEM_DUR_ZERO		"N"
#define AC_EXPORT_HD_TEX_ITEM_DUR_5_UNDER	"O"

#define AC_EXPORT_HD_TEX_SKILL_SKILL		"P"
#define AC_EXPORT_HD_TEX_SKILL_SMALL		"Q"
#define AC_EXPORT_HD_TEX_SKILL_UNABLE		"R"

#define AC_EXPORT_HD_TEX_EFFECT_EFFECT		"S"

#define AC_EXPORT_HD_TEX_UI_UI				"T"
#define AC_EXPORT_HD_TEX_UI_CONTROL			"U"

//customize
#define AC_EXPORT_HD_CHAR_CUSTOMIZE			"V"
///////////////////////////////////////////////////////////////////////////////

#define AC_EXPORT_EXT_CLUMP					"ECL"
#define AC_EXPORT_EXT_ANIMATION				"EAN"
#define AC_EXPORT_EXT_SPLINE				"ESP"
#define AC_EXPORT_EXT_TEXTURE_DDS			"DDS"

#define AC_EXPORT_ID_LENGTH					7

#define AC_EXPORT_TRACE_FILE				"ERROR_EXPORT_RESOURCE.TXT"

class AcClientData
{
protected:
	BOOL	m_bExport;

public:
	AcClientData()
	{
		m_bExport	= FALSE;
	}

	inline VOID	SetExport(BOOL bExport)	{m_bExport	= bExport;}
	inline BOOL IsExport()	{return m_bExport;}
};

typedef struct _AgcdLOD AgcdLOD;

class AgcdLODData
{
public:
	INT32		m_lIndex;

	INT32		m_lHasBillNum;
	INT32		m_alBillInfo[AGPDLOD_MAX_NUM];
	UINT32		m_ulMaxLODLevel;
	UINT32		m_aulLODDistance[AGPDLOD_MAX_NUM];

	UINT32		m_ulBoundary;

	UINT32		m_ulMaxDistanceRatio;

	AgcdLOD		*m_pstAgcdLOD;

	AgcdLODData()
	{
		m_lIndex						= 0;
		m_lHasBillNum					= 0;
		m_ulMaxLODLevel					= 0;
		m_ulBoundary					= 0;
		m_ulMaxDistanceRatio			= 0;
		m_pstAgcdLOD					= NULL;

		for(INT32 lCount = 0; lCount < AGPDLOD_MAX_NUM; ++lCount)
		{
			m_alBillInfo[lCount]		= 0;
			m_aulLODDistance[lCount]	= 0;
		}
	}
};

typedef struct _AgcdLODList
{
	AgcdLODData		m_csData;
	_AgcdLODList	*m_pstNext;
} AgcdLODList;

typedef struct _AgcdLOD
{
	INT32		m_lNum;
	INT32		m_lDistanceType;
	AgcdLODList	*m_pstList;
} AgcdLOD;

#define ARGB32_TO_DWORD(a,r,g,b)	((r & 0xff) + ((g & 0xff) << 8) + ((b & 0xff) << 16) + ((a & 0xff) << 24))
#define ARGB_TO_ABGR(color)	( ((color << 8) >> 24) | ((color << 24) >> 8) | ((color & 0xff00ff00)) )

#define		MY2D_VERTEX_FLAG (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define		SIZE_MY2D_VERTEX				28
typedef struct My2DVertex {
	float x, y, z, rhw;
	DWORD color;
	float u,v;
}My2DVertex;

//----> kday 20040916
enum		// Effect Process Type 
{
	AGCMEFF2_PROCESS_TYPE_NONE				= 0	,
	AGCMEFF2_PROCESS_TYPE_EFFECT_START		,// Effect 시작
	AGCMEFF2_PROCESS_TYPE_EFFECT_END		,// Effect 종료  
	AGCMEFF2_PROCESS_TYPE_EFFECT_MISSLEEND	,// 발사체가 도달! - Effect 가 끝난것은 아님 
};
enum		// Effect Process Type 
{
	AGCMEFFECT_EFFECT_PROCESS_TYPE_NONE				= 0	,
	AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_START			,// Effect 시작
	AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_END			,// Effect 종료  
	AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_MISSLEEND		 // 발사체가 도달! - Effect 가 끝난것은 아님 
};

enum
{
	RWFLAG_RENDER_ADD		= 0x01,
	RWFLAG_DONOT_CULL		= 0x02
};
///////////////////////////////////////////////////////////////////////////////
// struct
///////////////////////////////////////////////////////////////////////////////
struct stAgcmEffectNoticeEffectProcessData
{
	INT32	lEffectProcessType;
	INT32	lOwnerCID;
	INT32	lTargetCID;
	BOOL	bMissile;//안쓰이는듯.
	INT32	lCustData;
	INT32	lCustID;

	stAgcmEffectNoticeEffectProcessData()
	: lEffectProcessType(AGCMEFF2_PROCESS_TYPE_NONE)
	, lOwnerCID(0)
	, lTargetCID(0)
	, bMissile(FALSE)
	, lCustData(0)
	, lCustID(0)
	{
	};
};
typedef stAgcmEffectNoticeEffectProcessData STEFF_NOTICE_PROCESSDATA, 
											*PSTEFF_NOTICE_PROCESSDATA, 
											*LPSTEFF_NOTICE_PROCESSDATA;
///////////////////////////////////////////////////////////////////////////////
// fptr
///////////////////////////////////////////////////////////////////////////////
// CALLBACK FUNCTION
typedef BOOL (*AgcmEffectNoticeEffectProcessCB ) ( stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass );
//<----
 

static inline DWORD RwRGBAToDWORD(RwRGBA rgba)
{
	return DEF_ARGB32(rgba.alpha, rgba.red, rgba.green, rgba.blue);
}

#endif // __ACDEFINE_H__