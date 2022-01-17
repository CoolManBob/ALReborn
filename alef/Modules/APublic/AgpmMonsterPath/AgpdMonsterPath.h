#pragma once

#include <list>
#include <string>

enum	ePathType
{
	E_PATH_LOOP			,				//	마지막 포인트에 도달시 처음위치로 이동해서 반복
	E_PATH_REVERSE		,				//	마지막 포인트에 도달시 역으로 반복

	E_PATH_COUNT		,
};


typedef struct	_stMonsterPathNode 
{

	INT		nX;			
	INT		nZ;

	INT		nPointIndex;

}STMonsterPathNode , *STMonsterPathNodePtr;

typedef std::list< STMonsterPathNode >					MonsterPathList;
typedef std::list< STMonsterPathNode >::iterator		MonsterPathListIter;

typedef struct	_stMonsterPathInfo
{
	_stMonsterPathInfo()	{	nPointCount	=	0;	bUpdate = FALSE; eType = E_PATH_LOOP;	}

	std::string				strSpawnName;			//	Spawn Name

	MonsterPathList			PathList;				//	경로 리스트
	INT						nPointCount;			//	경로 갯수
	ePathType				eType;					//	진행 방식

	BOOL					bUpdate;				//	Save시 저장되야 할 정보

}STMonsterPathInfo , *STMonsterPathInfoPtr;
