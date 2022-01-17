#ifndef __AGCDUSEEFFECTSET_H__
#define __AGCDUSEEFFECTSET_H__

#include <rwcore.h>

typedef struct AgcdUseEffectSet
{
	UINT16		m_unEID;
	RwV3d		m_v3dOffset;
	FLOAT		m_fScale;
	UINT16		m_unParentNodeID;
	UINT32		m_ulStartGap;

	RwFrame		*m_pstTargetFrame;
} AgcdUseEffectSet;

#endif // __AGCDUSEEFFECTSET_H__