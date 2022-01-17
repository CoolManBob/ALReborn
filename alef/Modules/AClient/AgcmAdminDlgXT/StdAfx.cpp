// stdafx.cpp : source file that includes just the standard includes
//	AgcmAdminDlgXT.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


CXTPSetValueGridItem::CXTPSetValueGridItem(CString strCaption, CString strValue) : CXTPPropertyGridItem(strCaption, strValue, NULL)
{
}

void CXTPSetValueGridItem::SetValue(CString strValue)
{
	CXTPPropertyGridItem::SetValue(strValue);
}

