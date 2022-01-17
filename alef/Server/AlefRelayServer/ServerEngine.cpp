// ServerEngine.cpp: implementation of the ServerEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerEngine.h"

// Global 변수 선언

//AuOLEDBManager		g_csAuOLEDBManager;
ApmMap*				g_pcsApmMap;
ApmObject*			g_pcsApmObject;
AgpmFactors*		g_pcsAgpmFactors;
AgpmCharacter*		g_pcsAgpmCharacter;
AgpmUnion*			g_pcsAgpmUnion;
AgpmGrid*			g_pcsAgpmGrid;
AgpmItem*			g_pcsAgpmItem;
AgpmItemConvert*		g_pcsAgpmItemConvert;
AgpmOptimizedPacket*	g_pcsAgpmOptimizedPacket;
AgpmCombat*			g_pcsAgpmCombat;
AgpmItemLog*		g_pcsAgpmItemLog;
AgpmParty*			g_pcsAgpmParty;
AgpmTimer*			g_pcsAgpmTimer;

ApmEventManager*	g_pcsApmEventManager;
AgpmEventNature*	g_pcsAgpmEventNature;
AgpmEventSpawn*		g_pcsAgpmEventSpawn;
AgpmSkill*			g_pcsAgpmSkill;
AgpmShrine*			g_pcsAgpmShrine;
AgpmChatting*		g_pcsAgpmChatting;
AgpmUIStatus*		g_pcsAgpmUIStatus;
AgpmGuild*			g_pcsAgpmGuild;
AgpmLog*			g_pcsAgpmLog;

//AgsmDBStream		g_csAgsmDBStream;
AgsmMap*			g_pcsAgsmMap;
AgsmAOIFilter*		g_pcsAgsmAOIFilter;
AgsmServerManager*	g_pcsAgsmServerManager;
AgsmInterServerLink*	g_pcsAgsmInterServerLink;
AgsmCharacter*		g_pcsAgsmCharacter;
AgsmZoning*			g_pcsAgsmZoning;
AgsmItem*			g_pcsAgsmItem;
AgsmItemConvert*		g_pcsAgsmItemConvert;
AgsmItemManager*	g_pcsAgsmItemManager;
AgsmItemLog*		g_pcsAgsmItemLog;
AgsmParty*			g_pcsAgsmParty;
AgsmCombat*			g_pcsAgsmCombat;
AgsmDeath*			g_pcsAgsmDeath;
AgsmSkill*			g_pcsAgsmSkill;
AgsmSkillManager*	g_pcsAgsmSkillManager;
AgsmSystemInfo*		g_pcsAgsmSystemInfo;
AgsmFactors*		g_pcsAgsmFactors;
AgsmAccountManager*	g_pcsAgsmAccountManager;
AgsmCharManager*	g_pcsAgsmCharManager;
AgsmChatting*		g_pcsAgsmChatting;
//AgsmLoginDB			g_csAgsmLoginDB;
AgsmUIStatus*		g_pcsAgsmUIStatus;
AgsmLog*			g_pcsAgsmLog;

AgsmDBPool*			g_pcsAgsmDBPool;
AgpmEventBinding*	g_pcsAgpmEventBinding;
AgsmMakeSQL*		g_pcsAgsmMakeSQL;

AgpmAdmin*			g_pcsAgpmAdmin;
AgsmAdmin*			g_pcsAgsmAdmin;
AgsmAdminDB*		g_pcsAgsmAdminDB;
AgsmRelay*			g_pcsAgsmRelay;

AgpmAI2*			g_pcsAgpmAI2;
AgpmPathFind*		g_pcsAgpmPathFind;

AsEchoServer*		g_pcsEchoServer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ServerEngine::ServerEngine()
{

}

ServerEngine::~ServerEngine()
{

}

