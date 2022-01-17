#ifndef __AGCM_RENDER_TYPE_H__
#define __AGCM_RENDER_TYPE_H__

#include "ApModuleStream.h"
#include "AgcdRenderType.h"

#include <rwcore.h>
#include <rpworld.h>

typedef enum eAgcmClumpRenderTypeStreamReadResult
{
	E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_ERROR = 0,
	E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_PASS,
	E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_READ
} eAgcmClumpRenderTypeStreamReadResult;

#define AGCM_CLUMP_RENDER_TYPE_STREAM_NUM			"CRT_CLUMP_RENDER_TYPE_NUM"
#define AGCM_CLUMP_RENDER_TYPE_STREAM_RENDERTYPE	"CRT_CLUMP_RENDER_TYPE"
#define AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA1	"CRT_CUSTOM_DATA1"
#define AGCM_CLUMP_RENDER_TYPE_STREAM_CUSTOM_DATA2	"CRT_CUSTOM_DATA2"

class AgcmRenderType
{
public:
	AgcmRenderType();
	virtual ~AgcmRenderType();

	BOOL	NewClumpRenderType(AgcdClumpRenderType *pcsType, INT32 lNum);

	BOOL	SearchClumpRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsType, INT32 lNum);
	BOOL	SearchClumpRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsSrcType, AgcdClumpRenderType *pcsDstType);
	VOID	DeleteClumpRenderType(AgcdClumpRenderType *pcsType);

	INT32	StreamReadClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType);
	VOID	StreamWriteClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType);
};

#endif