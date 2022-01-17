#ifndef __AUNPROTECT_H__
#define __AUNPROTECT_H__

#ifdef _AREA_JAPAN_

class CCSAuth2;
struct _GG_AUTH_DATA;

class AuNProtect
{
public:
	AuNProtect();
	~AuNProtect();

	bool Init();
	bool CreateAuthObject( CCSAuth2** ppAuth2 );
	bool ServerToClient(CCSAuth2* pAuth2, _GG_AUTH_DATA* ggData);
	bool ClientToServer(CCSAuth2* pAuth2, _GG_AUTH_DATA* ggData);
};

extern AuNProtect g_nProtect;

#endif //_AREA_JAPAN_

#endif