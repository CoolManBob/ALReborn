#include "AgpmMonsterPath.h"
#include "AuIniManager/AuIniManager.h"
#include <fstream>


AgpmMonsterPath::AgpmMonsterPath( VOID )
:	m_nMonsterPathCount( 0 )
{
	SetModuleName("AgpmMonsterPath");
}

AgpmMonsterPath::~AgpmMonsterPath( VOID )
{
	AllClear();
}


BOOL		AgpmMonsterPath::OnInit( VOID )
{
	return TRUE;
}

BOOL		AgpmMonsterPath::OnAddModule( VOID )
{
	return TRUE;
}

BOOL		AgpmMonsterPath::OnDestroy( VOID )
{
	AllClear();

	return TRUE;
}

VOID		AgpmMonsterPath::AllClear( VOID )
{
	MonsterPathMapIter		Iter		=	m_MonsterPath.begin();

	for( ; Iter != m_MonsterPath.end() ; ++Iter )
	{
		STMonsterPathInfoPtr	pPtr	=	Iter->second;
		if( pPtr )
		{
			DEF_SAFEDELETE( pPtr );
		}
	}

	m_MonsterPath.clear();
}

STMonsterPathInfoPtr	AgpmMonsterPath::CreatePathInfo( const string &strSpawnName , ePathType eType )
{
	STMonsterPathInfoPtr	pMonsterPathInfo	=	GetMonsterPath( strSpawnName );
	if( pMonsterPathInfo )
		return pMonsterPathInfo;
		
	pMonsterPathInfo		=	new STMonsterPathInfo;
	if( !pMonsterPathInfo )
		return NULL;
	
	pMonsterPathInfo->strSpawnName	=	strSpawnName;
	pMonsterPathInfo->nPointCount	=	0;
	pMonsterPathInfo->bUpdate		=	TRUE;
	pMonsterPathInfo->eType			=	eType;

	_PushPathInfo( pMonsterPathInfo );

	return pMonsterPathInfo;
}

BOOL	AgpmMonsterPath::DestroyPathInfo( const string &strSpawnName )
{
	STMonsterPathInfoPtr	pMonsterPathInfo	=	GetMonsterPath( strSpawnName );
	if( !pMonsterPathInfo )
		return FALSE;
		
	return _PopPathInfo( pMonsterPathInfo );
}

BOOL	AgpmMonsterPath::DestroyPathInfo( STMonsterPathInfoPtr pMonsterInfo )
{
	if( !pMonsterInfo )
		return FALSE;

	return _PopPathInfo( pMonsterInfo );
}

BOOL	AgpmMonsterPath::LoadPath( const string &strINIFolder )
{
	HANDLE				hFindFile;
	WIN32_FIND_DATA		FindFileData;
	CHAR				szCurrPath[ MAX_PATH ];
	BOOL				bResult		=	TRUE;
	string				strFullPath;
	
	ZeroMemory( szCurrPath , MAX_PATH );
	GetCurrentDirectory( MAX_PATH , szCurrPath );

	strcat_s( szCurrPath , "\\" );
	strcat_s( szCurrPath , strINIFolder.c_str() );

	strFullPath		=	szCurrPath;
	strFullPath		+=	"\\";

	strcat_s( szCurrPath , "\\*" );

	hFindFile	=	FindFirstFile( szCurrPath , &FindFileData );
	if( hFindFile	==	INVALID_HANDLE_VALUE )
		return FALSE;

	while( bResult )
	{	

		if( !(FindFileData.dwFileAttributes	&	FILE_ATTRIBUTE_DIRECTORY) )
			_LoadPath( strFullPath + FindFileData.cFileName );

		bResult		=	FindNextFile( hFindFile , &FindFileData );

	}	
	FindClose( hFindFile );
	return TRUE;
}

BOOL	AgpmMonsterPath::SavePath( const string &strINIFolder , BOOL bUpdateListSave )
{
	// Data가 없으면 그냥 종료
	if( !GetMonsterPathCount() )
		return FALSE;

	MonsterPathMapIter		Iter		=	m_MonsterPath.begin();

	for( ; Iter != m_MonsterPath.end() ; ++Iter )
	{
		string					strFullFileName;
		STMonsterPathInfoPtr	pMonsterPath	=	Iter->second;

		// 바뀐 내용만 저장하도록 했다면 바뀐내용이 아니면 스킵시킨다
		if( bUpdateListSave )
		{
			if( !pMonsterPath->bUpdate )
				continue;
		}
		
		strFullFileName	=	strINIFolder;
		strFullFileName +=	"\\";
		strFullFileName +=	pMonsterPath->strSpawnName;
		strFullFileName +=	".txt";

		_SavePath( strFullFileName , pMonsterPath );
	}

	return TRUE;
}

