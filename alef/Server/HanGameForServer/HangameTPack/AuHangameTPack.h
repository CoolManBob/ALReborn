#ifndef _AUHANGAMETPACK_
#define _AUHANGAMETPACK_

#include "ApDefine.h"
#include "ApMutualEx.h"

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
class AuHangameTPack
{
private:
	ApMutualEx				m_csMutex;
			
public:
	AuHangameTPack();
	~AuHangameTPack();

	BOOL Create(INT32 lServerID);
	void Destroy();

	BOOL CheckIn(CHAR* szAccount, CHAR* szIP);
	BOOL CheckOut(CHAR* szAccount);
};
#endif // _HANGAME_
#endif // _AREA_KOREA_
#endif // _AUHANGAMETPACK_
