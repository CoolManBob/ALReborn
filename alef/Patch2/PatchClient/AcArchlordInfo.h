#pragma once

#include <string>
#include <vector>

#include "AuXmlParser.h"

using namespace std;



// Archlord.ini 파일에서 정보를 읽어온다
// 로그인 IP 주소만 들어온다

struct	stLoginServerInfo 
{
	string				m_strServerIP;		//	Login Server IP
	BOOL				m_bTryConnect;		//	접속 시도했는지 여부

};

typedef vector< stLoginServerInfo >					VecServerInfo;
typedef vector< stLoginServerInfo >::iterator		VecServerInfoIter;

// 그룹에 매칭되는 로그인 서버 정보가 저장
struct	 stLoginGroup
{
	string				m_strGroupName;
	VecServerInfo		m_vecServerInfo;
};


typedef vector< stLoginGroup* >				VecLoginGroup;
typedef vector< stLoginGroup* >::iterator	VecLoginGroupIter;



class AcArchlordInfo
{
private:


public:
	AcArchlordInfo				( VOID );
	virtual ~AcArchlordInfo		( VOID );

	BOOL						LoadFile				( IN BOOL bEncryt );
	VOID						Destroy					( VOID );

	stLoginGroup*				GetGroup				( INT nIndex );
	INT32						GetGroupCount			( VOID )	{	return (INT32)m_VecLoginGroup.size();	}

	stLoginServerInfo*			GetPatchServer			( INT nIndex );
	INT32						GetPatchServerCount		( VOID )	{	return (INT32)m_VecPatchServer.size();	}

private:
	BOOL						_LoadArchlordInfoXML	( IN CONST string& strFileName );

	AuXmlDocument				m_XMLDocument;
	AuXmlNode*					m_XMLRootNode;

	VecLoginGroup				m_VecLoginGroup;
	VecServerInfo				m_VecPatchServer;

};