BOOL	AgpmMonsterPath::OneSavePath( const string &strINIFolder , STMonsterPathInfoPtr pMonsterPath )
{
	if( !pMonsterPath )
		return FALSE;

	string			strFullPathName;

	strFullPathName		=	strINIFolder;
	strFullPathName		+=	"\\";
	strFullPathName		+=	pMonsterPath->strSpawnName;
	strFullPathName		+=	".txt";

	return _SavePath( strFullPathName , pMonsterPath );
}

BOOL	AgpmMonsterPath::OneSavePath( const string &strINIFolder , const string &strSpawnName )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterPath( strSpawnName );
	return OneSavePath( strINIFolder , pMonsterPath );
}

INT		AgpmMonsterPath::AddMonsterPath( STMonsterPathInfoPtr pMonsterPath , AuPOS Position )
{
	return AddMonsterPath( pMonsterPath , Position.x , Position.z );
}
	
INT		AgpmMonsterPath::AddMonsterPath( STMonsterPathInfoPtr pMonsterPath , INT nX , INT nZ )
{
	if( !pMonsterPath )
		return 0;

	STMonsterPathNode	PathNode;

	++pMonsterPath->nPointCount;
	PathNode.nPointIndex		=	pMonsterPath->nPointCount;
	PathNode.nX					=	nX;
	PathNode.nZ					=	nZ;

	pMonsterPath->PathList.push_back( PathNode );

	return pMonsterPath->nPointCount;
}

INT		AgpmMonsterPath::AddMonsterPath( const string& strSpawnName , AuPOS Position )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterPath( strSpawnName );

	return AddMonsterPath( pMonsterPath , Position.x , Position.z );
}

INT		AgpmMonsterPath::AddMonsterPath( const string& strSpawnName , INT nX , INT nZ )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterPath( strSpawnName );

	return AddMonsterPath( pMonsterPath , nX , nZ );
}

BOOL	AgpmMonsterPath::EditMonsterPath( STMonsterPathInfoPtr pMonsterPath , INT nIndex , INT nX , INT nZ )
{
	if( !pMonsterPath )
		return FALSE;

	MonsterPathListIter	Iter	=	pMonsterPath->PathList.begin();
	for( ; Iter != pMonsterPath->PathList.end() ; ++Iter )
	{
		if( Iter->nPointIndex	==	nIndex )
		{
			Iter->nX		=	nX;
			Iter->nZ		=	nZ;
			pMonsterPath->bUpdate	=	TRUE;

			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL	AgpmMonsterPath::EditMonsterPath( const string& strSpawnName , INT nIndex , INT nX , INT nZ )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterPath( strSpawnName );
	if( !pMonsterPath )
		return FALSE;

	return EditMonsterPath( pMonsterPath , nIndex , nX , nZ );
}

BOOL	AgpmMonsterPath::EditPathType( const string& strSpawnName , ePathType eNewType )
{
	STMonsterPathInfoPtr		pMonsterPath		=	GetMonsterPath( strSpawnName );

	return  EditPathType( pMonsterPath , eNewType );
}

BOOL	AgpmMonsterPath::EditPathType( STMonsterPathInfoPtr pMonsterPath , ePathType eNewType )
{
	if( !pMonsterPath )
		return FALSE;

	pMonsterPath->eType			=	eNewType;

	return TRUE;
}

INT		AgpmMonsterPath::DelMonsterPath( STMonsterPathInfoPtr pMonsterPath , AuPOS Position )
{
	if( !pMonsterPath )
		return -1;

	BOOL		bErase		=	FALSE;

	MonsterPathListIter		Iter		=	pMonsterPath->PathList.begin();
	for( ; Iter != pMonsterPath->PathList.end() ; ++Iter )
	{
		if( !bErase )
		{
			if( Iter->nX == Position.x && Iter->nZ == Position.z )
				pMonsterPath->PathList.erase( Iter );
		}

		else
		{
			--Iter->nPointIndex;
		}
	}

	return (--pMonsterPath->nPointCount);
}

INT		AgpmMonsterPath::DelMonsterPath( STMonsterPathInfoPtr pMonsterPath , INT nIndex )
{
	if( !pMonsterPath )
		return -1;

	BOOL		bErase		=	FALSE;
	MonsterPathListIter		Iter		=	pMonsterPath->PathList.begin();
	for( ; Iter != pMonsterPath->PathList.end() ; ++Iter )
	{
		if( !bErase )
		{
			if( Iter->nPointIndex	==	nIndex )
				pMonsterPath->PathList.erase( Iter );
		}

		else
		{
			--Iter->nPointIndex;
		}
	}

	return (--pMonsterPath->nPointCount);
}

INT		AgpmMonsterPath::DelMonsterPath( const string& strSpawnName , AuPOS Position )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterPath( strSpawnName );

	return DelMonsterPath( pMonsterPath , Position );
}	

INT		AgpmMonsterPath::DelMonsterPath( const string& strSpawnName , INT nIndex )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterPath( strSpawnName );

	return DelMonsterPath( pMonsterPath , nIndex );
}

