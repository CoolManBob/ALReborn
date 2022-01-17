#ifndef _AGSD_NPCMANAGER_H
#define _AGSD_NPCMANAGER_H

class CLuaStreamPack;

class CNpcExData
{
public:
	INT32 NPCID;
	INT32 IsSystemNPC;
	CHAR strFileName[MAX_PATH+1];
	CLuaStreamPack* pLuaPack;

	CNpcExData()
		: NPCID(0), IsSystemNPC(0), pLuaPack(NULL)
	{
		memset(strFileName, 0, sizeof(strFileName));
	};

	~CNpcExData()
	{
		if(pLuaPack)
			delete pLuaPack;
	};
};
#endif // _AGSD_NPCMANAGER_H