BOOL ServerEngine::OnRegisterModule()
{
	g_AuCircularBuffer.Init(1 * 1024 * 1024);

//	REGISTER_MODULE(g_csAuOLEDBManager);
	REGISTER_MODULE(g_pcsAgpmItemLog, AgpmItemLog);
	REGISTER_MODULE(g_pcsAgpmLog, AgpmLog);
	REGISTER_MODULE(g_pcsApmMap, ApmMap);
	REGISTER_MODULE(g_pcsApmObject, ApmObject);
	REGISTER_MODULE(g_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(g_pcsAgpmCharacter, AgpmCharacter);
	REGISTER_MODULE(g_pcsAgpmUnion, AgpmUnion);
	REGISTER_MODULE(g_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(g_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(g_pcsAgpmItemConvert, AgpmItemConvert);
	REGISTER_MODULE(g_pcsAgpmOptimizedPacket, AgpmOptimizedPacket);
	REGISTER_MODULE(g_pcsAgpmParty, AgpmParty);
	REGISTER_MODULE(g_pcsAgpmTimer, AgpmTimer);
	REGISTER_MODULE(g_pcsApmEventManager, ApmEventManager);
	REGISTER_MODULE(g_pcsAgpmEventNature, AgpmEventNature);
	REGISTER_MODULE(g_pcsAgpmEventSpawn, AgpmEventSpawn);
	REGISTER_MODULE(g_pcsAgpmSkill, AgpmSkill);
	REGISTER_MODULE(g_pcsAgpmCombat, AgpmCombat);
	REGISTER_MODULE(g_pcsAgpmShrine, AgpmShrine);
	REGISTER_MODULE(g_pcsAgpmChatting, AgpmChatting);
	REGISTER_MODULE(g_pcsAgpmGuild, AgpmGuild);
	REGISTER_MODULE(g_pcsAgpmAdmin, AgpmAdmin);
	REGISTER_MODULE(g_pcsAgpmEventBinding, AgpmEventBinding);
	REGISTER_MODULE(g_pcsAgpmUIStatus, AgpmUIStatus);
	REGISTER_MODULE(g_pcsAgpmPathFind, AgpmPathFind);
	REGISTER_MODULE(g_pcsAgpmAI2, AgpmAI2);

	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	REGISTER_MODULE(g_pcsAgsmFactors, AgsmFactors);
	REGISTER_MODULE(g_pcsAgsmServerManager, AgsmServerManager);
	REGISTER_MODULE(g_pcsAgsmInterServerLink, AgsmInterServerLink);
	REGISTER_MODULE(g_pcsAgsmMap, AgsmMap);
	REGISTER_MODULE(g_pcsAgsmAccountManager, AgsmAccountManager);
	REGISTER_MODULE(g_pcsAgsmCharacter, AgsmCharacter);
	REGISTER_MODULE(g_pcsAgsmZoning, AgsmZoning);
	REGISTER_MODULE(g_pcsAgsmSystemInfo, AgsmSystemInfo);
	REGISTER_MODULE(g_pcsAgsmParty, AgsmParty);
	REGISTER_MODULE(g_pcsAgsmItem, AgsmItem);
	REGISTER_MODULE(g_pcsAgsmItemConvert, AgsmItemConvert);
	REGISTER_MODULE(g_pcsAgsmCharManager, AgsmCharManager);
	REGISTER_MODULE(g_pcsAgsmItemManager, AgsmItemManager);
	REGISTER_MODULE(g_pcsAgsmItemLog, AgsmItemLog);
	REGISTER_MODULE(g_pcsAgsmCombat, AgsmCombat);
	REGISTER_MODULE(g_pcsAgsmSkill, AgsmSkill);
	REGISTER_MODULE(g_pcsAgsmSkillManager, AgsmSkillManager);
	REGISTER_MODULE(g_pcsAgsmDeath, AgsmDeath);
	REGISTER_MODULE(g_pcsAgsmChatting, AgsmChatting);
	REGISTER_MODULE(g_pcsAgsmUIStatus, AgsmUIStatus);
	REGISTER_MODULE(g_pcsAgsmDBPool, AgsmDBPool);
	REGISTER_MODULE(g_pcsAgsmLog, AgsmLog);

	REGISTER_MODULE(g_pcsAgsmAdminDB, AgsmAdminDB);
	REGISTER_MODULE(g_pcsAgsmAdmin, AgsmAdmin);
	REGISTER_MODULE(g_pcsAgsmRelay, AgsmRelay);
	REGISTER_MODULE(g_pcsAgsmMakeSQL, AgsmMakeSQL);

	// disable idle events
	g_pcsAgpmCharacter->EnableIdle(FALSE);

	SetMaxCount();

	if (!Initialize())
		return FALSE;

	if (!LoadTemplateData())
		return FALSE;

	// Echo Server Start - 2004.05.16. steeple
	g_pcsEchoServer = new AsEchoServer();
	g_pcsEchoServer->SetServerTypeRelayServer();
	g_pcsEchoServer->Start();

	return TRUE;
}

BOOL ServerEngine::SetMaxCount()
{
	g_pcsAgpmCharacter->SetMaxCharacterTemplate( 200 );
	g_pcsAgpmCharacter->SetMaxCharacter( 2000 );
	g_pcsAgpmCharacter->SetMaxCharacterRemove( 1000 );
	g_pcsAgpmCharacter->SetMaxIdleEvent( 100 );

	g_pcsAgpmItem->SetMaxItemTemplate( 2000 );
	g_pcsAgpmItem->SetMaxItem( 20000 );
	//g_pcsAgpmItem->SetMaxItemRuneTemplate( 30 );

	g_pcsAgpmSkill->SetMaxSkill( 6000 );
	g_pcsAgpmSkill->SetMaxSkillTemplate( 150 );
	g_pcsAgpmSkill->SetMaxSkillSpecializeTemplate( 10	);

	g_pcsAgsmCharacter->SetMaxCheckRecvChar( 100	);
	g_pcsAgsmCharacter->SetMaxWaitForRemoveChar( 100	);

	g_pcsAgsmServerManager->SetMaxServer( 20 );
	g_pcsAgsmServerManager->SetMaxIdleEvent( 100 );

	g_pcsAgsmAOIFilter->SetMaxGroupCount( 20000 );

	//g_pcsAgsmConnectionSetupRelay->SetMaxIdleEvent(15);
	g_pcsAgsmInterServerLink->SetMaxIdleEvent2(15);

	return TRUE;
}

BOOL ServerEngine::LoadTemplateData()
{

	// module initialize
	g_pcsApmMap->SetLoadingMode	( TRUE , FALSE	);
	g_pcsApmMap->SetAutoLoadData	( FALSE			);


	if( !g_pcsApmMap->Init( NULL, ".\\Map\\Data\\Moonee", ".\\Map\\Data\\Compact", ".\\World" ) )
		return FALSE;

	g_pcsApmMap->LoadAll(FALSE);

	if (!g_pcsAgpmSkill->			StreamReadTemplate("Ini\\SkillTemplate.ini"	, NULL, FALSE		)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadTemplate 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmFactors->		CharacterTypeStreamRead("Ini\\CharType.ini"	, FALSE		)	)
	{
		ASSERT( !"g_csAgpmFactors CharacterTypeStreamRead 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadTemplate("Ini\\CharacterTemplate.ini"	, FALSE	)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadTemplate 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadLevelUpExpTxt("Ini\\LevelUpExp.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadLevelUpExpTxt 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadCharGrowUpTxt("Ini\\GrowUpFactor.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharGrowUpTxt 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmCharacter->		StreamReadCharKind("Ini\\CharKind.ini"				)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharKind 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmItem->			StreamReadTemplate("Ini\\ItemTemplate.ini"			)	)
	{
		ASSERT( !"g_csAgpmItem StreamReadTemplate 실패" );
		return FALSE;
	}

	/*
	if (!g_pcsAgpmItem->			StreamReadRuneTemplate("Ini\\ItemRuneTemplate.ini"	)	)
	{
		ASSERT( !"g_csAgpmItem StreamReadRuneTemplate() 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmItem->			StreamReadRuneAttribute("Ini\\ItemRuneAttribute.txt"	)	)
	{
		ASSERT( !"g_csAgpmItem StreamReadRuneAttribute() 실패" );
		return FALSE;
	}
	*/

	if (!g_pcsAgpmSkill->			ReadSkillSpecTxt("Ini\\Skill_Spec.txt", FALSE				)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillSpecTxt 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmSkill->			ReadSkillConstTxt("Ini\\Skill_Const.txt", FALSE			)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillConstTxt 실패" );
		return FALSE;
	}

	/*
	if (!g_pcsAgpmSkill->			ReadSkillMasteryTxt("Ini\\Skill_Mastery.txt", FALSE		)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillMasteryTxt 실패" );
		return FALSE;
	}
	*/

	if (!g_pcsAgpmSkill->			StreamReadSpecialize("Ini\\SkillSpecialization.ini"	, FALSE	)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadSpecialize 실패" );
		return FALSE;
	}

	if (!g_pcsAgpmGuild->			SetMaxGuild(5))
	{
		ASSERT( !"g_pcsAgpmGuild SetMaxGuild 실패");
		return FALSE;
	}

	//if (!g_pcsAgsmServerManager->StreamReadServerInfo("Ini\\ServerTemplate.ini"))
	if (!g_pcsAgsmServerManager->StreamReadServerInfo("SERVERINFO"))
	{
		ASSERT( !"g_csAgsmServerManager StreamReadServerInfo 실패" );
		return FALSE;
	}

	return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	// Echo Server Stop - 2004.05.16. steeple
	if(g_pcsEchoServer)
	{
		g_pcsEchoServer->Stop();
		delete g_pcsEchoServer;
		g_pcsEchoServer = NULL;
	}

	return TRUE;
}

BOOL ServerEngine::ConnectServers()
{
	return TRUE;
}

BOOL ServerEngine::CreateDBPool()
{
	g_pcsAgsmDBPool->CreateIOCPDatabase();
	//g_csDBPool.SetConnectParam("Tak", "alef", "alef");
	g_pcsAgsmDBPool->SetConnectParam("archlord", "alef1", "alef123");
	if(!g_pcsAgsmDBPool->Create(10))
		return FALSE;

	return TRUE;
}

BOOL ServerEngine::DisconnectServers()
{
	return TRUE;
}

BOOL ServerEngine::InitServerManager()
{
	if (!g_pcsAgsmServerManager->InitThisServer())
	{
		ASSERT( !"g_csAgsmServerManager InitServerData 실패" );
		return FALSE;
	}

	return TRUE;
}

BOOL ServerEngine::StartDBProcess()
{
	// ItemLog DB Start
	g_pcsAgsmItemLog->InitODBC();

	// Admin DB Start
	//g_csAgsmAdminDB.InitOLEDB();
	g_pcsAgsmAdminDB->InitODBC();

	return TRUE;
}

BOOL ServerEngine::StopDBProcess()
{
	g_pcsAgsmAdminDB->StopODBCThread();
	//g_csAgsmAdminDB.StopOLEDBThread();
	
	return TRUE;
}