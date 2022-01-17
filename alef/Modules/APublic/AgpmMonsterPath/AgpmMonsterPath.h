#pragma once

// MK		2009. 5. 21

#include <string>
#include <map>
#include "ApModule.h"
#include "AgpdMonsterPath.h"

using namespace std;

class AgpmMonsterPath
	:	public	ApModule
{
private:
	typedef	map< string , STMonsterPathInfoPtr >				MonsterPathMap;
	typedef map< string , STMonsterPathInfoPtr >::iterator		MonsterPathMapIter;	

public:
	AgpmMonsterPath									( VOID );
	virtual ~AgpmMonsterPath						( VOID );

	virtual		BOOL			OnInit				( VOID );
	virtual		BOOL			OnAddModule			( VOID );
	virtual		BOOL			OnDestroy			( VOID );

	//	다 지운다
	VOID						AllClear			( VOID );							

	// Instance 생성
	STMonsterPathInfoPtr		CreatePathInfo		( const string	&strSpawnName , ePathType eType = E_PATH_LOOP	);	//	Monster Path Info 추가

	// Instance 지우기
	BOOL						DestroyPathInfo		( const string	&strSpawnName	);	//	Monster Path Info 삭제
	BOOL						DestroyPathInfo		( STMonsterPathInfoPtr	pMonsterInfo );

	// 해당 파일들이 있는 폴더를 지정하면 알아서 다 읽어와서 저장된다
	BOOL						LoadPath			( const string	&strINIFolder	);	//	Monster Path가 저장된 폴던에서 모두 읽어온다

	// 저장할 INI 폴더를 지정하면 저장한다
	// bUpdateListSave가 TRUE면 바뀐 값들만 저장된다
	// FALSE는 무조건 다 저장
	BOOL						SavePath			( const string	&strINIFolder , BOOL bUpdateListSave = TRUE	);	

	BOOL						OneSavePath			( const string	&strINIFolder , const string	&strSpawnName );
	BOOL						OneSavePath			( const string	&strINIFolder , STMonsterPathInfoPtr	pMonsterPath );

	//	해당 몬스터의 경로 추가
	INT							AddMonsterPath		( STMonsterPathInfoPtr pMonsterPath , AuPOS Position );		
	INT							AddMonsterPath		( STMonsterPathInfoPtr pMonsterPath , INT nX , INT nZ );	
	INT							AddMonsterPath		( const string& strSpawnName , INT nX , INT nZ	);
	INT							AddMonsterPath		( const string& strSpawnName , AuPOS Position );

	// 해당 몬스터의 경로 수정
	BOOL						EditMonsterPath		( const string& strSpawnName , INT nIndex ,  INT nX , INT nZ );
	BOOL						EditMonsterPath		( STMonsterPathInfoPtr pMonsterPath , INT nIndex ,  INT nX , INT nZ );

	// 해당 몬스터의 진행 방식 수정
	BOOL						EditPathType		( STMonsterPathInfoPtr pMonsterPath , ePathType	eNewType );
	BOOL						EditPathType		( const string& strSpawnName , ePathType eNewType );

	// 해당 몬스터의 경로 삭제
	INT							DelMonsterPath		( STMonsterPathInfoPtr pMonsterPath , INT nIndex );			//	해당 몬스터의 경로 삭제
	INT							DelMonsterPath		( STMonsterPathInfoPtr pMonsterPath , AuPOS Position );		//	해당 몬스터의 경로 삭제
	INT							DelMonsterPath		( const string& strSpawnName , INT nIndex );
	INT							DelMonsterPath		( const string& strSpawnName , AuPOS Position );

	// Get
	inline STMonsterPathInfoPtr	GetMonsterPath		( const string& strSpawnName	);
	inline INT					GetMonsterPathCount	( VOID );							//	갯수

private:
	BOOL						_LoadPath			( const string	&strFullPath	);									//	LoadPath 내부에서 사용
	BOOL						_SavePath			( const string	&strFullPath , STMonsterPathInfoPtr pMonsterPath );	//	SavePath 내부에서 사용

	BOOL						_PushPathInfo		( STMonsterPathInfoPtr pPathInfo );	//	map에 Path 데이타 추가
	BOOL						_PopPathInfo		( STMonsterPathInfoPtr pPathInfo );	//	map에 Path 데이타 삭제


	// Monster Path Data
	MonsterPathMap				m_MonsterPath;
	INT							m_nMonsterPathCount;
};
