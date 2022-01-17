#ifndef			_AGCCHARACTERUTIL_H_
#define			_AGCCHARACTERUTIL_H_
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rtcharse.h"
#include "rwplcore.h"
#include "rphanim.h"
#include "rpusrdat.h"

#define AGC_CHAR_UTIL_MAX_ATOMICS	20

class UtilSetUDAInt32Params
{
public:
	INT32	m_lCBCount;
	INT32	m_alUDAInt32[AGC_CHAR_UTIL_MAX_ATOMICS];
	CHAR	m_szUDAName[256];

	//@{ 2006/11/14 burumal
	CHAR	m_nValidCount;
	//@}

	UtilSetUDAInt32Params()
	{		
		Clear();
	}

	//@{ 2006/11/14 burumal
	VOID Clear()
	{
		m_lCBCount = 0;
		m_nValidCount = 0;
		memset(m_alUDAInt32, 0, sizeof(INT32) * AGC_CHAR_UTIL_MAX_ATOMICS);
		strcpy(m_szUDAName, "");
	}
	//@}
};

RpAtomic *UtilSetHierarchyForSkinAtomic(RpAtomic *atomic, void *data);
RpHAnimHierarchy *UtilGetHierarchy(RpClump *clump);
RwFrame *UtilGetChildFrameHierarchy(RwFrame *frame, void *data);
RpHAnimAnimation *UtilLoadAnimationFile(RwChar *file);
RwChar	*UtilPathnameCreate( const RwChar *srcBuffer );
RpAtomic *UtilGetBBox(RpAtomic *atomic, void *data);
void UtilClumpGetBBox(RpClump *clump, RwBBox *bbox);
RpClump *UtilClumpLoad(const RwChar *clumpPath);
RpAtomic *UtilCopyAtomic(RpAtomic *pstAtomic);
RpAtomic *UtilSetAtomicCB(RpAtomic *atomic, void *data);
RpAtomic *UtilSetUDAInt32CB(RpAtomic *atomic, void *data);

#endif			_AGCCHARACTERUTIL_H_