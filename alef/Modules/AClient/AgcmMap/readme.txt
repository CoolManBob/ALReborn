extern RwBool
RpDWSectorSetWriteMode(RpWorld *world, RwUInt8 mode);

// 월드의 Write 모드만 변경한다. , 월드를 세이브할때
// 모든 DWSector 를 러프로 설정해두고 , rpDWSECTOR_WRITEMODE_CURRENT 모드를 적용하여
// BSP익스포트를 하면 러프에 대한 정보만 폴리건 안으로 들어간다. 
// 게임 클라이언트에서는 이 러프로 익스포트된 bsp 파일을 로딩하여 지형을 보여주고,
// 플레이어가 있는 부분만을 따로 로딩해서 사용한다.

extern RwStream *
RpDWSectorStreamWrite(RpDWSector *dwSector, RwStream *stream);

extern RwStream *
RpDWSectorStreamRead(RpDWSector *dwSector, RwStream *stream);

extern RwInt32
RpDWSectorStreamGetSize(RpDWSector *dwSector);

// 스트리밍 펑션들로 , 플레이어가 들어가는 자리에 저 펑션을 써서 메모리에 로딩한다.
// AgcmMap에선 각 섹터별로 월드 섹터에 대한 포인터를 맵툴처럼 가지고 있어야한다.
// 그래서 , 플레이어의 위치에 따라서 데이타를 로딩하는 루틴을 넣는다.
// DWSector 의 디테일 정보는 맵툴에서 로딩하는 단위 만큼 저장을 한다. 
// 읽어들일때는 FindChuck를 이용하여 순서대로 저장된 것을 하나씩 넘어가서
// 찾아내서 읽어 들이도록 한다.

// 만들어야할 펑션들
1,	AgcmMap::LoadDwSector( int x , int y )
	AgcmMap::LoadDwSector( ApWorldSector *  )
  해당 섹터를 로드한다.
2,	AgcmMap::ReleaseDwSector( int x , int y )
	AgcmMap::ReleaseDwSector( ApWorldSector *  )
	해당 섹터 데이타를 릴리즈 한다.
3,	AgcmMap::SetPlayerPostion( ApPOS )
	// 섹터 데이타를 체계적으로 로딩 한다.
	// ApPOS를 기준으로 주위에 로딩이 되어 있는 데이타를 점검하고.
	// 필요한것은 로딩하고 , 사용돼지 않는것은 메모리에서 제거한다.
	// 실제로 외부에서 사용하는 펑선은 이것 하나 뿐.

// 테스트 클라이언트 제작.
필요한 기능.
1,	월드를 로딩함
2,	특정 월드를 DWSector 를 로딩하고 디테일 변경
3,	전체 디테일 변경해서 로딩돼어졌는지 확인.