STMonsterPathInfoPtr	AgpmMonsterPath::GetMonsterPath( const string& strSpawnName )
{
	MonsterPathMapIter	Iter	=	m_MonsterPath.find( strSpawnName );
	if( Iter != m_MonsterPath.end() )
		return Iter->second;

	return NULL;
}

INT		AgpmMonsterPath::GetMonsterPathCount( VOID )
{
	return	(INT)m_MonsterPath.size();
}

BOOL	AgpmMonsterPath::_PushPathInfo( STMonsterPathInfoPtr pPathInfo )
{
	if( !pPathInfo )
		return FALSE;

	m_MonsterPath.insert( make_pair( pPathInfo->strSpawnName , pPathInfo) );
	return TRUE;
}

BOOL	AgpmMonsterPath::_PopPathInfo( STMonsterPathInfoPtr pPathInfo )
{
	MonsterPathMapIter	Iter	=	m_MonsterPath.find( pPathInfo->strSpawnName );

	if( Iter != m_MonsterPath.end() )
	{
		m_MonsterPath.erase( Iter );
		return TRUE;
	}

	return FALSE;
}

BOOL	AgpmMonsterPath::_LoadPath( const string &strFullPath )
{
	std::ifstream		FileStream;

	string					strSpawnName;
	INT						nType				=	E_PATH_LOOP;
	INT						nIndexCount			=	0;
	INT						nX					=	0;
	INT						nZ					=	0;
	STMonsterPathInfoPtr	pMonsterPathInfo	=	NULL;

	// 해당 파일 하나를 읽어온다
	FileStream.open( strFullPath.c_str() );

	if( !FileStream )
		return FALSE;
	// Spawn 이름
	FileStream >> strSpawnName;

	// 경로 진행 타입
	FileStream >> nType;

	// 패스 갯수
	FileStream >> nIndexCount;

	pMonsterPathInfo	=	CreatePathInfo( strSpawnName , (ePathType)nType );

	if( !pMonsterPathInfo )
		return FALSE;

	// 모든 패스를 읽어온다
	for( INT i = 0 ; i < nIndexCount ; ++i )
	{
		FileStream >> nX;
		FileStream >> nZ;

		AddMonsterPath( pMonsterPathInfo , nX , nZ );
	}
	
	return TRUE;
}

BOOL	AgpmMonsterPath::_SavePath( const string &strFullPath , STMonsterPathInfoPtr pMonsterPath )
{
	std::ofstream		FileStream;

	FileStream.open( strFullPath.c_str() );

	if( !FileStream )
		return FALSE;

	FileStream << pMonsterPath->strSpawnName	<< endl;
	FileStream << pMonsterPath->eType			<< endl;
	FileStream << pMonsterPath->nPointCount		<< endl;

	MonsterPathListIter		Iter		=	pMonsterPath->PathList.begin();

	for( ; Iter != pMonsterPath->PathList.end() ; ++Iter )
	{
		FileStream << Iter->nX << endl;
		FileStream << Iter->nZ << endl;
	}
	
	return TRUE;
}