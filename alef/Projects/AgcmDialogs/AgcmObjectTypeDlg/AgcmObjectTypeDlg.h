#pragma once

#include "rwcore.h"
#include "AcuObject.h"

#define AGCM_OBJECT_TYPE_DLG_MAX_STR		256

class AFX_EXT_CLASS AgcmObjectTypeDlg
{
public:
	AgcmObjectTypeDlg();
	virtual ~AgcmObjectTypeDlg();

	static AgcmObjectTypeDlg *GetInstance();

public:
	BOOL OpenObjectType(INT32 *pplObjectType, INT32 *pclObjectType, RwRGBA *pstRGBA);
};
