#ifndef __ACU_FILE_ADMIN_H__
#define __ACU_FILE_ADMIN_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuFileAdminD" )
#else
#pragma comment ( lib , "AcuFileAdmin" )
#endif
#endif

#include "ApAdmin.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuFileAdminD" )
#else
#pragma comment ( lib , "AcuFileAdmin" )
#endif
#endif

#define ACU_FILE_ADMIN_MAX_FILE_NAME 256

class AcuFileAdmin : public ApAdmin
{
public:
	AcuFileAdmin();
	virtual ~AcuFileAdmin();

	PVOID AddFile(CHAR* szFile, PVOID pvData);
	PVOID GetFile(CHAR* szFile);
	BOOL RemoveFile(CHAR* szFile);
};

#endif // __E_FILE_ADMIN_H__

/******************************************************************************
******************************************************************************/