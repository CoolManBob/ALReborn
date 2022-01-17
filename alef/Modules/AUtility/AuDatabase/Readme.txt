========================================================================
       STATIC LIBRARY : auDatabase
========================================================================

Usage 1:

	// 매니저 생성 및 초기화.
	auDatabaseManager Manager;
	Manager.Initialize(auDatabaseOLEDB, auOracle);		enum은 auDatabase.h 참조

	// Heap을 따로 create하길 원하믄...
	Manager.InitializeHeap(dwSize);

	// connection creation
	auConnection* pConnection = Manager.CreateConnecton();

	// connection open
	pConnection->Open(dsn, user, password);

	// if you wnat to use transaction facility
	pConnection->StartTransaction();

	// statment creation
	auStatement* pStatement = Manager.CreateStatement(pConnection);

	// statement open and execute
	pStatement->Open();
	pStatement->Execute(SQL);

	
	if (pStatement->HasResult)		// select
		;
	else			//if update, insert, delete, ...
		;

	auRowset* pRowset = Manger.CreateRowset(pStatement);
	
	pRowset->Bind(bBindToString = TRUE);	// 이때 컬럼수 컬럼명 결정됨.
											// bBindToString은 DB값을 버퍼에 바인딩할때 스트링으로 변환해서 할지
											// 아니면 각 타입별로 할지를 정한다. default는 string

	while (!pRowset->IsEnd())
		pRowset->Fetch(FetchCount))		// Fetch한 결과는 이전 결과에 축적되며
										// row count는 마지막 fetch된 넘 까지(쿼리의 총 결과수가 아님)

	LPCTSTR pHeader = pRowset->GetHeader(ulCol);		// 컬럼명

	for (r... GetRowCount()
		for (c... GetColCount()
			LPCTSTR pValue = pRowset->Get(r, c);		// 값. 

					
												


	pRowset->Release();

	각 instance마다 close는 명시적으로 해줘도 되구 안해줘도 됨.

Usage 2: alternative use of rowset

	auRowset* pRowset = Manger.CreateRowset(pStatement);
	pRowset->InitializeBuffer(size); !!! K-Byte !!!

	pStatement 맹그러서 실행 시키고...

	prowset->Set(pStatement);		// reset used size (to 0)

	bind

	fetch




////////////////////////////////////////////////////////
개선해야 할 사항.

현재는 String으로 Binding한다. 이 경우 Result Set의 사이즈가 커지는 단점이 있다.
이를 각 Type별로 Binding할 경우(eg. NUMBER(3) - short) auRowset에 GetHeader() 대신 
Bind된 Column의 정보를 알수있도록 GetColumnInfo()와 같은 메소드를 추가하고
Name, Type, Size등을 알 수 있도록 해야한다.
또한 Get()도 각 Type에 맞게 돌려줄 수 있도록 확장되어야 한다.

auEnvironmentOLEDB에서 enumerator를 사용하지 않는다.
원래 의도는 해당 eauDatabaseVender를 받아 해당 datasource를 찾으려 한건디
안쓰고있다. 그래도 걍 써라... 나중에 필요하믄 한당.

MySQL Test

Information method 추가해라.
		IsOpen()
		NumConnection();
		NumStatement() 등등... 

Parameter Set/Get method추가해라...

Type 조정. ApDefine에 있는 넘들로 맞춰라...

Database(connection)에 timeout추가...

BUG
	ODBC에서 DATE타입을 STRING(SQL_C_TCHAR)으로 바인딩할 경우...
	로우의 값이 NULL일때 SQLFetch()에서 에러 발생.
	Fetch가 되기전에는 실제 값이 NULL인지 아닌지 알 수 없어 해결방안이 요원하다.
	오라클만 그런건지 다 그런건지는 확인해봐야한다.