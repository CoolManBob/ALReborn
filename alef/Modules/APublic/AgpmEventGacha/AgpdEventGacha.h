#pragma once

#include <map>

using namespace std;

struct	AgpdGachaDropTable
{
    struct    DropInfo
    {
        INT32    nItemTID;        // 겜블 아이템

        INT32    nItemNeeded;    // 아이템 갯수. . 0이면 체크 안함

        INT32    nCost;    // 필요한 돈 액수
        INT32	nNeedCharisma; // 필요 카리스마 포인트 - arycoat

        //FLOAT    fRank1;	// 나머지 이니까 저장할 필요가 없음
        FLOAT    fRank2;
        FLOAT    fRank3;
        FLOAT    fRank4;    // 랭크별 값 저장 ,
							// 로드를 할때 미리 계산하여 확율값을 저장함.

		DropInfo(): nItemTID( 0 ) , nItemNeeded( 0 ) , nCost( 0 ), nNeedCharisma(0),
			fRank2( 0.0f ) ,fRank3( 0.0f ) ,fRank4( 0.0f ) {}
    };

    map<INT32,DropInfo>    mapGacha;

    BOOL    StreamRead( const char * pFilename , BOOL bDecryption );
};

struct AgpdGachaItemTable
{
	typedef map< INT32 , vector< INT32 > > Table; // Rank , TID

	// Level , Item vector
    map< INT32 , Table >    mapItemTable;
    // 백터의 맵으로 구성.

    bool ValidationCheck(); // 당위성 확인... 비어있는 레벨대가 없는지 확인.
};

struct AgpdGachaType
{
	INT32		nID;
	std::string	strName;
	std::string	strComment;
	bool		bRaceCheck;
	bool        bClassCheck;
	bool        bLevelCheck;    // 아이템 사용 레벨 체크
	INT32       nLevelLimit;    // 가챠 플레이 레벨 체크 
								// 이 레벨보다 같거나 클경우 사용 가능함.
	AgpdGachaDropTable	stDropTable;	// 비용과 데이타 정산..

	map< AuRace , AgpdGachaItemTable	>	mapGachaTable	;
	AgpdGachaItemTable	*	GetGachItemTable( AgpdCharacter * pcsCharacter	);
	AgpdGachaItemTable	*	GetGachItemTable( AuRace stRace	);

	AgpdGachaType() : nID( -1 ) , bRaceCheck( false ), bClassCheck( false ),
		bLevelCheck( false ) , nLevelLimit( 0 ) {}
};