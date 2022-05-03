// ServerEngine.h: interface for the ServerEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERENGINE_H__B3FA669F_1E59_4C1D_9121_5F59441769EE__INCLUDED_)
#define AFX_SERVERENGINE_H__B3FA669F_1E59_4C1D_9121_5F59441769EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "AsCommonLib.h"
#include "ApmObject.h"

#include "AgpmAdmin.h"
#include "AgsmAdmin.h"

#include "AgpmLog.h"
#include "AgsmLog.h"

//#include "AuOLEDB.h"
#include "AuExcelTxtLib.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
#include "AgsmMap.h"
#include "AgsmItem.h"
#include "AgsmItemConvert.h"
#include "AgsmItemManager.h"
#include "AgsmCharManager.h"
#include "AgsmDeath.h"
#include "AgsmSkill.h"
#include "AgsmSkillManager.h"
#include "AgsmChatting.h"
#include "AgsmUIStatus.h"
#include "AgsmInterServerLink.h"
#include "AgsmDBPool.h"
//#include "AgsmMakeSQL.h"
#include "AgsmRelay2.h"
#include "AgpmAI2.h"
#include "AgpmPathFind.h"
#include "AgpmEventSpawn.h"


class ServerEngine : public AgsEngine
{
private:
	//BOOL	SetMaxCount();

	BOOL	LoadTemplateData();

public:
	ServerEngine();
	virtual ~ServerEngine();
	
	BOOL	OnRegisterModule();
	BOOL	OnTerminate();

	//BOOL	InitServerManager();
	//BOOL	ConnectServers();
	//BOOL	CreateDBPool();
	//BOOL	DisconnectServers();

	//BOOL	StartDBProcess();
	//BOOL	StopDBProcess();
};

#endif // !defined(AFX_SERVERENGINE_H__B3FA669F_1E59_4C1D_9121_5F59441769EE__INCLUDED_)
