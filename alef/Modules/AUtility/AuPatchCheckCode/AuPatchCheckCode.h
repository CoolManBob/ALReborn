#ifndef __PATCH_CHECK_CODE_H__
#define __PATCH_CHECK_CODE_H__

// 2006.02.16. steeple
// ignore deprecated warning message in VS2005
#if _MSC_VER >= 1400
#pragma warning (disable : 4996)
#endif

#define	MAX_LOGIN_PATCH_CODE		10000

class AuPatchCheckCode
{
	unsigned int	*m_piData;

	bool GenerateCode();

public:
	AuPatchCheckCode();
	~AuPatchCheckCode();

	bool LoadPatchCode( char *pstrFileName );
	bool SavePatchCode( char *pstrFileName );
	unsigned int GetCode( int iVer );
};

#endif