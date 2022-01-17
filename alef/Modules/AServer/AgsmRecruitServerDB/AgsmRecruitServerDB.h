/******************************************************************************
Module:  AgsmAuctionServer.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 2. 13
******************************************************************************/

#ifndef __AGSM_RECRUIT_SERVERDB_H__
#define __AGSM_RECRUIT_SERVERDB_H__

#include "AgsEngine.h"
#include "AuOLEDB.h"
#include "AgsmRecruitServer.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmRecruitServerDBD" )
#else
#pragma comment ( lib , "AgsmRecruitServerDB" )
#endif
#endif

class AgsmRecruitServerDB : public AgsModule
{
	AuOLEDBManager		*m_paAuOLEDBManager;
	AgsmRecruitServer	*m_pagsmRecruitServer;

public:

	AgsmRecruitServerDB();
	~AgsmRecruitServerDB();

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();

	BOOL ProcessDBOperation( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );

	INT32 GetAllClassFlag();

	//Search Query
	BOOL GetClassSearchQuery( char *pstrString, INT32 lStringSize, INT32 lClass );

	//DB Operation
	BOOL ProcessDBInsertIntoLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBInsertIntoLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBUpdateLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBUpdateLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBDeleteFromLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBDeleteFromLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBSearchFromLFP( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );
	BOOL ProcessDBSearchFromLFM( COLEDB *pcOLEDB, AgsmRecruitQueueInfo *pcsQueryInfo );

	static unsigned  WINAPI RecruitDBProcessThread( void *pvArg );
};

#endif
