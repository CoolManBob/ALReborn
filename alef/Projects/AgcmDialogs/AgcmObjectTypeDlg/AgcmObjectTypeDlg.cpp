#include "stdafx.h"
#include "../resource.h"
#include "AgcmObjectTypeDlg.h"
#include "ObjectTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AgcmObjectTypeDlg *pcsInstAgcmObjectTypeDlg = NULL;

AgcmObjectTypeDlg *AgcmObjectTypeDlg::GetInstance()
{
	return pcsInstAgcmObjectTypeDlg;
}

AgcmObjectTypeDlg::AgcmObjectTypeDlg()
{
}

AgcmObjectTypeDlg::~AgcmObjectTypeDlg()
{
}

BOOL AgcmObjectTypeDlg::OpenObjectType(INT32 *pplObjectType, INT32 *pclObjectType, RwRGBA *pstRGBA)
{
	CObjectTypeDlg dlg(pclObjectType, pstRGBA);
	if(dlg.DoModal() == IDOK)
	{
		if(pplObjectType)
		{
			if(*pclObjectType & ACUOBJECT_TYPE_BLOCKING)
			{
				*pplObjectType |= ACUOBJECT_TYPE_BLOCKING;
			}
			else
			{
				*pplObjectType &= ~ACUOBJECT_TYPE_BLOCKING;
			}

			if(*pclObjectType & ACUOBJECT_TYPE_RIDABLE)
			{
				*pplObjectType |= ACUOBJECT_TYPE_RIDABLE;
			}
			else
			{
				*pplObjectType &= ~ACUOBJECT_TYPE_RIDABLE;
			}
		}

		return TRUE;
	}

	return FALSE;
}