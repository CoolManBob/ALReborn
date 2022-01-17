#pragma once
//중국 중독 방지 모듈
// 게임접속후 시간이 지남에 따라 유저에게 패널티를 부여함으로서 게임에 중독을 방지함. supertj@081125

class AgpmCharacter;
class AgsmCharacter;
class CFCMSDKBase;

class AgsmGameholic
{
public:
	AgsmGameholic();
	~AgsmGameholic();

	// Initialize
	bool Initialize( AgpmCharacter* agpmCharacter, AgsmCharacter* agsmCharacter );

	// Query
	void Online (const char* account, const char* charID, char* ip);
	void Offline(const char* account, const char* charID, char* ip);

	// Callback functions
	static int  FcmCallback(const char* szUserID, const char* szRoleID,int iOnlineMinutes,int iOfflineMinutes);
	static void SwitchCallback(int needFCM);
	static void QueryCallback(const char* szUserID, const char* szRoleID,int iState ,int iOnlineMinutes,int iOfflineMinutes);

private:
	CFCMSDKBase* m_pFcm;

	static AgpmCharacter* m_pAgpmCharacter;
	static AgsmCharacter* m_pAgsmCharacter;
};

extern AgsmGameholic g_agsmGameholic;