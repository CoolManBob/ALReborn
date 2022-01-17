#ifndef	__AGPMRESOURCEINFO_H__
#define	__AGPMRESOURCEINFO_H__

#include "ApBase.h"
#include "ApModule.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmResourceInfoD" )
#else
#pragma comment ( lib , "AgpmResourceInfo" )
#endif
#endif

class AgpmResourceInfo : public ApModule {
private:
	INT32			m_lMajorVersion;
	INT32			m_lMinorVersion;

public:
	AgpmResourceInfo();
	virtual ~AgpmResourceInfo();

	INT32			GetMajorVersion();
	INT32			GetMinorVersion();

	BOOL			SetMajorVersion(INT32 lMajorVersion);
	BOOL			SetMinorVersion(INT32 lMinorVersion);

	BOOL			CheckValidVersion(INT32 lMajorVersion, INT32 lMinorVersion);
};

#endif	//__AGPMRESOURCEINFO_H__