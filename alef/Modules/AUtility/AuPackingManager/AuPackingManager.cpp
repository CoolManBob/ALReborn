#include "AuPackingManager.h"
#include <windows.h>
#include <commctrl.h>
#include <io.h>
#include <dbghelp.h>

#define MAX_FILECOUNT_PER_FOLDER				20000


AuPackingManager* AuPackingManager::m_pThis = NULL;

void CPackingFile::Reset()
{
/*	CPackingDataNode	*pcsNextNode;
	CPackingDataNode	*pcsCurrentNode;

	pcsCurrentNode = m_pcsDataNode;

	while( 1 )
	{
		if( pcsCurrentNode == NULL )
			break;

		pcsNextNode = pcsCurrentNode->m_pcsNextNode;

		if( m_pcsDataNode != NULL )
            delete m_pcsDataNode;

		m_pcsDataNode = NULL;

		pcsCurrentNode = pcsNextNode;
	}*/

	if ( m_pcsDataNode )
	{
		delete m_pcsDataNode;
		m_pcsDataNode = NULL;
	}

	if( m_pstrFileName != NULL )
	{
		delete [] m_pstrFileName;
		m_pstrFileName = NULL;
	}
}

CPackingFolder::CPackingFolder()
{
	m_csFilesInfo.InitializeObject( sizeof(CPackingFile *), MAX_FILECOUNT_PER_FOLDER );

	m_Mutex.Init();
	
//	m_lEditing  = AuPacking_Status_Build;
	m_bExportToRef = false;

	m_lEmptyBlockCount = 0;
	m_lBlockSize = 0;
	m_pstrBlock = NULL;

	m_lFolderNameSize = 0;
	m_pstrFolderName = NULL;
}

CPackingFolder::~CPackingFolder()
{
	m_Mutex.Destroy();

	if( m_pstrFolderName )
	{
		delete [] m_pstrFolderName;
		m_pstrFolderName = NULL;
	}
}

void CPatchFolderInfo::SetPacking( bool bPacking )
{
	m_bPacking = bPacking;
}

AuPackingManager::AuPackingManager()
{
	m_lCurrentVersion = 0;

	m_csFolderInfo.InitializeObject( sizeof(CPackingFolder *), MAX_FILECOUNT_PER_FOLDER );
	m_csCrackedFolderInfo.InitializeObject( sizeof(CCrackedFolderInfo *), MAX_FILECOUNT_PER_FOLDER );
	m_csIgnoreFileInfo.InitializeObject( sizeof(CIgnoreFileInfo *), MAX_FILECOUNT_PER_FOLDER );
	//m_csLoadingFolder.InitializeObject( sizeof(void *), 1000 );

	//Read Only Mode
	m_bReadOnlyMode = true;

	//옵션세팅.
	m_bBuildJZP = true;
	m_bBuildDat = true;
	m_bCopyRawFiles = true;
//	m_bLoadFromRawFile = true;

//	setLoadFromPackingData( true );

	//@{ 2006/04/11 burumal
	m_cCompress.InitMemPool();
	//@}

	//@{ 2006/04/14 burumal
	m_nEmptyFileSerial = 0;
	//@}

	m_pThis = NULL;
}


AuPackingManager::~AuPackingManager()
{
	//@{ 2006/04/11 burumal
	m_cCompress.RemoveMemPool();
	//@}

	m_pThis = NULL;
}

void AuPackingManager::Destroy()
{
	//Folder를 한번 쭈욱~ 돌아보자꾸나~
	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;

	int				lFolderIndex;
	int				lFileIndex;

	lFolderIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( ppcsPackingFolder && *ppcsPackingFolder )
		{
			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
			{
				if( ppcsPackingFile && *ppcsPackingFile )
				{
					delete (*ppcsPackingFile);
				}
			}

			(*ppcsPackingFolder)->m_csFilesInfo.RemoveObjectAll();

			delete *ppcsPackingFolder;
		}
	}

	m_csFolderInfo.RemoveObjectAll();
}

ApAdmin	*AuPackingManager::GetFolderInfo()
{
	return &m_csFolderInfo;
}

CDList<CPatchVersionInfo *> *AuPackingManager::GetPatchList()
{
	return &m_cPatchList;
}

bool AuPackingManager::IsCheckedBuildJZP()
{
	return m_bBuildJZP;
}

bool AuPackingManager::IsCheckedBuildDat()
{
	return m_bBuildDat;
}

bool AuPackingManager::IsCheckedCopyRawFiles()
{
	return m_bCopyRawFiles;
}

void AuPackingManager::SetBuildJZP( bool bBuildJZP )
{
	m_bBuildJZP = bBuildJZP;
}

void AuPackingManager::SetBuildDat( bool bBuildDat )
{
	m_bBuildDat = bBuildDat;
}

void AuPackingManager::SetCopyRawFiles( bool bCopyRawFiles )
{
	m_bCopyRawFiles = bCopyRawFiles;
}

AuPackingManager* AuPackingManager::MakeInstance()
{
	if(!m_pThis)
	{
#ifdef new
#undef new
#endif
		m_pThis = new AuPackingManager;
		atexit(AuPackingManager::DestroyInstance);
#ifdef new
#undef new
#define new DEBUG_NEW
#endif
	}
	
	return	m_pThis;
}

void AuPackingManager::DestroyInstance()
{
	if (m_pThis)
	{
		AuPackingManager *	pThis = m_pThis;

		m_pThis = NULL;

		pThis->Destroy();

		delete pThis;
	}
}

AuPackingManager* AuPackingManager::GetSingletonPtr()
{
	return m_pThis;
}

/*void AuPackingManager::setLoadFromPackingData( bool bSet )
{
	m_bLoadFromPackingData = bSet;
}*/

bool AuPackingManager::LoadIgnoreFileInfo( char *pstrRootFolder, char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	if( pstrFileName )
	{
		FILE			*file;
		char			strOriginDir[255];

		GetCurrentDirectory( sizeof(strOriginDir), strOriginDir );

		file = fopen( pstrFileName, "rb" );
		if( file )
		{
			char			strSearchCommand[255];

			while( 1 )
			{
				if( EOF == fscanf( file, "%s", strSearchCommand ) )
				{
					break;
				}
				else
				{
					//해당폴더의 파일이름을 쫘악~~~ 읽어들여보자~ 아싸조쿠나~~~
					HANDLE				hHandle;
					WIN32_FIND_DATA		csFindData;
					char				strSearchFolder[255];
					char				strFolderName[255];
					char				strFileFullPath[255];

					bool				bResult;

					bResult = false;

					_splitpath( strSearchCommand, NULL, strFolderName, NULL, NULL );

					SetCurrentDirectory( pstrRootFolder );
					sprintf( strSearchFolder, "%s\\%s", pstrRootFolder,strSearchCommand );

					hHandle = FindFirstFile( strSearchFolder, &csFindData );

					if( INVALID_HANDLE_VALUE != hHandle )
					{
						if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							;
						}
						else
						{
							if( !strcmp( strFolderName, "\\" ) )
							{
								sprintf( strFileFullPath, "%s\\%s", pstrRootFolder, csFindData.cFileName );
							}
							else
							{
								sprintf( strFileFullPath, "%s\\%s%s", pstrRootFolder, strFolderName, csFindData.cFileName );
							}

							AddIgnoreFileName( strFileFullPath );
						}
					}

					while( 1 )
					{
						if( FindNextFile( hHandle, &csFindData ) == FALSE )
							break;

						//폴더라면 무시~
						if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							continue;
						}
						//파일이라면 무시 파일리스트에 추가한다~ 아싸조쿠나~
						else
						{
							if( !strcmp( strFolderName, "\\" ) )
							{
								sprintf( strFileFullPath, "%s\\%s", pstrRootFolder, csFindData.cFileName );
							}
							else
							{
								sprintf( strFileFullPath, "%s\\%s%s", pstrRootFolder, strFolderName, csFindData.cFileName );
							}

							AddIgnoreFileName( strFileFullPath );
						}
					}

					FindClose( hHandle );
				}
			}

			fclose( file );
		}

		SetCurrentDirectory( strOriginDir );
	}

	return bResult;
}

bool AuPackingManager::AddIgnoreFileName( char *pstrIgnoreFileName )
{
	CIgnoreFileInfo		*pcsIgnoreFileInfo;

	pcsIgnoreFileInfo = new CIgnoreFileInfo;

	pcsIgnoreFileInfo->m_lFileNameSize = strlen( pstrIgnoreFileName ) + 1;
	pcsIgnoreFileInfo->m_pstrFileName = new char[pcsIgnoreFileInfo->m_lFileNameSize];
	memset( pcsIgnoreFileInfo->m_pstrFileName, 0, pcsIgnoreFileInfo->m_lFileNameSize );
	strcat( pcsIgnoreFileInfo->m_pstrFileName, pstrIgnoreFileName );

	if( m_csIgnoreFileInfo.AddObject( (void **)&pcsIgnoreFileInfo, _strlwr(pstrIgnoreFileName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AuPackingManager::RemoveIgnoreFileName( char *pstrIgnoreFileName )
{
	CIgnoreFileInfo		**ppcsIgnoreFileInfo;

	ppcsIgnoreFileInfo = (CIgnoreFileInfo **)m_csIgnoreFileInfo.GetObject( _strlwr(pstrIgnoreFileName) );

	if( ppcsIgnoreFileInfo && (*ppcsIgnoreFileInfo) )
	{
		delete [] (*ppcsIgnoreFileInfo)->m_pstrFileName;

		delete (*ppcsIgnoreFileInfo);
	}

	if( m_csIgnoreFileInfo.RemoveObject( _strlwr(pstrIgnoreFileName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AuPackingManager::IsIgnoreFileName( char *pstrIgnoreFileName )
{
	if( m_csIgnoreFileInfo.GetObject( _strlwr(pstrIgnoreFileName) ) )
		return true;
	else
		return false;
}

ApAdmin *AuPackingManager::GetIgnoreFileInfo()
{
	return &m_csIgnoreFileInfo;
}

bool AuPackingManager::AddCrackedFolderName( char *pstrCrackedFolderName )
{
	CCrackedFolderInfo		*pcsCrackedFolderInfo;

	pcsCrackedFolderInfo = new CCrackedFolderInfo;

	pcsCrackedFolderInfo->m_lFolderNameSize = strlen( pstrCrackedFolderName ) + 1;
	pcsCrackedFolderInfo->m_pstrFolderName = new char[pcsCrackedFolderInfo->m_lFolderNameSize];
	memset( pcsCrackedFolderInfo->m_pstrFolderName, 0, pcsCrackedFolderInfo->m_lFolderNameSize );
	strcat( pcsCrackedFolderInfo->m_pstrFolderName, pstrCrackedFolderName );

	if( m_csCrackedFolderInfo.AddObject( (void **)&pcsCrackedFolderInfo, _strlwr(pstrCrackedFolderName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AuPackingManager::RemoveCrackedFolderName( char *pstrCrackedFolderName )
{
	CCrackedFolderInfo		**ppcsCrackedFolderInfo;

	ppcsCrackedFolderInfo = (CCrackedFolderInfo **)m_csCrackedFolderInfo.GetObject( _strlwr(pstrCrackedFolderName) );

	if( ppcsCrackedFolderInfo && (*ppcsCrackedFolderInfo) )
	{
		delete [] (*ppcsCrackedFolderInfo)->m_pstrFolderName;

		delete (*ppcsCrackedFolderInfo);
	}

	if( m_csCrackedFolderInfo.RemoveObject( _strlwr(pstrCrackedFolderName) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

ApAdmin *AuPackingManager::GetCrackedFolderInfo()
{
	return &m_csCrackedFolderInfo;
}

int AuPackingManager::GetLastVersion()
{
	int				lVersion;

	if( m_cPatchList.getCount() == 0 )
	{
		lVersion = 0;
	}
	else
	{
		CListNode<CPatchVersionInfo *>	*pcsNode;

		pcsNode = m_cPatchList.GetEndNode();

		if( pcsNode )
		{
			lVersion = pcsNode->m_tData->m_lVersion;
		}
		else
		{
			lVersion = 0;
		}
	}

	return lVersion;
}

CPatchVersionInfo *AuPackingManager::GetPatchInfo( int lVersion )
{
	CListNode<CPatchVersionInfo *>		*pcsPatchVersionNode;
	CPatchVersionInfo *pcsResult;

	pcsResult = NULL;

	for( pcsPatchVersionNode=m_cPatchList.GetStartNode(); pcsPatchVersionNode; pcsPatchVersionNode=pcsPatchVersionNode->m_pcNextNode )
	{
		if( pcsPatchVersionNode->m_tData->m_lVersion == lVersion )
		{
			pcsResult = pcsPatchVersionNode->m_tData;
		}
	}

	return pcsResult;
}

bool AuPackingManager::AddFolder( char *pstrFolderName )
{
	bool			bResult;

	bResult = false;

	CPackingFolder		*pcsPackingFolder;

	pcsPackingFolder = new CPackingFolder;

	if( pcsPackingFolder )
	{
		if( pstrFolderName )
		{
			size_t			lFolderNameLen;

			lFolderNameLen = strlen( pstrFolderName ) + 1;

			pcsPackingFolder->m_lFolderNameSize = lFolderNameLen;

			pcsPackingFolder->m_pstrFolderName = new char[lFolderNameLen];
			memset( pcsPackingFolder->m_pstrFolderName, 0, lFolderNameLen );
			strcat( pcsPackingFolder->m_pstrFolderName, pstrFolderName );

			if( m_csFolderInfo.AddObject( (void **)&pcsPackingFolder, _strlwr(pstrFolderName )) )
			{
				bResult = true;
			}
		}
	}

	return bResult;
}

bool AuPackingManager::AddFile( char *pstrFolderName, CPackingFile *pcsTempPackingFile, int lFileSize, unsigned long lCheckCode )
{
	bool			bResult;

	bResult = false;

	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;
	//CPackingDataNode	*pcsPackingDataNode;

	ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	if( ppcsPackingFolder && *ppcsPackingFolder && pstrFolderName )
	{
		//같은 이름이 존재하는지 본다.
		ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( _strlwr(pcsTempPackingFile->m_pstrFileName) );

		if( ppcsPackingFile )
		{
			delete (*ppcsPackingFile);
			(*ppcsPackingFolder)->m_csFilesInfo.RemoveObject( _strlwr(pcsTempPackingFile->m_pstrFileName) );
		}

		CPackingFile		*pcsPackingFile;

		pcsPackingFile = new CPackingFile;

		//@{ 2006/11/10 burumal
		//pcsPackingDataNode = new CPackingDataNode;
		//@}

		pcsPackingFile->m_bPacking = pcsTempPackingFile->m_bPacking;
		pcsPackingFile->m_lOperation = pcsTempPackingFile->m_lOperation;
		pcsPackingFile->m_lFileNameSize = pcsTempPackingFile->m_lFileNameSize;
		pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
		memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );
		strcat( pcsPackingFile->m_pstrFileName, pcsTempPackingFile->m_pstrFileName );
		pcsPackingFile->m_lFileSize = lFileSize;
		pcsPackingFile->m_lFileCheckSum = lCheckCode;

		if( (*ppcsPackingFolder)->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pcsPackingFile->m_pstrFileName) ) )
		{
			bResult = true;
		}
	}

	return bResult;
}

/*
bool AuPackingManager::AddPackingFile( char *pstrTempFolderName, char *pstrFolderName, char *pstrFileName )
{
	char			strTempFile[255];
	char			strDataFile[255];
	bool			bResult;

	bResult = false;

	CPackingFolder		**ppcsPackingFolder;

	ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	//폴더가 없다면! 만들고 다시 얻는다. <- 새로 추가되는 폴더의 경우 클라이언트에는 없으므로 추가한다.
	if( ppcsPackingFolder == NULL )
	{
        AddFolder( pstrFolderName );

		//디스크에 해당 폴더도 만든다.
		CreateFolder( pstrFolderName );

		ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );
	}

	//우선 지우고 본다.
	RemovePackingFile( pstrFolderName, pstrFileName );

	//인자 확인~
	if( ppcsPackingFolder && *ppcsPackingFolder && pstrFolderName && pstrFileName )
	{
		CPackingFile			*pcsPackingFile;

		FILE			*fpTempFile;
		FILE			*fpDataDat;
		char			strBuffer[Packing_lBlockSize];
		int				lFileSize;
		int				lBlockCount;

		sprintf( strTempFile, "%s\\%s", pstrTempFolderName, pstrFileName );
		fpTempFile = fopen( strTempFile, "rb" );

		if( fpTempFile )
		{
			fseek( fpTempFile, 0, SEEK_END );
			lFileSize = ftell( fpTempFile );

			lBlockCount = lFileSize/Packing_lBlockSize;

			if( lFileSize%Packing_lBlockSize )
			{
				lBlockCount++;
			}

			//파일 크기를 얻어냈으니 처음으로 돌린다.
			fseek( fpTempFile, 0, SEEK_SET );

			pcsPackingFile = new CPackingFile;

			//빈 블럭이 없으면 무조건 추가다!
			if( (*ppcsPackingFolder)->m_lEmptyBlockCount <= 0 )
			{
				CPackingDataNode		*pcsPackingDataNode;

				pcsPackingDataNode = new CPackingDataNode;

				sprintf( strDataFile, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );

				if( (*ppcsPackingFolder)->m_pstrBlock == NULL )
				{
					//File의 위치정보를 저장한다.
					pcsPackingDataNode->m_lStartPos = 0;
					pcsPackingDataNode->m_lSize = lFileSize;

					//새로운 블럭을 할당한다.
					(*ppcsPackingFolder)->m_pstrBlock = new char[lBlockCount];

					memset( (*ppcsPackingFolder)->m_pstrBlock, 1, lBlockCount );
					(*ppcsPackingFolder)->m_lBlockSize = lBlockCount;

					//새로 파일을 만든다.
					fpDataDat = fopen( strDataFile, "wb" );

					if( fpDataDat )
					{
						for( int lBlockCounter=0; lBlockCounter<lBlockCount; lBlockCounter++ )
						{
							memset( strBuffer, 0, Packing_lBlockSize );
							fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );

							fwrite( strBuffer, 1, Packing_lBlockSize, fpDataDat );
						}

						pcsPackingFile->m_lChildDataNodeCount = 1;
						pcsPackingFile->m_pcsDataNode = pcsPackingDataNode;

						fclose( fpDataDat );
					}
				}
				else
				{
					char		*pstrData;

					//File의 위치는 현재 마지막
					pcsPackingDataNode->m_lStartPos = ((*ppcsPackingFolder)->m_lBlockSize)*Packing_lBlockSize;
					pcsPackingDataNode->m_lSize = lFileSize;

					//원래 블럭을 지운다.
					delete [] (*ppcsPackingFolder)->m_pstrBlock;

					//블럭 크기를 증가시킨다.
					(*ppcsPackingFolder)->m_lBlockSize += lBlockCount;

					//새로운 블럭을 할당한다.
					pstrData = new char[(*ppcsPackingFolder)->m_lBlockSize];
					memset( pstrData, 1, (*ppcsPackingFolder)->m_lBlockSize );

					//새로 할당된 블럭을 세팅해준다.
					(*ppcsPackingFolder)->m_pstrBlock = pstrData;

					fpDataDat = fopen( strDataFile, "ab" );

					if( fpDataDat )
					{
						for( int lBlockCounter=0; lBlockCounter<lBlockCount; lBlockCounter++ )
						{
							memset( strBuffer, 0, Packing_lBlockSize );
							fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );

							fwrite( strBuffer, 1, Packing_lBlockSize, fpDataDat );
						}

						pcsPackingFile->m_lChildDataNodeCount = 1;
						pcsPackingFile->m_pcsDataNode = pcsPackingDataNode;

						fclose( fpDataDat );
					}
				}
			}
			//빈 블럭 있다면?
			else
			{
				int				lExtendBlockCount;
				int				lExportBlockCount;

				sprintf( strDataFile, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );

				fpDataDat = fopen( strDataFile, "rb+" );

				lExtendBlockCount = 0;
				lExportBlockCount = 0;

				if( fpDataDat )
				{
					//우선 파일이 빈 블럭에 다 들어갈수 있는지를 본다.
					if( lBlockCount > (*ppcsPackingFolder)->m_lEmptyBlockCount )
					{
						char		*pstrData;

						lExtendBlockCount = lBlockCount - (*ppcsPackingFolder)->m_lEmptyBlockCount;

						pstrData = new char[(*ppcsPackingFolder)->m_lBlockSize+lExtendBlockCount];
						memset( pstrData, 0, (*ppcsPackingFolder)->m_lBlockSize+lExtendBlockCount );

						//원래 블럭을 복사하고.
						memcpy( pstrData, (*ppcsPackingFolder)->m_pstrBlock, (*ppcsPackingFolder)->m_lBlockSize );

						//메모리 할당을 풀고
						delete [] (*ppcsPackingFolder)->m_pstrBlock;

						//붙여준다.
						(*ppcsPackingFolder)->m_pstrBlock = pstrData;

						//빈 블럭크기, 블럭크기를 전부 증가시킨다.
						(*ppcsPackingFolder)->m_lEmptyBlockCount += lExtendBlockCount;
						(*ppcsPackingFolder)->m_lBlockSize += lExtendBlockCount;
					}
					
					//빈 블럭을 보고 공간을 할당한다.
					CPackingDataNode	*pcsPrevPackingDataNode;

					int				lIndex;
					int				lStart, lSize;
					int				lAllocedBlockCount = 0;
					int				lWriteFileSize;					//실제 쓰여진 크기

					lStart = -1;
					lSize = 0;
					lWriteFileSize = 0;

					pcsPrevPackingDataNode = pcsPackingFile->m_pcsDataNode;

					for( lIndex=0; lIndex<(*ppcsPackingFolder)->m_lBlockSize; lIndex++)
					{
						if( (*ppcsPackingFolder)->m_pstrBlock[lIndex] == 0 )
						{
							lStart = lIndex;
							lSize = 1;

							lIndex++;

							for( ; lIndex<(*ppcsPackingFolder)->m_lBlockSize; lIndex++ )
							{
								if( (*ppcsPackingFolder)->m_pstrBlock[lIndex] == 0 )
								{
									lSize++;
								}
								else
								{
									//lStart, lSize세팅.
									CPackingDataNode		*pcsTempPackingDataNode;

									pcsTempPackingDataNode = new CPackingDataNode;

									//만약 앞 노드가 없다면?
									if( pcsPrevPackingDataNode == NULL )
									{
										pcsPackingFile->m_pcsDataNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}
									else
									{
										pcsPrevPackingDataNode->m_pcsNextNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}

									pcsPackingFile->m_lChildDataNodeCount++;

									//메모리 블럭을 채워준다.
									memset( &(*ppcsPackingFolder)->m_pstrBlock[lStart], 1, lSize );

									//위치 이동~
									fseek( fpDataDat, lStart*Packing_lBlockSize, SEEK_SET );

									//실제 파일을 복사한다.
									for( int lCounter=0; lCounter<lSize; lCounter++ )
									{
										memset( strBuffer, 0, Packing_lBlockSize );
										fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );
										lWriteFileSize += fwrite( strBuffer, 1,Packing_lBlockSize, fpDataDat );
									}

									//실제 파일위치와 실제크기를 기록한다.
									pcsTempPackingDataNode->m_lStartPos = lStart*Packing_lBlockSize;
									pcsTempPackingDataNode->m_lSize = lWriteFileSize;

									//쓰고난 만큼 빈 블럭을 줄여준다.
									(*ppcsPackingFolder)->m_lEmptyBlockCount -= lSize;

									lStart = -1;
									lSize = 0;
									lWriteFileSize = 0;
									
									break;
								}
							}
						}
					}

					if( lSize != -1 )
					{
						//lStart, lSize세팅.
						CPackingDataNode		*pcsTempPackingDataNode;

						pcsTempPackingDataNode = new CPackingDataNode;

						//만약 앞 노드가 없다면?
						if( pcsPrevPackingDataNode == NULL )
						{
							pcsPackingFile->m_pcsDataNode = pcsTempPackingDataNode;
							pcsPrevPackingDataNode = pcsTempPackingDataNode;
						}
						else
						{
							pcsPrevPackingDataNode->m_pcsNextNode = pcsTempPackingDataNode;
							pcsPrevPackingDataNode = pcsTempPackingDataNode;
						}

						pcsPackingFile->m_lChildDataNodeCount++;

						//메모리 블럭을 채워준다.
						memset( &(*ppcsPackingFolder)->m_pstrBlock[lStart], 1, lSize );

						//위치 이동~
						fseek( fpDataDat, lStart*Packing_lBlockSize, SEEK_SET );

						//실제 파일을 복사한다.
						for( int lCounter=0; lCounter<lSize; lCounter++ )
						{
							memset( strBuffer, 0, Packing_lBlockSize );
							fread( strBuffer, 1, Packing_lBlockSize, fpTempFile );
							lWriteFileSize += fwrite( strBuffer, 1,Packing_lBlockSize, fpDataDat );
						}

						//실제 파일위치와 실제크기를 기록한다.
						pcsTempPackingDataNode->m_lStartPos = lStart*Packing_lBlockSize;
						pcsTempPackingDataNode->m_lSize = lWriteFileSize;

						//쓰고난 만큼 빈 블럭을 줄여준다.
						(*ppcsPackingFolder)->m_lEmptyBlockCount -= lSize;
					}

					fclose( fpDataDat );
				}
			}

			//나머지 데이터 세팅
			//1.오퍼레이션
			pcsPackingFile->m_lOperation = Packing_Operation_Add;
			//2.파일크기
			pcsPackingFile->m_lFileNameSize = strlen( pstrFileName ) + 1;
			//3.파일이름
			pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
			memset( pcsPackingFile->m_pstrFileName, 0, sizeof(pcsPackingFile->m_lFileNameSize) );
			strcat( pcsPackingFile->m_pstrFileName, pstrFileName );
			//4.파일크기
			pcsPackingFile->m_lFileSize = lFileSize;
			//5.체크섬은 필요없으므로 추가하지 않는다.

			if( (*ppcsPackingFolder)->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pstrFileName) ) )
			{
				bResult = true;
			}

			fclose( fpTempFile );
		}
	}

	return bResult;
}*/

/*bool AuPackingManager::RemovePackingFile( char *pstrFolderName, char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	CPackingFolder		**ppcsPackingFolder;

	ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	//존재하는 폴더인가?
	if( ppcsPackingFolder && *ppcsPackingFolder && pstrFolderName && pstrFileName )
	{
		CPackingFile			**ppcsPackingFile;

		ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( _strlwr(pstrFileName) );

		//있다면!! 지운다~
		if( ppcsPackingFile && (*ppcsPackingFile) )
		{
			CPackingDataNode			*pstrTempNode;
			int					lStartPos;
			int					lBlockCount;

			for( pstrTempNode = (*ppcsPackingFile)->m_pcsDataNode; pstrTempNode; pstrTempNode=pstrTempNode->m_pcsNextNode )
			{
				lStartPos = pstrTempNode->m_lStartPos/Packing_lBlockSize;

				lBlockCount = pstrTempNode->m_lSize/Packing_lBlockSize;
				if( (pstrTempNode->m_lSize%Packing_lBlockSize) != 0 )
				{
					lBlockCount++;
				}

				//블럭의 영역을 지운다.
				memset( &(*ppcsPackingFolder)->m_pstrBlock[lStartPos], 0, lBlockCount );
				//지운만큼 추가한다.
                (*ppcsPackingFolder)->m_lEmptyBlockCount += lBlockCount;
			}

			//레퍼런스에서 삭제한다.
			(*ppcsPackingFile)->Reset();

			(*ppcsPackingFolder)->m_csFilesInfo.RemoveObject( _strlwr(pstrFileName) );
		}
	}

	return bResult;	
}*/


//@{ 2006/05/05 burumal
//CPackingFolder *AuPackingManager::GetFolder( char *pstrFolderName )
CPackingFolder *AuPackingManager::GetFolder( char *pstrFolderName, bool bAlreadlyLowercase )
//@}
{
	CPackingFolder** ppcsPackingFolder;

	if ( bAlreadlyLowercase )
		ppcsPackingFolder = (CPackingFolder**) m_csFolderInfo.GetObject( pstrFolderName );
	else
		ppcsPackingFolder = (CPackingFolder**) m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	if( (ppcsPackingFolder != NULL) && (*ppcsPackingFolder) != NULL )
	{
		return (*ppcsPackingFolder);
	}
	else 
	{
		return NULL;
	}
}

//@{ 2006/05/05 burumal
//CPackingFile *AuPackingManager::GetFile( char *pstrFolderName, char *pstrFileName )
CPackingFile *AuPackingManager::GetFile( char *pstrFolderName, char *pstrFileName, bool bAlreadlyLowercase  )
//@}
{
	CPackingFolder		**ppcsPackingFolder;

	if ( bAlreadlyLowercase )
		ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( pstrFolderName );
	else
		ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObject( _strlwr(pstrFolderName) );

	if( ppcsPackingFolder && (*ppcsPackingFolder) )
	{
		CPackingFile		**ppcsPackingFile;

		if ( bAlreadlyLowercase )
			ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( pstrFileName );
		else
			ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObject( _strlwr(pstrFileName) );

		if( ppcsPackingFile && (*ppcsPackingFile) )
		{
			return (*ppcsPackingFile);
		}
		else
		{
			return NULL;
		}
	}
	else 
	{
		return NULL;
	}
}

bool AuPackingManager::AddPatch( CPatchVersionInfo *pcsPatchVersionInfo )
{
	bool				bResult;

	bResult = false;

	if( m_cPatchList.Add( pcsPatchVersionInfo ) )
	{
		bResult = true;
	}

	return bResult;
}

bool AuPackingManager::BuildJZPFile( int lVersion, char *pstrExportFolder, CPatchVersionInfo *pcsPatchVersionInfo, HWND hDisplayWnd, HWND hProgressWnd )
{
	CListNode<CPatchFolderInfo *>		*pcsPatchFolderNode;
	CListNode<CPackingFile *>			*pcsPatchFileNode;

	bool			bResult;
	char			strFullFileName[256];

	bResult = false;
	
	if( pcsPatchVersionInfo )
	{
		FILE			*fpJZPFile;
		FILE			*fpResFile;

		bool			bPackingFolder;
		char			strJZPFileName[256];
		char			strResFileName[256];
		char			strBuffer[256];
		int				iFileHandle;
		int				iFileSize;
		size_t			iFileNameSize;
		int				iFolderCount;
		int				iFileCount;
		size_t			iFolderNameLen;
		int				iTotalFileCount;
		int				iProgressCurrentPostion;

		//프로그레스 바 설정을 위해 카운트를 센다.
		if( hProgressWnd )
		{
			iTotalFileCount = 0;
			iProgressCurrentPostion = 0;

			for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
			{
				iTotalFileCount += pcsPatchFolderNode->m_tData->m_clFileList.getCount();
			}

			//범위설정
			//SendMessage( hProgressWnd, PBM_SETRANGE, 0, MAKELPARAM( 0, iTotalFileCount) );
			//위치초기화
			//SendMessage( hProgressWnd, PBM_SETPOS, 0, 0 );

			::SendMessage( hProgressWnd , WM_USER+84 , 0 , MAKELPARAM( 0 , iTotalFileCount ) );
		}

		sprintf( strResFileName, "%s\\Patch%04d.JZP", pstrExportFolder, lVersion );

		fpResFile = fopen( strResFileName, "wb" );

		//버전 기록
		fwrite( &lVersion, 1, sizeof(int), fpResFile );

		//전체파일갯수는 다해서 몇개인가?
		fwrite( &iTotalFileCount, 1, sizeof(int), fpResFile );

		//폴더 갯수 기록
		iFolderCount = pcsPatchVersionInfo->m_clFolderList.getCount();
		fwrite( &iFolderCount, 1, sizeof(int), fpResFile );

		for( pcsPatchFolderNode=pcsPatchVersionInfo->m_clFolderList.GetStartNode(); pcsPatchFolderNode; pcsPatchFolderNode=pcsPatchFolderNode->m_pcNextNode )
		{
			//폴더이름 길이저장
			iFolderNameLen = strlen( pcsPatchFolderNode->m_tData->m_pstrFolderName ) + 1;
			fwrite( &iFolderNameLen, 1, sizeof(int), fpResFile );

			//폴더이름 저장
			fwrite( pcsPatchFolderNode->m_tData->m_pstrFolderName, 1, iFolderNameLen, fpResFile );

			//파일갯수기록
			iFileCount = pcsPatchFolderNode->m_tData->m_clFileList.getCount();
			fwrite( &iFileCount, 1, sizeof(int), fpResFile );

			//패킹과 관련된 파일이 올라가는지 시뮬레이션해본다.
			bPackingFolder = false;

			for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
			{
				if( pcsPatchFileNode->m_tData->m_bPacking == true )
				{
					bPackingFolder = true;
					break;
				}
			}

			//이 폴더에 대해서 패킹연산이 있는지를 저장한다.
			fwrite( &bPackingFolder, 1, sizeof(bool), fpResFile );

			for( pcsPatchFileNode = pcsPatchFolderNode->m_tData->m_clFileList.GetStartNode(); pcsPatchFileNode; pcsPatchFileNode=pcsPatchFileNode->m_pcNextNode )
			{
				if( !strcmpi( "root", pcsPatchFolderNode->m_tData->m_pstrFolderName ) )
				{
					sprintf( strFullFileName, "%s", pcsPatchFileNode->m_tData->m_pstrFileName );
				}
				else
				{
					sprintf( strFullFileName, "%s\\%s", pcsPatchFolderNode->m_tData->m_pstrFolderName, pcsPatchFileNode->m_tData->m_pstrFileName );
				}

				if( hDisplayWnd )
				{
					//디스플레이
					sprintf( strBuffer, "[Building JZP] %s 처리중", strFullFileName );
					//SetWindowText( hDisplayWnd, strBuffer );
					//::SendMessage( hDisplayWnd , LB_ADDSTRING , NULL , (LPARAM)strBuffer );
					//::SendMessage( hDisplayWnd , LB_SETCARETINDEX , (WPARAM)::SendMessage( hDisplayWnd , LB_GETCOUNT , NULL , NULL ) , TRUE );					
					iProgressCurrentPostion++;
					//::SendMessage( hProgressWnd, PBM_SETPOS, iProgressCurrentPostion, 0 );

					::SendMessage( hDisplayWnd , WM_USER+83 , (WPARAM)strBuffer , iProgressCurrentPostion );
				}

				if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Add )
				{
					//아래의 파일이름과 버전을 참조하여 특정한 파일을 만든다.
					sprintf( strJZPFileName, "ZipDirs\\%d\\%s", lVersion, strFullFileName );

					iFileNameSize = pcsPatchFileNode->m_tData->m_lFileNameSize; //Null문자포함.

					fpJZPFile = fopen( strJZPFileName, "rb" );

					if( fpJZPFile != NULL )
					{
						//파일을 읽어올 버퍼.
						char			*pstrJZPFile;

						//파일 길이를 얻어낸다.
						iFileHandle = fileno( fpJZPFile );
						iFileSize = filelength( iFileHandle );

						pstrJZPFile = new char[iFileSize];

						fread( pstrJZPFile, 1, iFileSize, fpJZPFile );

						fclose( fpJZPFile );

						//패킹인지 저장한다.
						fwrite( &pcsPatchFileNode->m_tData->m_bPacking, 1, sizeof(bool), fpResFile );

						//Res파일에쓴다.
						//0.File Operation을 쓴다.
						fwrite( &pcsPatchFileNode->m_tData->m_lOperation, 1, sizeof( pcsPatchFileNode->m_tData->m_lOperation ), fpResFile );
						//1.FileNameSize
						fwrite( &iFileNameSize, 1, sizeof(iFileNameSize), fpResFile );
						//2.FileName을 쓴다.
						fwrite( pcsPatchFileNode->m_tData->m_pstrFileName, 1, iFileNameSize, fpResFile );
						//3.FileSize
						fwrite( &iFileSize, 1, sizeof(iFileSize), fpResFile );
						//4.FileData;
						fwrite( pstrJZPFile, 1, iFileSize, fpResFile );

						delete [] pstrJZPFile;
					}
				}
				else if( pcsPatchFileNode->m_tData->m_lOperation == Packing_Operation_Remove )
				{
					iFileNameSize = pcsPatchFileNode->m_tData->m_lFileNameSize; //Null문자포함.

					//패킹인지 저장한다.
					fwrite( &pcsPatchFileNode->m_tData->m_bPacking, 1, sizeof(bool), fpResFile );

					//Res파일에쓴다.
					//0.File Operation을 쓴다.
					fwrite( &pcsPatchFileNode->m_tData->m_lOperation, 1, sizeof( pcsPatchFileNode->m_tData->m_lOperation ), fpResFile );
					//1.FileNameSize
					fwrite( &iFileNameSize, 1, sizeof(iFileNameSize), fpResFile );
					//2.FileName을 쓴다.
					fwrite( pcsPatchFileNode->m_tData->m_pstrFileName, 1, iFileNameSize, fpResFile );
				}
			}
		}

		fclose( fpResFile );

		bResult = true;
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

bool AuPackingManager::LoadResFile( char *pstrResFileName )
{
	FILE			*file;

	bool			bResult;

	bResult = false;

	if( pstrResFileName )
	{
		file = fopen( pstrResFileName, "rb" );

		if( file )
		{
			CPatchVersionInfo	*pcsPatchVersionInfo;

			int					lMaxPatchCount;
			int					lPatchCount;
			int					lMaxPatchFolderCount;
			int					lPatchFolderCount;
			int					lMaxPatchFileCount;
			int					lPatchFileCount;

			fread( &m_bBuildJZP		, 1 , sizeof(bool) , file );
			fread( &m_bBuildDat		, 1 , sizeof(bool) , file );
			fread( &m_bCopyRawFiles	, 1 , sizeof(bool) , file );

			//총 버전수~
			fread( &lMaxPatchCount	, 1 , sizeof( int ) , file );

			for( lPatchCount=0; lPatchCount<lMaxPatchCount; ++lPatchCount )
			{
				pcsPatchVersionInfo = new CPatchVersionInfo;
				m_cPatchList.Add( pcsPatchVersionInfo );

				//버전 정보를 읽는다.
				fread( &pcsPatchVersionInfo->m_lVersion, 1, sizeof(int), file );

				//폴더 갯수.
				fread( &lMaxPatchFolderCount, 1, sizeof(int), file );

				for( lPatchFolderCount=0; lPatchFolderCount<lMaxPatchFolderCount; ++lPatchFolderCount )
				{
					CPatchFolderInfo	*pcsPatchFolderInfo;

					int				lFolderNameLen;

					pcsPatchFolderInfo = new CPatchFolderInfo;
					pcsPatchVersionInfo->m_clFolderList.Add( pcsPatchFolderInfo );

					//Folder Name Length
					fread( &lFolderNameLen, 1, sizeof(int), file );

					pcsPatchFolderInfo->m_pstrFolderName = new char[lFolderNameLen];
					memset( pcsPatchFolderInfo->m_pstrFolderName, 0, lFolderNameLen );

					//Folder Name
					fread( pcsPatchFolderInfo->m_pstrFolderName, 1, lFolderNameLen, file );

					fread( &lMaxPatchFileCount, 1, sizeof(int), file );

					for(lPatchFileCount = 0; lPatchFileCount<lMaxPatchFileCount; ++lPatchFileCount )
					{
						CPackingFile			*pcsPackingFile;
						pcsPackingFile = new CPackingFile;

						pcsPatchFolderInfo->m_clFileList.Add( pcsPackingFile );

						//Operation
						fread( &pcsPackingFile->m_lOperation, 1, sizeof(int), file );
						//FileNameSize
						fread( &pcsPackingFile->m_lFileNameSize, 1, sizeof(int), file );

						pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
						memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );

						//FileName
						fread( pcsPackingFile->m_pstrFileName, 1, pcsPackingFile->m_lFileNameSize, file );
						//FileSize;
						fread( &pcsPackingFile->m_lFileSize, 1, sizeof(int), file );
						//CheckSum
						fread( &pcsPackingFile->m_lFileCheckSum, 1, sizeof(int), file );
					}
				}
			}

			//JZP 파일 정보를 읽자.
			int				iJZPFiles;

			fread( &iJZPFiles, 1, sizeof( iJZPFiles ), file );

			m_csJZPFileInfo.InitializeObject( sizeof(CJZPFileInfo *), iJZPFiles );

			for( int lCounter=0; lCounter<iJZPFiles; lCounter++ )
			{
				CJZPFileInfo	*pcJZPFileInfo;

				int				iVersion;
				int				iFileNameSize;
				char			*pstrFileName;
				int				iFileSize;
				int				iRawFileSize;
				unsigned long	iCRCCheckSum;

				if( feof( file ) )
					break;

				fread( &iVersion, 1, sizeof( iVersion ), file );
				fread( &iFileNameSize, 1, sizeof( iFileNameSize ), file );

				if( feof( file ) )
					break;

				pstrFileName = new char [iFileNameSize];
				memset( pstrFileName, 0, iFileNameSize );

				fread( pstrFileName, 1, iFileNameSize, file );
				fread( &iFileSize, 1, sizeof( iFileSize ), file );
				fread( &iRawFileSize, 1, sizeof( iRawFileSize ), file );
				fread( &iCRCCheckSum, 1, sizeof( unsigned long ), file );

				if( feof( file ) )
					break;

				pcJZPFileInfo = new CJZPFileInfo( iVersion, iFileNameSize, pstrFileName, iFileSize, iRawFileSize, iCRCCheckSum );

				m_csJZPFileInfo.AddObject( (void **)&pcJZPFileInfo, iVersion );
			}

			//버전 세팅.
			m_lCurrentVersion = GetLastVersion();

			//트리구성
			CListNode<CPatchVersionInfo *>	*pcsVersionInfo;
			CListNode<CPatchFolderInfo *>	*pcsFolderInfo;
			CListNode<CPackingFile *>		*pcsFileInfo;
			CPackingFile					*pcsPackingFile;

			//뒷 버전부터 시작한다.
			for( pcsVersionInfo = m_cPatchList.GetEndNode(); pcsVersionInfo; pcsVersionInfo=pcsVersionInfo->m_pcPrevNode )
			{
				//폴더는 정상적으로 돌린다~
				for( pcsFolderInfo = pcsVersionInfo->m_tData->m_clFolderList.GetStartNode(); pcsFolderInfo; pcsFolderInfo=pcsFolderInfo->m_pcNextNode )
				{
					AddFolder( pcsFolderInfo->m_tData->m_pstrFolderName );

					for( pcsFileInfo = pcsFolderInfo->m_tData->m_clFileList.GetStartNode(); pcsFileInfo; pcsFileInfo=pcsFileInfo->m_pcNextNode )
					{
						//이미 들어있나본다. 있으면 무시~
						pcsPackingFile = GetFile( pcsFolderInfo->m_tData->m_pstrFolderName, pcsFileInfo->m_tData->m_pstrFileName );

						//없으면 넣는다.
						if( pcsPackingFile == NULL )
						{
							AddFile( pcsFolderInfo->m_tData->m_pstrFolderName, pcsFileInfo->m_tData, pcsFileInfo->m_tData->m_lFileSize, pcsFileInfo->m_tData->m_lFileCheckSum );
						}
					}
				}
			}

			bResult = true;

			fclose( file );
		}
	}

	return bResult;
}

/*
bool AuPackingManager::BuildReferenceFile( char *pstrPackingFolder, char *pstrSourceFolder, bool bResort, HWND hDisplayWnd, HWND hProgressWnd )
{
	FILE			*file;
	bool			bResult;

	char			strCurrentDir[255];
	char			strPackingFolder[255];
	char			strDisplayBuffer[255];
	int				lFolderIndex;
	int				lFileIndex;
	int				lFileCount;
	int				lStartPos;

	lFolderIndex = 0;
	bResult = false;

	CPackingFolder		**ppcsPackingFolder;
	CPackingFile		**ppcsPackingFile;
	CPackingDataNode	*pcsPackingDataNode;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
	sprintf( strPackingFolder, "%s\\%s", strCurrentDir, pstrPackingFolder );

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		//Ref파일을 Export할지 결정한다.
		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		SetCurrentDirectory( strPackingFolder );

		//세팅이 안되면 폴더를 만들고 다시 세팅한다.
		if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
		{
			; //루트 이름로 특별한 무언가는 없다. ;;
		}
		else
		{
			if( SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName ) == FALSE )
			{
				CreateFolder( (*ppcsPackingFolder)->m_pstrFolderName );
				SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName );
			}
		}

		//패킹할 파일이 몇개인지 본다. 만약 0개면 그냥 리턴한다.
		lFileIndex = 0;
		lFileCount = 0;
		for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
		{
			if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
				continue;

			if( (*ppcsPackingFile)->m_bPacking == true )
				lFileCount++;
		}

		if( lFileCount == 0 )
			continue;

		file = fopen( Packing_Reference_FileName, "wb" );

		if( (*ppcsPackingFolder) && file )
		{
			char				lEditing;

			lEditing = AuPacking_Status_Build;
			lFileIndex = 0;
			lStartPos = 0;

			//새로 저장하는것이니 무조건 편집중이다~
			fwrite( &lEditing, 1, sizeof(bool), file );

			//폴더 이름크기
			fwrite( &(*ppcsPackingFolder)->m_lFolderNameSize, 1, sizeof(int), file );

			//폴더 이름
			fwrite( (*ppcsPackingFolder)->m_pstrFolderName, 1, (*ppcsPackingFolder)->m_lFolderNameSize, file );

			lFileCount = (*ppcsPackingFolder)->m_csFilesInfo.GetObjectCount();

			fwrite( &lFileCount, 1, sizeof(int), file );

			if( bResort == false )
			{
				//전체적으로 블럭이 몇개인가?
				fwrite( &(*ppcsPackingFolder)->m_lBlockSize, 1, sizeof(int), file );

				//블럭을 크기만큼 저장한다.
				fwrite( (*ppcsPackingFolder)->m_pstrBlock, 1, (*ppcsPackingFolder)->m_lBlockSize, file );

				//빈블럭은 몇개인가?
				fwrite( &(*ppcsPackingFolder)->m_lEmptyBlockCount, 1, sizeof(int), file );
			}
			//재정렬일때
			else //if( bResort == true )
			{
				int				lFileTotalSize;
				int				lEmptyBlockCount;
				char			*pstrTempBlock;
				int				lFileSize;

				lFileTotalSize = 0;
				lEmptyBlockCount = 0; //재정렬의 경우 빈블럭은 무조건~ 0이다.
				lFileIndex = 0;

				//파일 크기를 계산한다.
				for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
				{
					if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
						continue;

					if( (*ppcsPackingFile)->m_bPacking == false )
						continue;

					lFileSize = (*ppcsPackingFile)->m_lFileSize;

					lFileTotalSize += lFileSize/Packing_lBlockSize;

					if( (lFileSize%Packing_lBlockSize) != 0 )
					{
						lFileTotalSize++;
					}
				}

				//재정렬시에는 무조건 1(즉, 블럭에 뭔가 들어있다.)로 저장한다.
				pstrTempBlock = new char[lFileTotalSize];
				memset( pstrTempBlock, 1, lFileTotalSize );

				//전체적으로 블럭이 몇개인가?
				fwrite( &lFileTotalSize, 1, sizeof(int), file );

				//블럭을 크기만큼 저장한다.
				fwrite( pstrTempBlock, 1, lFileTotalSize, file );

				//빈블럭은 몇개인가?
				fwrite( &lEmptyBlockCount, 1, sizeof(int), file );
			}

			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
			{
				if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
					continue;

				if( (*ppcsPackingFile)->m_bPacking == false )
					continue;

				if( hDisplayWnd )
				{
					//정보 표시
					sprintf( strDisplayBuffer, "[Build Ref] %s\\%s 처리중", (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );
					SetWindowText( hDisplayWnd, strDisplayBuffer );
				}

				//파일이름크기
				fwrite( &(*ppcsPackingFile)->m_lFileNameSize, 1, sizeof(int), file );

				//파일이름
				fwrite( (*ppcsPackingFile)->m_pstrFileName, 1, (*ppcsPackingFile)->m_lFileNameSize, file );

				if( bResort == false )
				{
					//파일의 위치정보갯수
					fwrite( &(*ppcsPackingFile)->m_lChildDataNodeCount, 1, sizeof(int), file );

					pcsPackingDataNode = (*ppcsPackingFile)->m_pcsDataNode;

					for( int lChildCount=0; lChildCount<(*ppcsPackingFile)->m_lChildDataNodeCount; lChildCount++ )
					{
						if( pcsPackingDataNode == NULL )
							break;

						//시작점
						fwrite( &pcsPackingDataNode->m_lStartPos, 1, sizeof(int), file );

						//크기
						fwrite( &pcsPackingDataNode->m_lSize, 1, sizeof(int), file );

						pcsPackingDataNode = pcsPackingDataNode->m_pcsNextNode;
					}
				}
				//재정렬일때
				else //if( bResort == true )
				{
					int				lChildDataNodeCount;
					int				lFileSize;

					//재정렬일때는 ChildDataNodeCount는 무조건 0이다.
					lChildDataNodeCount = 1;
					
					fwrite( &lChildDataNodeCount, 1, sizeof(int), file );

					//시작점
					fwrite( &lStartPos, 1, sizeof(int), file );

					lFileSize = (*ppcsPackingFile)->m_lFileSize;
					//크기
					fwrite( &lFileSize, 1, sizeof(int), file );

					lStartPos += ((lFileSize/Packing_lBlockSize)*Packing_lBlockSize);

					if( (lFileSize%Packing_lBlockSize) != 0 )
					{
						lStartPos += Packing_lBlockSize;
					}
				}
			}

			fclose( file );
		}
	}

	//Ref파일 인크립트실행
	lFolderIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		//Ref파일을 Export할지 결정한다.
		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		//패킹할 파일이 몇개인지 본다. 만약 0개면 그냥 리턴한다.
		lFileIndex = 0;
		lFileCount = 0;
		for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
		{
			if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
				continue;

			if( (*ppcsPackingFile)->m_bPacking == true )
				lFileCount++;
		}

		if( lFileCount == 0 )
			continue;

		SetCurrentDirectory( strPackingFolder );

		//세팅이 안되면 폴더를 만들고 다시 세팅한다.
		if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
		{
			; //루트 이름로 특별한 무언가는 없다. ;;
		}
		else
		{
			if( SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName ) == FALSE )
			{
				CreateFolder( (*ppcsPackingFolder)->m_pstrFolderName );
				SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName );
			}
		}

		file = fopen( Packing_Reference_FileName, "rb" );

		if( file )
		{
			long				lFileSize;
			char				*pstrEncryptBuffer;
			char				lEditing;

			lEditing = AuPacking_Status_Build;

			//파일 크기를 얻는다.
			fseek( file, 0, SEEK_END );
			lFileSize = ftell(file) - 1; //가장앞에 lEditing(char)값은 인코딩하지 않는다.
			//파일크기를 얻었으니 다시 앞으로~
			fseek( file, 1, SEEK_SET );  //가장앞에 lEditing(char)값은 건너뛴다.

			pstrEncryptBuffer = new char[lFileSize];

			fread( pstrEncryptBuffer, 1, lFileSize, file );

			fclose( file );

			//Key는 "archlord"이다.
			m_cMD5Encrypt.EncryptString( "archlord", pstrEncryptBuffer, lFileSize );

			file = fopen( Packing_Reference_FileName, "wb" );
			if( file )
			{
				//가장 앞에 편집관련 코드를 저장한다.
				fwrite( &lEditing, 1, sizeof(char), file );
				fwrite( pstrEncryptBuffer, 1, lFileSize, file );

				fclose( file );
			}

			delete [] pstrEncryptBuffer;
		}
	}

	//다 끝났으면 원래 디렉토리로 돌린다.
	SetCurrentDirectory( strCurrentDir );

	return bResult;
}
*/

bool AuPackingManager::MarkPacking( char *pstrPackingFolder )
{
	if( pstrPackingFolder )
	{
		FILE			*file;
		char			strFilePath[255];

		sprintf( strFilePath, "%s\\%s", pstrPackingFolder, Packing_Reference_FileName );

		file = fopen( strFilePath, "rb+" );

		if( file )
		{
			char			lEditing;

			lEditing = AuPacking_Status_Complete;

			fwrite( &lEditing, 1, sizeof(char), file );

			fclose( file );
		}
	}

	return true;
}

bool AuPackingManager::MarkPackingComplete( char *pstrPackingFolder )
{
	CPackingFolder		**ppcsPackingFolder;
	FILE			*file;

	char			lEditing;
	char			strCurrentDir[255];
	char			strPackingFolder[255];
	int				lFolderIndex;

	lFolderIndex = 0;
	lEditing = AuPacking_Status_Complete;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
	sprintf( strPackingFolder, "%s\\%s", strCurrentDir, pstrPackingFolder );

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		//Ref파일을 Export할지 결정한다.
		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		SetCurrentDirectory( strPackingFolder );
		SetCurrentDirectory( (*ppcsPackingFolder)->m_pstrFolderName );

		//최종적으로 패킹 완료 코드를 넣어준다.
		file = fopen( Packing_Reference_FileName, "rb+" );
		if( file )
		{
			//가장 앞에 편집관련 코드를 저장한다.
			fwrite( &lEditing, 1, sizeof(char), file );
			fclose( file );
		}
	}

	//다 끝났으면 원래 디렉토리로 돌린다.
	SetCurrentDirectory( strCurrentDir );

	return true;
}

/*
bool AuPackingManager::LoadReferenceFile( LPCTSTR pstrCurrentPath, bool bReadOnly, bool bRecursiveSubDir )
{
	HANDLE				hHandle;
	WIN32_FIND_DATA		csFindData;
	char				strSearchFolder[255];
	char				strCurrentFolder[255];

	bool				bResult;

	bResult = false;

	if( pstrCurrentPath == NULL )
	{
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );
        sprintf( strSearchFolder, "%s\\*.*", strCurrentFolder );
	}
	else
	{
		sprintf( strCurrentFolder, "%s", pstrCurrentPath );
        sprintf( strSearchFolder, "%s\\*.*", pstrCurrentPath );
	}

	hHandle = FindFirstFile( strSearchFolder, &csFindData );

	while( 1 )
	{
		if( FindNextFile( hHandle, &csFindData ) == FALSE )
			break;

		//폴더인가?
		if( csFindData.dwFileAttributes == 16 )
		{
			if( !strcmpi( ".", csFindData.cFileName ) || !strcmpi( "..", csFindData.cFileName ) )
			{
				continue;
			}
			else
			{
				//하위 디렉토리도 타고 들어가서 검색할것인가?
				if( bRecursiveSubDir == true )
				{
					char			strBuffer[255];

					sprintf( strBuffer, "%s\\%s", strCurrentFolder, csFindData.cFileName );

					LoadReferenceFile( strBuffer );
				}
			}
		}
		else
		{
			int			lFileCount;

			//Reference.Dat파일이 있다면 읽는다.
			if( !strcmpi( Packing_Reference_FileName, csFindData.cFileName) )
			{
				FILE				*file;
				char				lEditing;
				char				strFullPath[255];
				char				*pstrEncryptBuffer;
				char				*pstrDeletePointer;
				int					lEncryptSize;

				sprintf( strFullPath, "%s\\%s", pstrCurrentPath, csFindData.cFileName );

				file = fopen( strFullPath, "rb" );
				
				if( file )
				{
					//파일크기를 얻는다.
					fseek( file, 0, SEEK_END );
					lEncryptSize = ftell(file) - 1;		//맨 앞에 Editing(bool)값은 패스~ 1Byte를 읽지 않는다.
					//파일 크기를 얻었으니 다시 앞으로~
					fseek( file, 0, SEEK_SET );

					pstrEncryptBuffer = new char[lEncryptSize];
					pstrDeletePointer = pstrEncryptBuffer; //지우기용으로 포인터만 복사해둔다.

					fread( &lEditing, 1, sizeof(char), file );
					fread( pstrEncryptBuffer, 1, lEncryptSize, file );

					fclose( file );

					//파일을 다 읽었으니 암호화풀기~
					if( m_cMD5Encrypt.EncryptString( "archlord", pstrEncryptBuffer, lEncryptSize ) )
					{
						CPackingFolder			*pcsPackingFolder;
						CPackingFile			*pcsPackingFile;
						CPackingDataNode		*pcsPrevPackingDataNode;

						pcsPackingFolder = new CPackingFolder;

						pcsPackingFolder->m_lEditing = lEditing;

						//폴더 이름크기
						//fread( &pcsPackingFolder->m_lFolderNameSize, 1, sizeof(int), file );
						memcpy( &pcsPackingFolder->m_lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
						pstrEncryptBuffer+=sizeof(int);

						//폴더 이름
						pcsPackingFolder->m_pstrFolderName = new char[pcsPackingFolder->m_lFolderNameSize];
						memset( pcsPackingFolder->m_pstrFolderName, 0, pcsPackingFolder->m_lFolderNameSize );
						//fread( pcsPackingFolder->m_pstrFolderName, 1, pcsPackingFolder->m_lFolderNameSize, file );
						memcpy( pcsPackingFolder->m_pstrFolderName, pstrEncryptBuffer, pcsPackingFolder->m_lFolderNameSize );
						pstrEncryptBuffer+=pcsPackingFolder->m_lFolderNameSize;

						//만약 편집중이었던 파일이었다면?
						if( pcsPackingFolder->m_lEditing == AuPacking_Status_Editing )
						{
							//깨진폴더 리스트에 추가한다.
							AddCrackedFolderName( pcsPackingFolder->m_pstrFolderName );
						}
						else
						{
							//몇개의 파일이 있는지 읽는다.
							//fread( &lFileCount, 1, sizeof(int), file );
							memcpy( &lFileCount, pstrEncryptBuffer, sizeof(int) );
							pstrEncryptBuffer+=sizeof(int);

							//전체적으로 블럭이 몇개인가?
							//fread( &pcsPackingFolder->m_lBlockSize, 1, sizeof(int), file );
							memcpy(&pcsPackingFolder->m_lBlockSize, pstrEncryptBuffer, sizeof(int) );
							pstrEncryptBuffer+=sizeof(int);

							if( pcsPackingFolder->m_lBlockSize )
							{
								//블럭을 크기만큼 저장한다.
								pcsPackingFolder->m_pstrBlock = new char[pcsPackingFolder->m_lBlockSize];
								memset( pcsPackingFolder->m_pstrBlock, 0, pcsPackingFolder->m_lBlockSize );
								//fread( pcsPackingFolder->m_pstrBlock, 1, pcsPackingFolder->m_lBlockSize, file );
								memcpy( pcsPackingFolder->m_pstrBlock, pstrEncryptBuffer, pcsPackingFolder->m_lBlockSize );
								pstrEncryptBuffer+=pcsPackingFolder->m_lBlockSize;
							}

							//빈블럭은 몇개인가?
							//fread( &pcsPackingFolder->m_lEmptyBlockCount, 1, sizeof(int), file );
							memcpy( &pcsPackingFolder->m_lEmptyBlockCount, pstrEncryptBuffer, sizeof(int) );
							pstrEncryptBuffer+=sizeof(int);

							//폴더를 추가한다.
							m_csFolderInfo.AddObject( (void **)&pcsPackingFolder, _strlwr(pcsPackingFolder->m_pstrFolderName) );

							//파일갯수 읽기
							for( int lCounter=0; lCounter<lFileCount; lCounter++ )
							{
								pcsPackingFile = new CPackingFile;

								//파일이름크기
								//fread( &pcsPackingFile->m_lFileNameSize, 1, sizeof(int), file );
								memcpy( &pcsPackingFile->m_lFileNameSize, pstrEncryptBuffer, sizeof(int) );
								pstrEncryptBuffer+=sizeof(int);

								pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
								memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );

								//파일이름
								//fread( pcsPackingFile->m_pstrFileName, 1, pcsPackingFile->m_lFileNameSize, file );
								memcpy( pcsPackingFile->m_pstrFileName, pstrEncryptBuffer, pcsPackingFile->m_lFileNameSize );
								pstrEncryptBuffer+=pcsPackingFile->m_lFileNameSize;

								//파일의 위치정보갯수
								//fread( &pcsPackingFile->m_lChildDataNodeCount, 1, sizeof(int), file );
								memcpy( &pcsPackingFile->m_lChildDataNodeCount, pstrEncryptBuffer, sizeof(int) );
								pstrEncryptBuffer+=sizeof(int);

								pcsPrevPackingDataNode = pcsPackingFile->m_pcsDataNode;

								for( int lChildNodeCounter=0; lChildNodeCounter<pcsPackingFile->m_lChildDataNodeCount; lChildNodeCounter++ )
								{
									//파일위치 정보 세팅
									CPackingDataNode		*pcsTempPackingDataNode;

									pcsTempPackingDataNode =  new CPackingDataNode;

									//fread( &pcsTempPackingDataNode->m_lStartPos, 1, sizeof(int), file );
									memcpy( &pcsTempPackingDataNode->m_lStartPos, pstrEncryptBuffer, sizeof(int) );
									pstrEncryptBuffer+=sizeof(int);
									//fread( &pcsTempPackingDataNode->m_lSize, 1, sizeof(int), file );
									memcpy( &pcsTempPackingDataNode->m_lSize, pstrEncryptBuffer, sizeof(int) );
									pstrEncryptBuffer+=sizeof(int);

									if( pcsPrevPackingDataNode == NULL )
									{
										pcsPackingFile->m_pcsDataNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}
									else
									{
										pcsPrevPackingDataNode->m_pcsNextNode = pcsTempPackingDataNode;
										pcsPrevPackingDataNode = pcsTempPackingDataNode;
									}
								}

								//파일을 추가한다.
								pcsPackingFolder->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pcsPackingFile->m_pstrFileName) );
							}
						}

						bResult = true;
					}

					delete [] pstrDeletePointer;
				}
			}
		}
	}

	FindClose( hHandle );

	return bResult;
}
*/

//@{ 2006/04/16 burumal
//bool AuPackingManager::LoadReferenceFile( LPCTSTR pstrCurrentPath, bool bReadOnly, bool bRecursiveSubDir )
bool AuPackingManager::LoadReferenceFile( LPCTSTR pstrCurrentPath, bool bReadOnly, bool bRecursiveSubDir, bool bResizing )
//@}
{
	HANDLE				hHandle;
	WIN32_FIND_DATA		csFindData;
	char				strSearchFolder[255];
	char				strCurrentFolder[255];

	bool				bResult;

	bResult = false;

	if( pstrCurrentPath == NULL )
	{
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );
        sprintf( strSearchFolder, "%s\\*.*", strCurrentFolder );
	}
	else
	{
		sprintf( strCurrentFolder, "%s", pstrCurrentPath );
        sprintf( strSearchFolder, "%s\\*.*", pstrCurrentPath );
	}

	//Reference.Dat파일이 있다면 읽는다.
	{
		char				strFullPath[255];
		sprintf( strFullPath, "%s\\%s", strCurrentFolder, Packing_Reference_FileName );
		
		//@{ 2006/04/16 burumal
		//if( ReadReferenceFile( strFullPath ) )
		if( ReadReferenceFile( strFullPath, bResizing ) )
		//@}
		{
			bResult = true;
		}
	}

	if( bRecursiveSubDir == true )
	{
		hHandle = FindFirstFile( strSearchFolder, &csFindData );
		while( 1 )
		{
			if( FindNextFile( hHandle, &csFindData ) == FALSE )
				break;

			//폴더인가?
			if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( !strcmpi( ".", csFindData.cFileName		) ||
					!strcmpi( "..", csFindData.cFileName	) ||
					!strcmpi( ".svn", csFindData.cFileName	) )
				{
					continue;
				}
				else
				{
					//하위 디렉토리도 타고 들어가서 검색할것인가?
					{
						char			strBuffer[255];

						sprintf( strBuffer, "%s\\%s", strCurrentFolder, csFindData.cFileName );

						//@{ 2006/04/16 burumal
						//LoadReferenceFile( strBuffer, bReadOnly, bRecursiveSubDir );
						LoadReferenceFile( strBuffer, bReadOnly, bRecursiveSubDir, bResizing );
						//@}
					}
				}
			}
		}
		FindClose( hHandle );
	}

	return bResult;
}

//@{ 2006/04/16 burumal
//bool AuPackingManager::ReadReferenceFile( char * pstFileName )
bool AuPackingManager::ReadReferenceFile( char * pstFileName, bool bResizing  )
//@}
{
	// Implementation of reading reference file
	// called in LoadReferenceFile function internally

	FILE				*file;
	char				*pstrEncryptBuffer;
	char				*pstrDeletePointer;
	int					lEncryptSize;
	int					lFileCount;

	bool				bResult = false;
	
	file = fopen( pstFileName, "rb" );
	
	if( file )
	{
		//파일크기를 얻는다.
		fseek( file, 0, SEEK_END );
		lEncryptSize = ftell(file);

		//파일 크기를 얻었으니 다시 앞으로~
		fseek( file, 0, SEEK_SET );

		pstrEncryptBuffer = new char[lEncryptSize];
		pstrDeletePointer = pstrEncryptBuffer; //지우기용으로 포인터만 복사해둔다.
		fread( pstrEncryptBuffer, 1, lEncryptSize, file );

		fclose( file );
		
		//파일을 다 읽었으니 암호화풀기~
		if( m_cMD5Encrypt.DecryptString( "1111", pstrEncryptBuffer, lEncryptSize ) )
		{			
			CPackingFolder*			pcsPackingFolder;
			CPackingFile*			pcsPackingFile;

			char szTempMsg[256];
			sprintf(szTempMsg, "%s %d\n", pstFileName, lEncryptSize);
			//OutputDebugStr(szTempMsg);

			pcsPackingFolder = new CPackingFolder;
			
			//몇개의 파일이 있는지 읽는다.
			//fread( &lFileCount, 1, sizeof(int), file );
			memcpy( &lFileCount, pstrEncryptBuffer, sizeof(int) );
			pstrEncryptBuffer+=sizeof(int);

			//@{ 2006/08/03 burumal
			if ( lFileCount < 0 )
			{
				delete pcsPackingFolder;
				delete [] pstrDeletePointer;
				return false;
			}

			// 제대로된 Reference.Dat 파일인지 최소한의 방법으로 파일 크기를 검사해본다
			// 현상황에서 폴더나 파일의 이름을 알수가 없으므로 최소 길이인 2로 계산했다
			if ( lEncryptSize < 
				(int) (sizeof(int) + 2 + lFileCount * (sizeof(int) + 2 + sizeof(int) + sizeof(int))) )
			{
				delete pcsPackingFolder;
				delete [] pstrDeletePointer;
				return false;
			}
			//@}

			//@{ 2006/10/19 burumal
			if ( lFileCount == 0 )
			{
				delete pcsPackingFolder;
				delete [] pstrDeletePointer;
				return true;
			}
			//@}

			if( lFileCount != 0 )
			{
				//폴더 이름크기
				//fread( &pcsPackingFolder->m_lFolderNameSize, 1, sizeof(int), file );
				memcpy( &pcsPackingFolder->m_lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);

				//폴더 이름
				//fread( pcsPackingFolder->m_pstrFolderName, 1, pcsPackingFolder->m_lFolderNameSize, file );
				pcsPackingFolder->m_pstrFolderName = new char[pcsPackingFolder->m_lFolderNameSize];
				memset( pcsPackingFolder->m_pstrFolderName, 0, pcsPackingFolder->m_lFolderNameSize );
				memcpy( pcsPackingFolder->m_pstrFolderName, pstrEncryptBuffer, pcsPackingFolder->m_lFolderNameSize );
				pstrEncryptBuffer+=pcsPackingFolder->m_lFolderNameSize;

				//@{ 2006/04/16 burumal
				long nCurFolderDataFileSize = 0;
				pcsPackingFolder->m_lDataFileSize = 0;
				//@}
				
				//폴더를 추가한다.
				m_csFolderInfo.AddObject( (void **)&pcsPackingFolder, _strlwr(pcsPackingFolder->m_pstrFolderName) );
				
				//파일갯수 읽기
				for( int lCounter=0; lCounter<lFileCount; lCounter++ )
				{
					pcsPackingFile = new CPackingFile;

					//파일이름크기
					//fread( &pcsPackingFile->m_lFileNameSize, 1, sizeof(int), file );
					memcpy( &pcsPackingFile->m_lFileNameSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);

					pcsPackingFile->m_pstrFileName = new char[pcsPackingFile->m_lFileNameSize];
					memset( pcsPackingFile->m_pstrFileName, 0, pcsPackingFile->m_lFileNameSize );

					//파일이름
					//fread( pcsPackingFile->m_pstrFileName, 1, pcsPackingFile->m_lFileNameSize, file );
					memcpy( pcsPackingFile->m_pstrFileName, pstrEncryptBuffer, pcsPackingFile->m_lFileNameSize );
					pstrEncryptBuffer+=pcsPackingFile->m_lFileNameSize;

					//파일위치 정보 세팅
					CPackingDataNode		*pcsPackingDataNode;

					pcsPackingDataNode =  new CPackingDataNode;

					//fread( &pcsPackingDataNode->m_lStartPos, 1, sizeof(int), file );
					memcpy( &pcsPackingDataNode->m_lStartPos, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &pcsPackingDataNode->m_lSize, 1, sizeof(int), file );
					memcpy( &pcsPackingDataNode->m_lSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);

					//@{ 2006/04/16 burumal
					if ( (pcsPackingDataNode->m_lStartPos + pcsPackingDataNode->m_lSize) > nCurFolderDataFileSize )
						nCurFolderDataFileSize = (pcsPackingDataNode->m_lStartPos + pcsPackingDataNode->m_lSize);
					//@}

					pcsPackingFile->m_pcsDataNode = pcsPackingDataNode;

					//파일을 추가한다.
					pcsPackingFolder->m_csFilesInfo.AddObject( (void **)&pcsPackingFile, _strlwr(pcsPackingFile->m_pstrFileName) );
				}

				//@{ 2006/04/16 burumal
				pcsPackingFolder->m_lDataFileSize = nCurFolderDataFileSize;

				if ( bResizing )
					ResizeDatFile(pcsPackingFolder);
				//@}				
			}

			bResult = true;
		}

		delete [] pstrDeletePointer;
	}

	return bResult;
}


void AuPackingManager::SetFilePointer(bool bLoadSubDir)
{
	//Ref파일을 읽었으니 Data.Dat파일의 파일 포인터를 세팅하자!!
	CPackingFolder			**ppcsPackingFolder;

	char				strDataFilePath[255];
	int					lIndex;

	lIndex = 0;

	char	strDir[255];
	GetCurrentDirectory( 255, strDir );

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ) )
	{
		if( (*ppcsPackingFolder) )
		{
			if( !stricmp( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
			{
				sprintf( strDataFilePath, "%s", Packing_Data_FileName );
			}
			else
			{
				if( bLoadSubDir )
				{
                    sprintf( strDataFilePath, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );
				}
				else
				{
                    sprintf( strDataFilePath, "%s", Packing_Data_FileName );
				}
			}

			//Read, Write, Append 모두 가능하게 연다.
			if( m_bReadOnlyMode )
			{
				(*ppcsPackingFolder)->m_fpFile = fopen( strDataFilePath, "rb" );
			}
			else
			{
				(*ppcsPackingFolder)->m_fpFile = fopen( strDataFilePath, "rb+" );
			}
			//어차피 세팅이 안되면 NULL리턴이기에 그냥 열고 땡이다. 특별한 처리는 없다. 
		}
	}
}

void AuPackingManager::CloseAllFileHandle()
{
	CPackingFolder			**ppcsPackingFolder;

	int					lIndex;

	lIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lIndex ) )
	{
		if( (*ppcsPackingFolder) )
		{
			if( (*ppcsPackingFolder)->m_fpFile != NULL )
			{
				fclose( (*ppcsPackingFolder)->m_fpFile );
				(*ppcsPackingFolder)->m_fpFile = NULL;
			}			
		}
	}
}

bool AuPackingManager::BuildDatFile( char *pstrPackingDir, char *pstrRawDir, int lVersion, HWND hDisplayhWnd, HWND hProgressWnd )
{
	FILE			*datfile;
	FILE			*readfile;
	CPackingFolder	**ppcsPackingFolder;
	CPackingFile	**ppcsPackingFile;

	bool			bResult;
	char			strCurrentDir[255];
	char			strDataFileName[255];
	char			strSourceFileName[255];
	int				lFolderIndex;
	int				lFileIndex;
	int				lRealFileCount;
	int				lTotalFileCount;
	int				lProgressCurrentPostion;

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir ); 

	lFolderIndex = 0;

	if( hProgressWnd )
	{
		lTotalFileCount = 0;
		lProgressCurrentPostion = 0;

		for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
		{
			if( ppcsPackingFolder && (*ppcsPackingFolder) )
			{
				//바뀐경우만 새로 빌드한다.
				if( (*ppcsPackingFolder)->m_bExportToRef == true )
                    lTotalFileCount += (*ppcsPackingFolder)->m_csFilesInfo.GetObjectCount();
			}
		}

		//범위설정
		SendMessage( hProgressWnd, PBM_SETRANGE, 0, MAKELPARAM( 0, lTotalFileCount) );
		//위치초기화
		SendMessage( hProgressWnd, PBM_SETPOS, 0, 0 );
	}

	lFolderIndex = 0;

	for( ppcsPackingFolder = (CPackingFolder **)m_csFolderInfo.GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder	**)m_csFolderInfo.GetObjectSequence( &lFolderIndex ) )
	{
		if( (*ppcsPackingFolder) == NULL )
			continue;

		if( (*ppcsPackingFolder)->m_bExportToRef == false )
			continue;

		//폴더 이동후 파일을 연다.
		if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName) )
		{
            sprintf( strDataFileName, "%s\\%s\\%s", strCurrentDir, pstrPackingDir, Packing_Data_FileName );
		}
		else
		{
            sprintf( strDataFileName, "%s\\%s\\%s\\%s", strCurrentDir, pstrPackingDir, (*ppcsPackingFolder)->m_pstrFolderName, Packing_Data_FileName );
		}

		//실제로 저장한 파일이 몇개인지 본다.
		lFileIndex = 0;
		lRealFileCount = 0;
		for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
		{
			if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
				continue;

			if( (*ppcsPackingFile)->m_bPacking == true )
				lRealFileCount++;
		}

		if( lRealFileCount == 0 )
			continue;

		datfile = fopen( strDataFileName, "wb" );

		if( datfile )
		{			
			char			strBuffer[Packing_lBlockSize];
			char			strDisplayBuffer[256];
			size_t			lReadBytes;

			lFileIndex = 0;

			for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence( &lFileIndex ) )
			{
				if( (*ppcsPackingFile)->m_lOperation == Packing_Operation_Remove )
					continue;

				if( (*ppcsPackingFile)->m_bPacking == false )
					continue;

				if( !strcmpi( "root", (*ppcsPackingFolder)->m_pstrFolderName) )
				{
					sprintf( strSourceFileName, "%s\\%s", pstrRawDir, (*ppcsPackingFile)->m_pstrFileName );
				}
				else
				{
					sprintf( strSourceFileName, "%s\\%s\\%s", pstrRawDir, (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );
				}

				if( hDisplayhWnd != NULL )
				{
					//디스플레이.
					sprintf( strDisplayBuffer, "[Build Dat] %s 처리중...", strSourceFileName );
					SetWindowText( hDisplayhWnd, strDisplayBuffer );

					lProgressCurrentPostion++;
					SendMessage( hProgressWnd, PBM_SETPOS, lProgressCurrentPostion, 0 );
				}

				readfile = fopen( strSourceFileName, "rb" );

				if( readfile )
				{
					while( 1 )
					{
						//버퍼 초기화
						memset( strBuffer, 0, Packing_lBlockSize );

						//파일을 저장한다.
						lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, readfile );

						if( 0 < lReadBytes )
						{
							//4k씩 읽어서 Data.Dat에 저장한다. 
							fwrite( strBuffer, 1, Packing_lBlockSize, datfile );
						}

						//4k이하를 읽었다면 끝낸다.
						if( lReadBytes < Packing_lBlockSize )
						{
							fclose( readfile );
							break;
						}
					}
				}
			}

			fclose( datfile ); 
		}
	}

	return bResult;
}

/*
bool AuPackingManager::UpdateDatAndRefFileFromJZP( char *pstrTempFolderName, char *pstrJzpFileName )
{
	FILE			*file;
	bool			bResult;
	bool			bPacking;
	char			strCurrentDir[255];
	char			strTempDir[255];
	char			strJZPFileName[255];
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;
	int				lMaxFileCount;
	int				lTotalFileCount;
	int				lFileCount;

	int				lFolderNameLen;
	char			*pstrFolderName;

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDir), strCurrentDir );
    sprintf( strTempDir, "%s\\%s", strCurrentDir, pstrTempFolderName );
	sprintf( strJZPFileName, "%s\\%s\\%s", strCurrentDir, pstrTempFolderName, pstrJzpFileName );

	if( pstrJzpFileName )
	{
		file = fopen( strJZPFileName, "rb" );

		if( file )
		{
			//버전
			fread( &lVersion, 1, sizeof(int), file );

			fread( &lTotalFileCount, 1, sizeof(int), file );

			//폴더 갯수
			fread( &lMaxFolderCount, 1, sizeof(int), file );

			for( lFolderCount=0; lFolderCount<lMaxFolderCount; lFolderCount++ )
			{
				//폴더이름 길이로드
				fread( &lFolderNameLen, 1, sizeof(int), file );

				//폴더이름 로드
				pstrFolderName = new char[lFolderNameLen];
				memset( pstrFolderName, 0, lFolderNameLen );
				fread( pstrFolderName, 1, lFolderNameLen, file );

				//편집중이라고 세팅한다.
				MarkPacking( pstrFolderName );

				//파일갯수기록
				fread( &lMaxFileCount, 1, sizeof(int), file );

				for( lFileCount=0; lFileCount<lMaxFileCount ;lFileCount++ )
				{
					int				lOperation;
					int				lFileNameLength;
					int				lFileSize;
					char			*pstrFileName;
					char			*pstrBuffer;

					pstrFileName = NULL;
					pstrBuffer = NULL;

					//Packing
					fread( &bPacking, 1, sizeof(bool), file );

					//Operation
					fread( &lOperation, 1, sizeof(int), file );

					//FileNameLength
					fread( &lFileNameLength, 1, sizeof(int), file );

					//FileName
					pstrFileName = new char[lFileNameLength];
					memset( pstrFileName, 0, lFileNameLength );
					fread(pstrFileName, 1, lFileNameLength, file );

					if( lOperation == Packing_Operation_Add )
					{
						//FileSize
						fread( &lFileSize, 1, sizeof(int), file );

						pstrBuffer = new char[lFileSize];

						//FileData
						fread( pstrBuffer, 1, lFileSize, file );

						//Packing이면 Data.Dat에 추가한다.
						if( bPacking == true )
						{
							//우선 읽었으니 풀어낸다.
							m_cCompress.decompressMemory( pstrBuffer, lFileSize, strTempDir, pstrFileName );

							//파일을 풀었으니~ Data.Dat파일에 추가하자~
							AddPackingFile( pstrTempFolderName, pstrFolderName, pstrFileName );
						}
						//Packing이 아니면 해당 폴더에 그냥 파일을 생성한다.
						else
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, "", pstrFileName );
							}
							else
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrFolderName, pstrFileName );
							}
						}

						//추가를 다했으면 파일을 지운다. Temp폴더는 자동으로 지워지니 안지워도 되려나? 쩝.
					}
					else
					{
						//Data.Dat 파일에서 내용을 지운다.
						RemovePackingFile( pstrFolderName, pstrFileName );
					}

					if( pstrFileName != NULL )
						delete [] pstrFileName;
					if( pstrBuffer != NULL )
						delete [] pstrBuffer;
				}

				//해당폴더의 Refrence 파일을 다시 빌드하도록 세팅한다.
				CPackingFolder			*pcsTempPackingFolder;
				pcsTempPackingFolder = GetFolder( pstrFolderName );

				if( pcsTempPackingFolder )
				{
					pcsTempPackingFolder->m_bExportToRef = true;
				}
			}
		}
	}

	return bResult;
}
*/

bool AuPackingManager::GetFolderName( const char *pstrFullPath, const char *pstrSourceFolder, char *pstrBuffer, int lBufferSize )
{
	bool			bResult;

	bResult = false;

	if( pstrFullPath && pstrSourceFolder && pstrBuffer )
	{
		size_t		lRootSize;
		size_t		lFullPathSize;

		memset( pstrBuffer, 0, lBufferSize );
		lRootSize = strlen( pstrSourceFolder ) + 1; // "\\", 즉 역슬래쉬를 지운다.
		lFullPathSize = strlen( pstrFullPath );
		
		if( lRootSize >= lFullPathSize )
		{
			; //그냥 넘어간다.
		}
		else
		{
			memcpy( pstrBuffer, &pstrFullPath[lRootSize], lFullPathSize - lRootSize );
		}

		bResult = true;
	}

	return bResult;
}

bool AuPackingManager::CreateFolder( char *pstrFolderName )
{
	bool			bResult;
	char			strFolderName[255];

	bResult = false;

	if( pstrFolderName )
	{
		int			lFolderNameLen;

		lFolderNameLen = 0;

		memset( strFolderName, 0, 255 );

		for( int lCounter=0; lCounter<255; lCounter++ )
		{
			if( pstrFolderName[lCounter] == 0 )
				break;

			if( pstrFolderName[lCounter] == '\\' )
			{
				CreateDirectory( strFolderName, NULL );
			}

			memcpy( &strFolderName[lFolderNameLen], &pstrFolderName[lCounter], 1 );
			lFolderNameLen++;
		}

		if( lFolderNameLen != 0 )
		{
			CreateDirectory( strFolderName, NULL );
		}
	}

	return bResult;
}

CJZPFileInfo *AuPackingManager::GetJZPInfo( int iVersion )
{
	CJZPFileInfo		**ppcsJZPFileInfo;

	ppcsJZPFileInfo = (CJZPFileInfo **)m_csJZPFileInfo.GetObject( iVersion );

	if( ppcsJZPFileInfo && (*ppcsJZPFileInfo) )
	{
		return (*ppcsJZPFileInfo);
	}

	return NULL;
}

int AuPackingManager::GetFileSize( CPackingFile *pcsPackingFile )
{
	int				lFileSize;

	lFileSize = 0;

	CPackingDataNode		*pcsPackingDataNode;

	if( pcsPackingFile )
	{
		pcsPackingDataNode = pcsPackingFile->m_pcsDataNode;

		if( pcsPackingDataNode )
		{
			lFileSize = pcsPackingDataNode->m_lSize;
		}
	}

	return lFileSize;
}

int AuPackingManager::GetFileSize( ApdFile *pcsApdFile )
{
	CPackingFile			*pcsPackingFile;
	int						lFileSize;

	lFileSize = 0;

	if( pcsApdFile )
	{
		//패킹파일 정보가 있다면 패킹에서 찾아본다.
		if( pcsApdFile->m_pcsPackingFolder && pcsApdFile->m_pcsPackingFile )
		{
			pcsPackingFile = GetFile( pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName );

			if( pcsPackingFile )
			{
                lFileSize = GetFileSize( pcsPackingFile );
			}
		}
		//아니라면 직접 읽어본다.
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				FILE			*file;
				char			strFileName[255];

				sprintf( strFileName, "%s\\%s", pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName );

				file = fopen( strFileName, "rb" );

				if( file )
				{
					fseek(file, 0, SEEK_END );
					lFileSize = ftell( file );
					fclose(file);
				}
			}
		}
	}

	return lFileSize;
}

bool AuPackingManager::OpenFile( char *pstrFilePath, ApdFile *pcsApdFile )
{
    if( pstrFilePath && pcsApdFile )
	{
		char			strFolderName[255];
		char			strFileName[255];
		char			strFileExt[255];
		char			strFileFullName[255];
		size_t			lFolderNameLen;

		_splitpath( pstrFilePath, NULL, strFolderName, strFileName, strFileExt );
		sprintf( strFileFullName, "%s%s", strFileName, strFileExt );
		
		//폴더 네임뒤에 붙은 "\"를 지운다.
		lFolderNameLen = strlen(strFolderName);

		if( lFolderNameLen == 0 )
		{
			sprintf( strFolderName, "%s", "root" );
		}
		else
		{
			//@{ 2006/05/05 burumal
			//memset( &strFolderName[lFolderNameLen-1], 0, 1 );
			strFolderName[lFolderNameLen-1] = NULL;
			//@}
		}
		
		//@{ 2006/05/05 burumal
		//return OpenFile( strFolderName, strFileFullName, pcsApdFile );
		return OpenFile( strFolderName, strFileFullName, pcsApdFile, pstrFilePath );
		//@}
	}
	else
	{
		return 0;
	}
}

//@{ 2006/05/05 burumal
//bool AuPackingManager::OpenFile( char *pstrFolderName, char *pstrFileName, ApdFile *pcsApdFile )
bool AuPackingManager::OpenFile( char *pstrFolderName, char *pstrFileName, ApdFile *pcsApdFile, char* pstrFullFilePath )
//@}
{
	CPackingFolder		*pcsPackingFolder;
	CPackingFile		*pcsPackingFile;

	bool				bResult;

	bResult = false;

	if( pstrFolderName && pstrFileName && pcsApdFile )
	{
		memset( pcsApdFile->m_strFolderName, 0, sizeof( CHAR ) * 255 );
		memset( pcsApdFile->m_strFileName, 0, sizeof( CHAR ) * 255 );

		strncpy( pcsApdFile->m_strFolderName	, pstrFolderName	, 255	);
		strncpy( pcsApdFile->m_strFileName		, pstrFileName		, 255	);

		//읽어들인 폴더, 파일이름을 소문자로 바꾼다.
		_strlwr(pcsApdFile->m_strFolderName);
		_strlwr(pcsApdFile->m_strFileName);

		//@{ 2006/05/05 burumal
		//pcsPackingFolder = GetFolder( pcsApdFile->m_strFolderName );
		pcsPackingFolder = GetFolder( pcsApdFile->m_strFolderName, true );
		//@}

		//우선 패킹에서 읽어본다.
		if( pcsPackingFolder )
		{
			//@{ 2006/05/05 burumal
			//pcsPackingFile = GetFile( pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName );
			pcsPackingFile = GetFile( pcsApdFile->m_strFolderName, pcsApdFile->m_strFileName, true );
			//@}

			if( pcsPackingFile )
			{
				if( pcsPackingFile->m_pcsDataNode )
				{
					int				lStartPoint;

					//락 작업 들어갑니다~ 갑니다~ 자~ 빠져봅시다~
					Lock( pcsApdFile );

					lStartPoint = pcsPackingFile->m_pcsDataNode->m_lStartPos;
					pcsApdFile->m_lFilePosition = lStartPoint;

					pcsApdFile->m_pcsPackingFolder = pcsPackingFolder;
					pcsApdFile->m_pcsPackingFile = pcsPackingFile;

					//시작 위치로 이동시켜놓는다.
					if( pcsPackingFolder->m_fpFile )
					{
						fseek( pcsPackingFolder->m_fpFile, lStartPoint, SEEK_SET );

						bResult = true;
					}
				}
			}
		}
		//패킹파일이 아니라면 직접 찾아본다.
		else
		{
			//@{ 2006/05/05 burumal
			/*
			FILE			*file;
			char			strFilePath[255];

			sprintf( strFilePath, "%s\\%s", pstrFolderName, pstrFileName );

			file = fopen( strFilePath, "rb" );

			if( file )
			{
				pcsApdFile->m_phDirectAccess = file;
				pcsApdFile->m_bLoadFromPackingFile = false;
				bResult = true;
			}
			*/

			FILE			*file;

			file = fopen( pstrFullFilePath, "rb" );

			if( file )
			{
				pcsApdFile->m_phDirectAccess = file;
				pcsApdFile->m_bLoadFromPackingFile = false;
				bResult = true;
			}
			//@}
		}
	}	

	return bResult;
}

int AuPackingManager::ReadFile( void *pstrBuffer, int iBufferLen, ApdFile *pcsApdFile )
{
	size_t			lReadBytes;

	lReadBytes = 0;

	if( pstrBuffer && iBufferLen > 0 && pcsApdFile )
	{
		CPackingFolder		*pcsPackingFolder;

		pcsPackingFolder = GetFolder( pcsApdFile->m_strFolderName );

		//우선 패킹에서 읽어본다.
		if( pcsPackingFolder )
		{
			int				lStartIndex;
			int				lCompiledPos;
			size_t			lCurrentPosition;
			size_t			lRealReadBytes;

			lStartIndex = 0;
			lCompiledPos = 0;
			lCurrentPosition = pcsApdFile->m_lCurrentPosition;			
			
			lRealReadBytes = fread( &(((char *)pstrBuffer)[lReadBytes]), 1, iBufferLen, pcsPackingFolder->m_fpFile );

			lReadBytes += lRealReadBytes;
			pcsApdFile->m_lCurrentPosition += lRealReadBytes;			
		}
		//패킹파일이 아니라면 직접 찾아본다.
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				lReadBytes = fread( pstrBuffer, 1, iBufferLen, pcsApdFile->m_phDirectAccess );
			}
		}
	}

	return (int)lReadBytes;
}

bool AuPackingManager::SetPos( int lPos, ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( lPos >= 0 && pcsApdFile )
	{
		//우선 패킹 정보가 있는가?
		if( pcsApdFile->m_pcsPackingFolder && pcsApdFile->m_pcsPackingFile )
		{
			if( pcsApdFile->m_pcsPackingFile->m_pcsDataNode )
			{
				CPackingDataNode	*pcsPackingDataNode;

				int					lCompiledCount;

				lCompiledCount = 0;
				pcsPackingDataNode = pcsApdFile->m_pcsPackingFile->m_pcsDataNode;

				//fseek은 성공하면 0을 리턴한다.
				if( fseek(	pcsApdFile->m_pcsPackingFolder->m_fpFile,
							pcsApdFile->m_pcsPackingFile->m_pcsDataNode->m_lStartPos + lPos,
							SEEK_SET )  == 0 )
				{
					pcsApdFile->m_lCurrentPosition = lPos;
					bResult = true;
				}
			}
		}
		//없다면 그냥 읽어낸다.
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				if( fseek( pcsApdFile->m_phDirectAccess, lPos, SEEK_SET ) == 0 )
				{
					bResult = true;                
				}
			}
		}
	}

	return bResult;
}

bool AuPackingManager::CloseFile( ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( pcsApdFile )
	{
		//우선 패킹 정보가 있는가?
		if( pcsApdFile->m_pcsPackingFolder && pcsApdFile->m_pcsPackingFile )
		{
			bResult = Unlock( pcsApdFile );
		}
		else
		{
			if( pcsApdFile->m_bLoadFromPackingFile == false )
			{
				if( fclose( pcsApdFile->m_phDirectAccess ) == 0 )
				{
					bResult = true;
				}
			}
		}
	}

	return bResult;
}

bool AuPackingManager::Lock( ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( pcsApdFile )
	{
		if( pcsApdFile->m_pcsPackingFolder )
		{
			pcsApdFile->m_pcsPackingFolder->m_Mutex.WLock();
		}
	}

	return bResult;
}

bool AuPackingManager::Unlock( ApdFile *pcsApdFile )
{
	bool			bResult;

	bResult = false;

	if( pcsApdFile )
	{
		if( pcsApdFile->m_pcsPackingFolder )
		{
			pcsApdFile->m_pcsPackingFolder->m_Mutex.Release();
		}
	}

	return bResult;
}

//. 2006. 4. 25. nonstopdj
void AuPackingManager::CreateEmptyFile( const char* pstrFullPath )
{
	FILE*			fpTemp = NULL;
	HANDLE			handle;

	//. check file exist.
	fpTemp = fopen(pstrFullPath, "wb");
	if(!fpTemp)
	{
		//. if original file is not exist then create file.
		handle = CreateFile(pstrFullPath, FILE_ALL_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(handle);
	}
	else
		fclose(fpTemp);
}


bool AuPackingManager::ReadyPacking( char *pstrFolder )
{
	bool			bResult;

	bResult = false;

	char			strDataFilePath[255];
	char			strBackupDataFilePath[255];
	char			strRefrenceFilePath[255];
	char			strBackupRefrenceFilePath[255];

	if( !stricmp( pstrFolder, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );		
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );

		sprintf( strRefrenceFilePath, "%s", Packing_Reference_FileName );		
		sprintf( strBackupRefrenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );		
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );

		sprintf( strRefrenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );		
		sprintf( strBackupRefrenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );

		//폴더를 만들어준다.
		CreateFolder( pstrFolder );
	}

	//Data, Ref 파일을 백업한다.
	int nTry;
	for ( nTry = 0; nTry < 3; nTry++ )
		if ( MoveFile( strDataFilePath, strBackupDataFilePath ) != FALSE )
			break;

	for ( nTry = 0; nTry < 3; nTry++ )
		if ( MoveFile( strRefrenceFilePath, strBackupRefrenceFilePath ) != FALSE )
			break;

	//Ref파일을 생성한다. 갯수는 0개로.
	FILE				*file;

	file = fopen( strRefrenceFilePath, "wb" );

	if( file )
	{
		int				lFileCount;
		size_t			lFolderNameLen;

		lFileCount = 0;

		fwrite( &lFileCount, 1, sizeof(int), file );

		lFolderNameLen = strlen( pstrFolder ) + 1;

		if( lFolderNameLen == 1 )
		{
			char			strBuffer[5] = "root";

			lFolderNameLen = 5;

			fwrite( &lFolderNameLen, 1, sizeof(int), file );
			fwrite( strBuffer, 1, lFolderNameLen, file );
		}
		else
		{
			fwrite( &lFolderNameLen, 1, sizeof(int), file );
			fwrite( pstrFolder, 1, lFolderNameLen, file );
		}

		fclose( file );
		bResult = true;
	}

	return bResult;
}

/*
bool AuPackingManager::CompletePacking( char *pstrFolder, int lCurrentPosition, int lPackedFileCount, ApAdmin *pcsAdminAdd, ApAdmin *pcsAdminRemove )
{
	FILE			*fpDat, *fpRef;
	FILE			*fpBackupDat, *fpBackupRef;

	bool			bResult;
	bool			bEmptyDat, bEmptyRef;
	char			strDataFilePath[255];
	char			strBackupDataFilePath[255];
	char			strReferenceFilePath[255];
	char			strBackupReferenceFilePath[255];

	bEmptyDat = false;
	bEmptyRef = false;

	if(  !stricmp( pstrFolder, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );
	}

	bResult = false;

	fpDat = fopen( strDataFilePath, "ab" );
	fpBackupDat = fopen( strBackupDataFilePath, "rb" );

	fpRef = fopen( strReferenceFilePath, "ab" );
	fpBackupRef = fopen( strBackupReferenceFilePath, "rb" );

	if( (fpDat!=NULL) && (fpRef!=NULL) )
	{
		if( (fpBackupDat!=NULL) && (fpBackupRef!=NULL) )
		{
			char				*pstrEncryptBuffer;
			char				*pstrDeletePointer;
			int					lEncryptSize;

			//파일크기를 얻는다.
			fseek( fpBackupRef, 0, SEEK_END );
			lEncryptSize = ftell(fpBackupRef);

			//파일 크기를 얻었으니 다시 앞으로~
			fseek( fpBackupRef, 0, SEEK_SET );

			pstrEncryptBuffer = new char[lEncryptSize];
			pstrDeletePointer = pstrEncryptBuffer; //지우기용으로 포인터만 복사해둔다.

			fread( pstrEncryptBuffer, 1, lEncryptSize, fpBackupRef );
			fclose( fpBackupRef );

			if( m_cMD5Encrypt.DecryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lEncryptSize ) )
			{
				//백업된 RefrenceFile을 읽으며 데이터를 추가한다.
				int					lFolderNameSize;
				int					lRefFileCount;
				char				*pstrFolderName;

				//파일이 몇개냐?
				memcpy( &lRefFileCount, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lRefFileCount, 1, sizeof(int), fpBackupRef );

				//폴더 이름크기
				memcpy( &lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lFolderNameSize, 1, sizeof(int), fpBackupRef );

				//폴더 이름
				pstrFolderName = new char[lFolderNameSize];
				memset( pstrFolderName, 0, lFolderNameSize );
				memcpy( pstrFolderName, pstrEncryptBuffer, lFolderNameSize );
				pstrEncryptBuffer+=lFolderNameSize;
				//fread( pstrFolderName, 1, lFolderNameSize, fpBackupRef );

				for( int lCounter=0; lCounter<lRefFileCount; lCounter++ )
				{
					size_t				lFileNameSize;
					int				lStartPos, lFileSize;
					char			*pstrFileName;

					//파일이름 크기 복사.
					memcpy( &lFileNameSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileNameSize, 1, sizeof(int), fpBackupRef );

					pstrFileName = new char[lFileNameSize];
					memset( pstrFileName, 0, lFileNameSize );

					//파일이름 복사
					memcpy( pstrFileName, pstrEncryptBuffer, lFileNameSize );
					pstrEncryptBuffer+=lFileNameSize;
					//fread( pstrFileName, 1, lFileNameSize, fpBackupRef );

					//시작위치 복사
					memcpy( &lStartPos, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lStartPos, 1, sizeof(int), fpBackupRef );

					//파일크기 복사
					memcpy( &lFileSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileSize, 1, sizeof(int), fpBackupRef );

					if( pstrFileName )
					{
						bool			bSkip;

						bSkip = false;

						//만약 패킹된 파일이라면 예전파일은 필요없으므로 스킵~
						if( pcsAdminAdd->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//만약 지우게될 파일이면 필요없으므로 스킵~
						if( pcsAdminRemove->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//스킵될 파일이 아니면 이어서 쓴다.
						if( bSkip == false )
						{
							size_t			lRestSize;
							size_t			lReadBytes;
							char			strBuffer[Packing_lBlockSize];

							//Refrence파일을 편집한다.
							lFileNameSize = strlen( pstrFileName ) + 1;

							fwrite( &lFileNameSize, 1, sizeof(int), fpRef );
							fwrite( pstrFileName, 1, lFileNameSize, fpRef );
							fwrite( &lCurrentPosition, 1, sizeof(int), fpRef );
							fwrite( &lFileSize, 1, sizeof(int), fpRef );

							//복사할 파일의 위치로 이동한다.
							fseek( fpBackupDat, lStartPos, SEEK_SET );

							lRestSize = lFileSize;

							//Data.Dat로 파일을 복사한다.
							while( 1 )
							{
								//버퍼 초기화
								memset( strBuffer, 0, Packing_lBlockSize );

								if( lRestSize < Packing_lBlockSize )
								{
									lReadBytes = fread( strBuffer, 1, lRestSize, fpBackupDat );
									fwrite( strBuffer, 1, lReadBytes, fpDat );
									break;
								}
								else
								{
									lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, fpBackupDat );

									if( 0 < lReadBytes )
									{
										//4k씩 읽어서 Data.Dat에 저장한다. 
										fwrite( strBuffer, 1, Packing_lBlockSize, fpDat );
									}

									//4k이하를 읽었다면 끝낸다.
									if( lReadBytes < Packing_lBlockSize )
									{
										break;
									}

									lRestSize -= lReadBytes;

									if( lRestSize <= 0 )
									{
										break;
									}
								}
							}

							lCurrentPosition += lFileSize;
							lPackedFileCount++;
						}
					}

					delete [] pstrFileName;
				}

				delete [] pstrFolderName;
			}

			delete [] pstrDeletePointer;

			fclose( fpBackupDat );
			fclose( fpBackupRef );
		}

		//파일크기를 얻어낸다.
		fseek( fpDat, 0, SEEK_END );
		fseek( fpRef, 0, SEEK_END );

		if( ftell( fpDat ) == 0 )
			bEmptyDat = true;
		if( ftell( fpRef ) == 0  )
			bEmptyRef = true;

		fclose( fpDat );
		fclose( fpRef );

		//백업 파일은 삭제하고 Refrence파일은 암호화한다.
		DeleteFile( strBackupDataFilePath );
		DeleteFile( strBackupReferenceFilePath );

		if( (bEmptyDat == true) || (bEmptyRef == true) )
		{
			DeleteFile( strDataFilePath );
			DeleteFile( strReferenceFilePath );
		}
		else
		{
			//최종적으로 fpRef에 lPackedFileCount를 수정해준다.
			fpRef = fopen( strReferenceFilePath, "rb+" );
			if( fpRef )
			{
				fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );
				fclose( fpRef );
			}

			//암호화한다.
			fpRef = fopen( strReferenceFilePath, "rb" );
			if( fpRef )
			{
				long				lFileSize;
				char				*pstrEncryptBuffer;

				//파일 크기를 얻는다.
				fseek( fpRef, 0, SEEK_END );
				lFileSize = ftell(fpRef);
				//파일크기를 얻었으니 다시 앞으로~
				fseek( fpRef, 0, SEEK_SET ); 

				pstrEncryptBuffer = new char[lFileSize];

				fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

				fclose( fpRef );

				//Key는 "archlord"이다.
				if( m_cMD5Encrypt.EncryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lFileSize ) )
				{
					fpRef = fopen( strReferenceFilePath, "wb" );

					if( fpRef )
					{
						fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

						fclose( fpRef );
					}
				}

				delete [] pstrEncryptBuffer;
			}
		}
	}
	else
	{
		if( fpDat )
		{
			fclose( fpDat );
		}
		if( fpRef )
		{
			fclose( fpRef );
		}
		if( fpBackupDat )
		{
			fclose( fpBackupDat );
		}
		if( fpBackupRef )
		{
			fclose( fpBackupRef );
		}
	}

	return bResult;
}
*/

bool AuPackingManager::CompletePacking( char *pstrFolder, int lCurrentPosition, int lPackedFileCount, ApAdmin *pcsAdminAdd, ApAdmin *pcsAdminRemove )
{
	FILE			*fpDat, *fpRef;
	FILE			*fpBackupDat, *fpBackupRef;

	bool			bResult;
	bool			bEmptyDat, bEmptyRef;
	char			strDataFilePath[255];
	char			strBackupDataFilePath[255];
	char			strReferenceFilePath[255];
	char			strBackupReferenceFilePath[255];

	bEmptyDat = false;
	bEmptyRef = false;

	if(  !stricmp( pstrFolder, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );

		sprintf( strReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );
	}

	bResult = false;

	fpDat = fopen( strDataFilePath, "ab" );
	fpBackupDat = fopen( strBackupDataFilePath, "rb" );

	fpRef = fopen( strReferenceFilePath, "ab" );
	fpBackupRef = fopen( strBackupReferenceFilePath, "rb" );
	
	if( (fpDat!=NULL) && (fpRef!=NULL) )
	{
		if( (fpBackupDat!=NULL) && (fpBackupRef!=NULL) )
		{
			char				*pstrEncryptBuffer;			
			char				*pstrDeletePointer;
			int					lEncryptSize;
			
			//파일크기를 얻는다.
			fseek( fpBackupRef, 0, SEEK_END );
			lEncryptSize = ftell(fpBackupRef);

			//파일 크기를 얻었으니 다시 앞으로~
			fseek( fpBackupRef, 0, SEEK_SET );

			//@{ 2006/04/11 burumal
			//pstrEncryptBuffer = new char[lEncryptSize];
			//pstrDeletePointer = pstrEncryptBuffer; //지우기용으로 포인터만 복사해둔다.

			bool bUseMemPool = false;
			if ( m_cCompress.GetMemPool() )
			{
				if ( lEncryptSize > m_cCompress.GetMemPoolSize() )					
					m_cCompress.AllocMemPool(lEncryptSize);

				if ( lEncryptSize <= m_cCompress.GetMemPoolSize() )
				{
					pstrEncryptBuffer = m_cCompress.GetMemPool();
					pstrDeletePointer = NULL;
					bUseMemPool = true;
				}
			}			
			
			if ( bUseMemPool == false )
			{
				pstrEncryptBuffer = new char[lEncryptSize];
				pstrDeletePointer = pstrEncryptBuffer; //지우기용으로 포인터만 복사해둔다.
			}
			//@}

			fread( pstrEncryptBuffer, 1, lEncryptSize, fpBackupRef );
			fclose( fpBackupRef );			

			if( m_cMD5Encrypt.DecryptString( "1111", pstrEncryptBuffer, lEncryptSize ) )
			{
				//백업된 RefrenceFile을 읽으며 데이터를 추가한다.
				int					lFolderNameSize;
				int					lRefFileCount;
				
				//@{ 2006/04/11 burumal
				//char				*pstrFolderName;
				char				pstrFolderName[512];
				//@}

				//파일이 몇개냐?
				memcpy( &lRefFileCount, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lRefFileCount, 1, sizeof(int), fpBackupRef );

				//폴더 이름크기
				memcpy( &lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				//fread( &lFolderNameSize, 1, sizeof(int), fpBackupRef );

				//폴더 이름
				//@{ 2006/04/11 burumal
				//pstrFolderName = new char[lFolderNameSize];
				//@}
				memset( pstrFolderName, 0, lFolderNameSize );
				memcpy( pstrFolderName, pstrEncryptBuffer, lFolderNameSize );
				pstrEncryptBuffer+=lFolderNameSize;
				//fread( pstrFolderName, 1, lFolderNameSize, fpBackupRef );				

				for( int lCounter=0; lCounter < lRefFileCount; lCounter++ )
				{
					int				lFileNameSize;
					int				lStartPos, lFileSize;					
					char			pstrFileName[512];

					//파일이름 크기 복사.
					memcpy( &lFileNameSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileNameSize, 1, sizeof(int), fpBackupRef );

					//@{ 2006/04/11 burumal
					//pstrFileName = new char[lFileNameSize];
					//@}
					memset( pstrFileName, 0, lFileNameSize );

					//파일이름 복사
					memcpy( pstrFileName, pstrEncryptBuffer, lFileNameSize );
					pstrEncryptBuffer+=lFileNameSize;
					//fread( pstrFileName, 1, lFileNameSize, fpBackupRef );

					//시작위치 복사
					memcpy( &lStartPos, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lStartPos, 1, sizeof(int), fpBackupRef );

					//파일크기 복사
					memcpy( &lFileSize, pstrEncryptBuffer, sizeof(int) );
					pstrEncryptBuffer+=sizeof(int);
					//fread( &lFileSize, 1, sizeof(int), fpBackupRef );

					if( pstrFileName )
					{
						bool			bSkip;

						bSkip = false;

						//만약 패킹된 파일이라면 예전파일은 필요없으므로 스킵~
						if( pcsAdminAdd->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//만약 지우게될 파일이면 필요없으므로 스킵~
						if( pcsAdminRemove->GetObject( pstrFileName ) )
						{
							bSkip = true;
						}

						//스킵될 파일이 아니면 이어서 쓴다.
						if( bSkip == false )
						{
							int				lRestSize;
							int				lReadBytes;
							char			strBuffer[Packing_lBlockSize];
							
							//Refrence파일을 편집한다.
							lFileNameSize = (int)strlen( pstrFileName ) + 1;

							fwrite( &lFileNameSize, 1, sizeof(int), fpRef );
							fwrite( pstrFileName, 1, lFileNameSize, fpRef );
							fwrite( &lCurrentPosition, 1, sizeof(int), fpRef );
							fwrite( &lFileSize, 1, sizeof(int), fpRef );

							//복사할 파일의 위치로 이동한다.
							fseek( fpBackupDat, lStartPos, SEEK_SET );

							lRestSize = lFileSize;
							
							//Data.Dat로 파일을 복사한다.
							while( 1 )
							{								
								//버퍼 초기화
								//memset( strBuffer, 0, Packing_lBlockSize );

								if( lRestSize < Packing_lBlockSize )
								{
									lReadBytes = (int)fread( strBuffer, 1, lRestSize, fpBackupDat );
									fwrite( strBuffer, 1, lReadBytes, fpDat );
									break;
								}
								else
								{
									lReadBytes = (int)fread( strBuffer, 1, Packing_lBlockSize, fpBackupDat );

									if( 0 < lReadBytes )
									{
										//@{ 2006/04/10 burumal								

										//4k씩 읽어서 Data.Dat에 저장한다. 										
										//fwrite( strBuffer, 1, Packing_lBlockSize, fpDat );

										//읽어 들인 만큼 Data.Dat에 저장한다.
										fwrite( strBuffer, 1, lReadBytes, fpDat );
										//@}
									}

									//4k이하를 읽었다면 끝낸다.
									if( lReadBytes < Packing_lBlockSize )
									{
										break;
									}

									lRestSize -= lReadBytes;

									if( lRestSize <= 0 )
									{
										break;
									}
								}
							}

							lCurrentPosition += lFileSize;
							lPackedFileCount++;
						}
					}

					//@{ 2006/04/11 burumal
					//delete [] pstrFileName;
					//@}
				}

				//@{ 2006/04/11 burumal
				//delete [] pstrFolderName;
				//@}
			}

			//@{ 2006/04/11 burumal
			//delete [] pstrDeletePointer;
			if ( pstrDeletePointer )
				delete [] pstrDeletePointer;
			//@}

			fclose( fpBackupDat );
			fclose( fpBackupRef );
		}

		//파일크기를 얻어낸다.
		fseek( fpDat, 0, SEEK_END );
		fseek( fpRef, 0, SEEK_END );

		if( ftell( fpDat ) == 0 )
			bEmptyDat = true;
		if( ftell( fpRef ) == 0  )
			bEmptyRef = true;

		fclose( fpDat );
		fclose( fpRef );

		//@{ 2006/04/11 burumal
		/*
		//백업 파일은 삭제하고 Refrence파일은 암호화한다.
		DeleteFile( strBackupDataFilePath );
		DeleteFile( strBackupReferenceFilePath );

		if( (bEmptyDat == true) || (bEmptyRef == true) )
		{
			DeleteFile( strDataFilePath );
			DeleteFile( strReferenceFilePath );
		}
		else
		{
			//최종적으로 fpRef에 lPackedFileCount를 수정해준다.
			fpRef = fopen( strReferenceFilePath, "rb+" );
			if( fpRef )
			{
				fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );
				fclose( fpRef );
			}

			//암호화한다.
			fpRef = fopen( strReferenceFilePath, "rb" );
			if( fpRef )
			{
				long				lFileSize;
				char				*pstrEncryptBuffer;

				//파일 크기를 얻는다.
				fseek( fpRef, 0, SEEK_END );
				lFileSize = ftell(fpRef);
				//파일크기를 얻었으니 다시 앞으로~
				fseek( fpRef, 0, SEEK_SET ); 

				pstrEncryptBuffer = new char[lFileSize];

				fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

				fclose( fpRef );

				//Key는 "archlord"이다.
				if( m_cMD5Encrypt.EncryptString( MD5_HASH_KEY_STRING, pstrEncryptBuffer, lFileSize ) )
				{
					fpRef = fopen( strReferenceFilePath, "wb" );

					if( fpRef )
					{
						fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

						fclose( fpRef );
					}
				}

				delete [] pstrEncryptBuffer;
			}
		}
		*/
		// reference 파일을 암호화한후 백업 파일을 삭제한다.
		//@}

		if( (bEmptyDat == true) || (bEmptyRef == true) )
		{
			DeleteFile( strDataFilePath );
			DeleteFile( strReferenceFilePath );
		}
		else
		{
			//최종적으로 fpRef에 lPackedFileCount를 수정해준다.
			fpRef = fopen( strReferenceFilePath, "rb+" );
			if( fpRef )
			{
				fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );
				fclose( fpRef );
			}

			//암호화한다.
			fpRef = fopen( strReferenceFilePath, "rb" );
			if( fpRef )
			{
				long				lFileSize;
				char				*pstrEncryptBuffer;

				//파일 크기를 얻는다.
				fseek( fpRef, 0, SEEK_END );
				lFileSize = ftell(fpRef);

				//파일크기를 얻었으니 다시 앞으로~
				fseek( fpRef, 0, SEEK_SET ); 

				pstrEncryptBuffer = new char[lFileSize];

				fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

				fclose( fpRef );

				//Key는 MD5_HASH_KEY_STRING이다.
				if( m_cMD5Encrypt.EncryptString( "1111", pstrEncryptBuffer, lFileSize ) )
				{
					fpRef = fopen( strReferenceFilePath, "wb" );

					if( fpRef )
					{
						fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

						fclose( fpRef );
					}
				}

				delete [] pstrEncryptBuffer;
			}
		}

		//백업 파일은 삭제한다.
		DeleteFile( strBackupDataFilePath );
		DeleteFile( strBackupReferenceFilePath );
	}
	else
	{
		if( fpDat )
		{
			fclose( fpDat );
		}
		if( fpRef )
		{
			fclose( fpRef );
		}
		if( fpBackupDat )
		{
			fclose( fpBackupDat );
		}
		if( fpBackupRef )
		{
			fclose( fpBackupRef );
		}
	}

	return bResult;
}

/*
bool AuPackingManager::BuildPackingFile( char *pstrTempFolderName, char *pstrFolderName, char *pstrFileName, int lCurrentPos, int lFileSize )
{
	FILE			*fpData, *fpReference;
	FILE			*fpOriginFile;
	bool			bResult;

	char			strDataFilePath[255];
	char			strReferenceFilePath[255];
	char			strOriginFilePath[255];

	bResult = false;

	if( !stricmp( pstrFolderName, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strOriginFilePath, "%s", pstrFileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pstrFolderName, Packing_Data_FileName );
		sprintf( strReferenceFilePath, "%s\\%s", pstrFolderName, Packing_Reference_FileName );
		sprintf( strOriginFilePath, "%s\\%s", pstrTempFolderName, pstrFileName );

		//@{ 2006/04/07 burumal		
		//CreateFolder( pstrFolderName );
		//@}
	}

	fpData = fopen( strDataFilePath, "ab" );
	fpReference = fopen( strReferenceFilePath, "ab" );
	fpOriginFile = fopen( strOriginFilePath, "rb" );

	if( fpData && fpReference && fpOriginFile )
	{
		size_t			lFileNameSize;
		size_t			lReadBytes;

		//@{ 2006/04/07 burumal
		//char			strBuffer[Packing_lBlockSize];
		static char		strBuffer[Packing_lBlockSize];
		//@}

		//파일 맨 마지막으로 이동한다.
		fseek( fpData, 0, SEEK_END );
		fseek( fpReference, 0, SEEK_END );

		//Refrence파일을 편집한다.
		lFileNameSize = strlen( pstrFileName ) + 1;

		fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
		fwrite( pstrFileName, 1, lFileNameSize, fpReference );
		fwrite( &lCurrentPos, 1, sizeof(int), fpReference );
        fwrite( &lFileSize, 1, sizeof(int), fpReference );

		//Data.Dat로 파일을 복사한다.
		while( 1 )
		{
			//@{ 2006/04/07 burumal 
			// 굳이 버퍼를 초기화할 필요는 없음
			//버퍼 초기화
			//memset( strBuffer, 0, Packing_lBlockSize );
			//@}

			//파일을 저장한다.
			lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, fpOriginFile );

			if( 0 < lReadBytes )
			{
				//4k씩 읽어서 Data.Dat에 저장한다. 
				fwrite( strBuffer, 1, lReadBytes, fpData );
			}

			//4k이하를 읽었다면 끝낸다.
			if( lReadBytes < Packing_lBlockSize )
			{
				break;
			}
		}

		fclose( fpData );
		fclose( fpReference );
		fclose( fpOriginFile );
	}
	//여기로 들어올일은 없지만 혹시 모르니....;;
	else
	{
		if( fpData )
		{
			fclose( fpData );
		}
		if( fpReference )
		{
			fclose( fpReference );
		}
		if( fpOriginFile )
		{
			fclose( fpOriginFile );
		}
	}

	return bResult;
}
*/

bool AuPackingManager::BuildPackingFile( char* pstrTempFolderName, char* pstrFolderName, 
										char* pstrFileName, int lCurrentPos, int lFileSize,
										FILE* fpData, FILE* fpReference, char* pDecompMem)
{	
	//FILE			*fpOriginFile;
	bool			bResult;
	
	char			strOriginFilePath[255];

	bResult = false;

	if ( !fpData || !fpReference || !pDecompMem || (lFileSize <= 0) )
		return bResult;

	if( !stricmp( pstrFolderName, "root" ) )
	{		
		sprintf( strOriginFilePath, "%s", pstrFileName );
	}
	else
	{		
		sprintf( strOriginFilePath, "%s\\%s", pstrTempFolderName, pstrFileName );
	}
	
	//fpOriginFile = fopen( strOriginFilePath, "rb" );

	//if( fpData && fpReference && fpOriginFile )
	if( fpData && fpReference )
	{
		int			lFileNameSize;
		int			lReadBytes;
		
		//char		strBuffer[Packing_lBlockSize];
		
		//Refrence파일을 편집한다.
		lFileNameSize = (int)strlen( pstrFileName ) + 1;

		fwrite( &lFileNameSize, 1, sizeof(int), fpReference );
		fwrite( pstrFileName, 1, lFileNameSize, fpReference );
		fwrite( &lCurrentPos, 1, sizeof(int), fpReference );
		fwrite( &lFileSize, 1, sizeof(int), fpReference );

		//Data.Dat로 파일을 복사한다.
		while( 1 )
		{			
			/*
			//파일을 저장한다.
			lReadBytes = fread( strBuffer, 1, Packing_lBlockSize, fpOriginFile );		

			if( 0 < lReadBytes )
			{
				//4k씩 읽어서 Data.Dat에 저장한다. 
				fwrite( strBuffer, 1, lReadBytes, fpData );
			}

			//4k이하를 읽었다면 끝낸다.
			if( lReadBytes < Packing_lBlockSize )
			{
				break;
			}
			*/
		
			if ( lFileSize >= Packing_lBlockSize )
			{
				lReadBytes = Packing_lBlockSize;
				lFileSize -= Packing_lBlockSize;
			}
			else
			{
				lReadBytes = lFileSize;
				lFileSize = 0;
			}

			if ( 0 < lReadBytes )
			{
				fwrite(pDecompMem, 1, lReadBytes, fpData);
				pDecompMem += lReadBytes;
			}

			if ( lReadBytes < Packing_lBlockSize )
			{
				break;
			}
		}
		
		//fclose( fpOriginFile );
	}
	//여기로 들어올일은 없지만 혹시 모르니....;;
	else
	{
		//if( fpOriginFile )
		{
			//fclose( fpOriginFile );
		}
	}

	return bResult;
}

//@{ 2006/04/12 burumal
//PackingFileSpaceCompFunc
struct PACKING_FILE_SPACE
{
	int nStart;
	int nEnd;
	int nType; // 0: normal , 1: empty
	CPackingFile* pPackingFile;
};

int PackingFileSpaceCompFunc(const void* pAlpha, const void* pBeta)
{
	return ((*(PACKING_FILE_SPACE**) pAlpha))->nStart - (*((PACKING_FILE_SPACE**) pBeta))->nStart;
}

bool AuPackingManager::ReadBakReference(char* pstrFolder, ApAdmin* pAdminAdd, ApAdmin* pAdminRemove, 
										ApAdmin* pAdminPackingNormal, ApAdmin* pAdminPackingEmpty)
{
	FILE*	fpBackupRef;
	char	strBackupReferenceFilePath[255];

	int		lFolderNameSize;
	int		lRefFileCount;

	char	pstrFolderName[256];
	
	PACKING_FILE_SPACE** ppPackingFileSpaceInfo = NULL;
	
	if( !stricmp( pstrFolder, "root" ) )
	{		
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );
	}
	else
	{
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );
	}

	fpBackupRef = fopen( strBackupReferenceFilePath, "rb" );
	
	if ( fpBackupRef )
	{
		char*	pstrEncryptBuffer;			
		char*	pstrDeletePointer;
		int		lEncryptSize;

		//파일크기를 얻는다.
		fseek( fpBackupRef, 0, SEEK_END );
		lEncryptSize = ftell(fpBackupRef);

		//파일 크기를 얻었으니 다시 앞으로~
		fseek( fpBackupRef, 0, SEEK_SET );

		// 메모리풀 처리
		bool bUseMemPool = false;
		if ( m_cCompress.GetMemPool() )
		{
			if ( lEncryptSize > m_cCompress.GetMemPoolSize() )					
				m_cCompress.AllocMemPool(lEncryptSize);

			if ( lEncryptSize <= m_cCompress.GetMemPoolSize() )
			{
				pstrEncryptBuffer = m_cCompress.GetMemPool();
				pstrDeletePointer = NULL;
				bUseMemPool = true;
			}
		}			

		if ( bUseMemPool == false )
		{
			pstrEncryptBuffer = new char[lEncryptSize];
			pstrDeletePointer = pstrEncryptBuffer; //지우기용으로 포인터만 복사해둔다.
		}

		fread( pstrEncryptBuffer, 1, lEncryptSize, fpBackupRef );
		fclose( fpBackupRef );

		if( m_cMD5Encrypt.DecryptString( "1111", pstrEncryptBuffer, lEncryptSize ) )
		{
			//백업된 RefrenceFile을 읽으며 데이터를 추가한다.

			//파일이 몇개냐?
			memcpy( &lRefFileCount, pstrEncryptBuffer, sizeof(int) );
			pstrEncryptBuffer+=sizeof(int);
			
			//폴더 이름크기
			memcpy( &lFolderNameSize, pstrEncryptBuffer, sizeof(int) );
			pstrEncryptBuffer+=sizeof(int);
			
			//폴더 이름
			memset( pstrFolderName, 0, lFolderNameSize );
			memcpy( pstrFolderName, pstrEncryptBuffer, lFolderNameSize );
			pstrEncryptBuffer+=lFolderNameSize;

			// 파일 공간정보 메모리 할당
			ppPackingFileSpaceInfo = new PACKING_FILE_SPACE*[lRefFileCount];
			ASSERT(ppPackingFileSpaceInfo);
			memset(ppPackingFileSpaceInfo, 0, sizeof(PACKING_FILE_SPACE*) * lRefFileCount);
			
			for( int lCounter = 0; lCounter < lRefFileCount; lCounter++ )
			{				
				int				lFileNameSize;
				int				lStartPos, lFileSize;					
				char			pstrFileName[256];

				//파일이름 크기 복사.
				memcpy( &lFileNameSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				
				memset( pstrFileName, 0, lFileNameSize );

				//파일이름 복사
				memcpy( pstrFileName, pstrEncryptBuffer, lFileNameSize );
				pstrEncryptBuffer+=lFileNameSize;
				
				//시작위치 복사
				memcpy( &lStartPos, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);
				
				//파일크기 복사
				memcpy( &lFileSize, pstrEncryptBuffer, sizeof(int) );
				pstrEncryptBuffer+=sizeof(int);

				if ( pstrFileName )
				{
					bool bSkip = false;

					//만약 패킹된 파일이라면 예전파일은 필요없으므로 스킵~
					if( !bSkip && pAdminAdd->GetObject( _strlwr(pstrFileName) ) )
					{
						bSkip = true;
					}

					//만약 지우게될 파일이면 필요없으므로 스킵~
					if( !bSkip && pAdminRemove->GetObject( _strlwr(pstrFileName) ) )
					{
						bSkip = true;
					}
					
					// 데이타 준비
					CPackingFile* pPackingFile;
					CPackingDataNode* pPackingDataNode;

					pPackingFile = new CPackingFile;
					pPackingDataNode = new CPackingDataNode;

					ASSERT(pPackingFile);
					ASSERT(pPackingDataNode);

					// 파일정보 세팅
					pPackingFile->m_lFileNameSize = lFileNameSize;
					pPackingFile->m_pstrFileName = new char[lFileNameSize];
					memset(pPackingFile->m_pstrFileName, 0, lFileNameSize);
					memcpy(pPackingFile->m_pstrFileName, pstrFileName, lFileNameSize);

					// 파일위치 정보세팅
					pPackingDataNode->m_lStartPos = lStartPos;
					pPackingDataNode->m_lSize = lFileSize;

					pPackingFile->m_pcsDataNode = pPackingDataNode;

					if ( bSkip )
					{
						// Empty 배열에 추가한다
						pAdminPackingEmpty->AddObject( (void **)&pPackingFile, _strlwr(pstrFileName) );
					}
					else
					{
						// Normal 배열에 추가한다
						pAdminPackingNormal->AddObject( (void **)&pPackingFile, _strlwr(pstrFileName) );
					}

					//파일 공간정보 저장
					ppPackingFileSpaceInfo[lCounter] = new PACKING_FILE_SPACE;
					ppPackingFileSpaceInfo[lCounter]->nStart = lStartPos;
					ppPackingFileSpaceInfo[lCounter]->nEnd = lStartPos + lFileSize - 1;
					ppPackingFileSpaceInfo[lCounter]->nType = bSkip ? 1 : 0;
					ppPackingFileSpaceInfo[lCounter]->pPackingFile = pPackingFile;
				}
			}
		}

		if ( pstrDeletePointer )
			delete [] pstrDeletePointer;

		fclose(fpBackupRef);
	}
	
	if ( ppPackingFileSpaceInfo )
	{
		// 파일 공간정보 Sorting
		qsort(ppPackingFileSpaceInfo, lRefFileCount, sizeof(PACKING_FILE_SPACE**), ::PackingFileSpaceCompFunc);
		
		
		// 모든 빈틈을 찾아서 empty 공간으로 만든다
		int nIdx;

		for ( nIdx = 0; nIdx < lRefFileCount - 1; nIdx++ )
		{
			ASSERT(ppPackingFileSpaceInfo[nIdx + 1]->nStart > ppPackingFileSpaceInfo[nIdx]->nEnd);

			if ( (ppPackingFileSpaceInfo[nIdx]->nEnd + 1) != ppPackingFileSpaceInfo[nIdx + 1]->nStart )
			{
				// 왼쪽과 오른쪽 사이에 빈틈이 있을때

				if ( ppPackingFileSpaceInfo[nIdx]->nType == 1 ) 
				{
					// 왼쪽이 이미 empty 공간이므로 왼쪽을 확장해준다
					ASSERT(ppPackingFileSpaceInfo[nIdx]->pPackingFile);

					ppPackingFileSpaceInfo[nIdx]->pPackingFile->m_pcsDataNode->m_lSize =
						ppPackingFileSpaceInfo[nIdx + 1]->nStart - ppPackingFileSpaceInfo[nIdx]->nStart;
				}
				else
				{
					// 왼쪽이 normal 이고

					if ( ppPackingFileSpaceInfo[nIdx + 1]->nType == 1 )
					{
						// 오른쪽이 empty 이면 오른쪽 empty 공간을 확장해준다

						ppPackingFileSpaceInfo[nIdx + 1]->nStart = ppPackingFileSpaceInfo[nIdx]->nEnd + 1;

						ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pcsDataNode->m_lStartPos =
							ppPackingFileSpaceInfo[nIdx + 1]->nStart;

						ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pcsDataNode->m_lSize =
							ppPackingFileSpaceInfo[nIdx + 1]->nEnd - ppPackingFileSpaceInfo[nIdx + 1]->nStart + 1;
					}
					else
					{
						// 오른쪽도 normal 이면 새로운 empty 공간 오브젝트를 추가해준다

						AddEmptyPackingFileInfo(ppPackingFileSpaceInfo[nIdx]->nEnd + 1,
							ppPackingFileSpaceInfo[nIdx + 1]->nStart - ppPackingFileSpaceInfo[nIdx]->nEnd - 1, pAdminPackingEmpty);
					}
				}
			}
		}

		// 붙어있는 empty 공간을 서로 merge 시킨다	
		if ( pAdminPackingEmpty->GetObjectCount() > 0 )
		{
			for ( nIdx = 0; nIdx < lRefFileCount - 1; nIdx++ )
			{
				if ( ppPackingFileSpaceInfo[nIdx]->nType == 1 && ppPackingFileSpaceInfo[nIdx + 1]->nType == 1 )
				{
					// 양쪽이 모두 empty면 merge 시킨다

					ppPackingFileSpaceInfo[nIdx]->pPackingFile->m_pcsDataNode->m_lSize +=
						ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pcsDataNode->m_lSize;

					// 필요없는 empty 공간 삭제
					pAdminPackingEmpty->RemoveObject(ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile->m_pstrFileName);
					delete ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile;

					ppPackingFileSpaceInfo[nIdx]->nEnd = ppPackingFileSpaceInfo[nIdx + 1]->nEnd;
					ppPackingFileSpaceInfo[nIdx + 1]->nStart = ppPackingFileSpaceInfo[nIdx]->nStart;
					ppPackingFileSpaceInfo[nIdx + 1]->pPackingFile = ppPackingFileSpaceInfo[nIdx]->pPackingFile;
				}
			}
		}
	}

	/*
	// 붙어있는 empty 공간을 서로 merge 시킨다	
	if ( pAdminPackingEmpty->GetObjectCount() > 0 )
	{
		int nEmptyIdx = 0;
		CPackingFile** ppCurEmpty;

		// 모든 empty 파일의 후방 쓰레기 공간에 대한 처리
		for ( ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx);
			ppCurEmpty;
			ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx) )
		{
			ASSERT(ppCurEmpty);
			ASSERT(*ppCurEmpty);
			ASSERT((*ppCurEmpty)->m_pcsDataNode);

			long lSrcStartPos = (*ppCurEmpty)->m_pcsDataNode->m_lStartPos;
			long lSrcEndPos = lSrcStartPos + (*ppCurEmpty)->m_pcsDataNode->m_lSize;

			if ( lSrcStartPos < 0 && lSrcEndPos < 0 )
				continue;

			// 다시 모든 Empty 리스트에 대해서
			int nRefIdx = 0;
			CPackingFile** ppRef;

			for ( ppRef = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nRefIdx);
				ppRef;
				ppRef= (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nRefIdx) )
			{					
				ASSERT(ppRef);
				ASSERT(*ppRef);
				ASSERT((*ppRef)->m_pcsDataNode);

				long lDestStartPos = (*ppRef)->m_pcsDataNode->m_lStartPos;
				long lDestEndPos = lDestStartPos + (*ppRef)->m_pcsDataNode->m_lSize;

				if ( lDestStartPos < 0 && lDestEndPos < 0 )
					continue;

				if ( *ppRef == *ppCurEmpty )
					continue;

				if ( lSrcEndPos == lDestStartPos )
				{					
				}
			}
		}
	}
	*/

	/*
	// Empty 파일들을 대상으로 쓰레기공간(space)을 계산해 크기를 조정한다
	{
		if ( pAdminPackingEmpty->GetObjectCount() <= 0 )
			return true;

		int nEmptyIdx = 0;
		CPackingFile** ppCurEmpty;

		// lFirstSpaceSize 값이 0 이 아닌 경우는 
		// Data.Bak 파일의 첫번째 공간이 쓰레기 공간일 경우다	
		long lFirstSpaceSize = -1;
		CPackingFile** ppFirstSpaceNeighbor = NULL;

		// 모든 empty 파일의 후방 쓰레기 공간에 대한 처리
		for ( ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx);
			ppCurEmpty;
			ppCurEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nEmptyIdx) )
		{
			ASSERT(ppCurEmpty);
			ASSERT(*ppCurEmpty);

			ApAdmin* pRefAdmin;

			int nAdminIdx;
			long lResizedPos = -1;

			bool bNoSpace = false;

			ASSERT((*ppCurEmpty)->m_pcsDataNode);
			
			long lSrcStartPos = (*ppCurEmpty)->m_pcsDataNode->m_lStartPos;
			long lSrcEndPos = lSrcStartPos + (*ppCurEmpty)->m_pcsDataNode->m_lSize;

			if ( lSrcStartPos < 0 && lSrcEndPos < 0 )
				continue;

			for ( nAdminIdx = 0; nAdminIdx < 2; nAdminIdx++ )
			{
				switch ( nAdminIdx )
				{
				case 0 :
					// Normal 리스트에 대해서 search
					pRefAdmin = pAdminPackingNormal;
					break;

				case 1 :
					// Empty 리스트에 대해서 search
					pRefAdmin = pAdminPackingEmpty;
					break;
				}

				CPackingFile** ppRef;
				int nRefIdx = 0;				
				
				for ( ppRef = (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx);
					ppRef;
					ppRef= (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx) )
				{					
					ASSERT(ppRef);
					ASSERT(*ppRef);
					ASSERT((*ppRef)->m_pcsDataNode);

					long lDestStartPos = (*ppRef)->m_pcsDataNode->m_lStartPos;
					long lDestEndPos = lDestStartPos + (*ppRef)->m_pcsDataNode->m_lSize;

					if ( lDestStartPos < 0 && lDestEndPos < 0 )
						continue;

					if ( lFirstSpaceSize == -1 )
					{
						lFirstSpaceSize = lDestStartPos;

						if ( nAdminIdx == 1 ) 
							ppFirstSpaceNeighbor = ppRef; // empty 일때만 기록
						else
							ppFirstSpaceNeighbor = NULL;
					}	
					else
					if ( lFirstSpaceSize > lDestStartPos )
					{
						lFirstSpaceSize = lDestStartPos;

						if ( nAdminIdx == 1 ) 
							ppFirstSpaceNeighbor = ppRef; // empty 일때만 기록
						else
							ppFirstSpaceNeighbor = NULL;
					}

					if ( *ppRef == *ppCurEmpty )
						continue;

					// 빈틈이 없는 경우
					if ( lDestStartPos == lSrcEndPos )
					{	
						if ( nAdminIdx == 1 ) // 같은 Empty 공간이면 merge 시킨다
						{
							ASSERT(*ppRef != *ppCurEmpty);

							(*ppCurEmpty)->m_pcsDataNode->m_lSize = 
								(*ppCurEmpty)->m_pcsDataNode->m_lSize + (*ppRef)->m_pcsDataNode->m_lSize;

							// DataNode 값을 둘다 -1로 마크해둔뒤 이 루프가 끝난후 이 오브젝트를 찾아 삭제한다
							(*ppRef)->m_pcsDataNode->m_lStartPos = -1;
							(*ppRef)->m_pcsDataNode->m_lSize = -1;
							
							continue;
						}

						bNoSpace = true;
						break;
					}

					if ( lDestStartPos > lSrcEndPos )
					{
						if ( lResizedPos == -1 )
							lResizedPos = lDestStartPos;
						else
						if ( lResizedPos > lDestStartPos )
							lResizedPos = lDestStartPos;
					}
				}

				if ( bNoSpace )
					break;
			}

			if ( !bNoSpace )
			{
				//ASSERT(lResizedPos > 0);

				// 사이즈를 늘려준다
				if ( lResizedPos > 0 )
					(*ppCurEmpty)->m_pcsDataNode->m_lSize = (lResizedPos - (*ppCurEmpty)->m_pcsDataNode->m_lStartPos);

				// lResizedPos 가 -1인 경우는 ppCurEmpty가 마지막 파일이기 때문
			}
		}		
		
		// 지워진 Empty 공간 오브젝트를 삭제한다
		int nDelIdx = 0;
		CPackingFile* pDelEmpty = NULL;
		CPackingFile** ppDelEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nDelIdx);

		while ( pDelEmpty && *ppDelEmpty )
		{
			pDelEmpty = *ppDelEmpty;

			if ( (pDelEmpty->m_pcsDataNode->m_lStartPos == -1) && (pDelEmpty->m_pcsDataNode->m_lSize == -1) )
			{
				pAdminPackingEmpty->RemoveObject(pDelEmpty->m_pstrFileName);
				delete pDelEmpty;
			}

			ppDelEmpty = (CPackingFile**) pAdminPackingEmpty->GetObjectSequence(&nDelIdx);
		}		
		
		// First Space pos
		ASSERT(lFirstSpaceSize >= 0);

		if ( lFirstSpaceSize > 0 )
		{
			// First Space 공간을
			// CPackingFile 오브젝트로 만들어서 AdminEmpty에 추가한다
			if ( ppFirstSpaceNeighbor && (*ppFirstSpaceNeighbor) )
			{
				(*ppFirstSpaceNeighbor)->m_pcsDataNode->m_lStartPos = 0;
				(*ppFirstSpaceNeighbor)->m_pcsDataNode->m_lSize = (*ppFirstSpaceNeighbor)->m_pcsDataNode->m_lSize + lFirstSpaceSize;
			}
			else
			{
				AddEmptyPackingFileInfo(0, lFirstSpaceSize, pAdminPackingEmpty);
			}
		}		
	}
	*/

	
	/*
	// Normal 파일들을 대상으로 쓰레기공간(space)을 계산해 Empty 리스트에 추가한다
	{
		if ( pAdminPackingNormal->GetObjectCount() <= 0 )
			return true;

		int nNormalIdx = 0;
		CPackingFile** ppCurNormal;

		// 모든 normal 파일의 후방 쓰레기 공간에 대한 처리
		for ( ppCurNormal = (CPackingFile**) pAdminPackingNormal->GetObjectSequence(&nNormalIdx);
			ppCurNormal;
			ppCurNormal = (CPackingFile**) pAdminPackingNormal->GetObjectSequence(&nNormalIdx) )
		{
			ASSERT(ppCurNormal);
			ASSERT(*ppCurNormal);

			ApAdmin* pRefAdmin;

			int nAdminIdx;
			long lResizedPos = -1;		

			bool bNoSpace = false;

			ASSERT((*ppCurNormal)->m_pcsDataNode);
			long lSrcStartPos = (*ppCurNormal)->m_pcsDataNode->m_lStartPos;
			long lSrcEndPos = lSrcStartPos + (*ppCurNormal)->m_pcsDataNode->m_lSize;

			for ( nAdminIdx = 0; nAdminIdx < 2; nAdminIdx++ )
			{
				switch ( nAdminIdx )
				{
				case 0 :				
					// Normal 리스트에 대해서 search
					pRefAdmin = pAdminPackingNormal;					
					break;

				case 1 :
					// Empty 리스트에 대해서 search
					pRefAdmin = pAdminPackingEmpty;
					break;
				}

				CPackingFile** ppRef;
				int nRefIdx = 0;

				for ( ppRef = (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx);
					ppRef;
					ppRef= (CPackingFile**) pRefAdmin->GetObjectSequence(&nRefIdx) )
				{
					ASSERT(ppRef);
					ASSERT(*ppRef);
					ASSERT((*ppRef)->m_pcsDataNode);

					long lDestStartPos = (*ppRef)->m_pcsDataNode->m_lStartPos;

					if ( *ppRef == *ppCurNormal )
						continue;

					// 빈틈이 없는 경우
					if ( lDestStartPos == lSrcEndPos )
					{
						bNoSpace = true;
						break;
					}

					if ( lDestStartPos > lSrcEndPos )
					{
						if ( lResizedPos == -1 )
							lResizedPos = lDestStartPos;
						else
							if ( lResizedPos > lDestStartPos )
								lResizedPos = lDestStartPos;
					}
				}

				if ( bNoSpace )
					break;
			}

			if ( !bNoSpace )
			{
				// Normal 파일과 Normal 파일 사이에 새롭게 생긴 빈 공간을
				// CPackingFile 오브젝트로 만들어서 AdminEmpty에 추가한다

				// lResizedPos 가 -1인 경우는 ppCurNormal가 마지막 파일이기 때문

				if ( lResizedPos > 0 )
					AddEmptyPackingFileInfo(lSrcEndPos, lResizedPos - lSrcEndPos, pAdminPackingEmpty);
			}
		}	
	}
	*/

	if ( ppPackingFileSpaceInfo )
	{
		int lCounter;
		for ( lCounter = 0; lCounter < lRefFileCount; lCounter++ )
			if ( ppPackingFileSpaceInfo[lCounter] )
				delete ppPackingFileSpaceInfo[lCounter];

		delete [] ppPackingFileSpaceInfo;
	}

	return true;
}
//@}

//@{ 2006/04/13 burumal
CPackingFile* AuPackingManager::AddEmptyPackingFileInfo(long lStartPos, long lSize, ApAdmin* pAdminPackingEmpty)
{
	if ( !pAdminPackingEmpty || (lStartPos < 0) || (lSize <= 0) )
		return NULL;
	
	CPackingFile* pNewEmpty = new CPackingFile;
	ASSERT(pNewEmpty);

	if ( pNewEmpty )
	{
		// 이름을 임시로 발급한다
		char pTempFileName[256];
		sprintf(pTempFileName, "__e_%05d__.__", m_nEmptyFileSerial++);
		int nFileNameLen = (int)strlen(pTempFileName) + 1;

		char* pNewFileName = new char[nFileNameLen];
		CPackingDataNode* pNewDataNode = new CPackingDataNode;

		if ( !pNewFileName || !pNewDataNode )
		{
			if ( pNewFileName )
				delete pNewFileName;

			if ( pNewDataNode )
				delete pNewDataNode;

			delete pNewEmpty;
		}
		else
		{
			memset(pNewFileName, 0, nFileNameLen);
			memcpy(pNewFileName, pTempFileName, nFileNameLen);

			pNewEmpty->m_pstrFileName = pNewFileName;
			pNewEmpty->m_lFileNameSize = nFileNameLen;
			
			pNewDataNode->m_lStartPos = lStartPos;
			pNewDataNode->m_lSize = lSize;

			pNewEmpty->m_pcsDataNode = pNewDataNode;

			pAdminPackingEmpty->AddObject((void**) &pNewEmpty, pNewFileName);
		}
	}

	return pNewEmpty;
}
//@}

//@{ 2006/04/16 burumal
bool AuPackingManager::ResizeFileSize(char* pFilePath, int nFinalSize)
{	
	if ( !pFilePath || nFinalSize <= 0 )
		return false;

	FILE* fpData = fopen(pFilePath, "rb");
	if ( !fpData )
		return false;
	fseek(fpData, 0, SEEK_END);
	long lDataFileSize = ftell(fpData);
	fclose(fpData);

	if ( lDataFileSize > nFinalSize )
	{
		HANDLE hDataFile = ::CreateFile(
			pFilePath, 
			//FILE_ALL_ACCESS, 
			GENERIC_READ | GENERIC_WRITE,
			0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if ( hDataFile != INVALID_HANDLE_VALUE )
		{	
			LARGE_INTEGER llMove;

			llMove.HighPart = 0;
			llMove.LowPart = (DWORD) nFinalSize;

			//SetFilePointerEx(hDataFile, llMove, NULL, FILE_BEGIN); // win98은 이 함수를 지원하지 않는다 -_-			

			DWORD dwPtrLow, dwError;
			dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);

			if ( dwPtrLow == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != NO_ERROR )
			{
				// try once again
				dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);

				if ( dwPtrLow == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != NO_ERROR )
				{
					ASSERT(0);
					CloseHandle(hDataFile);
					return false;
				}
			}

			SetEndOfFile(hDataFile);
			CloseHandle(hDataFile);

			return true;
		}
	}

	return false;
}

bool AuPackingManager::ResizeDatFile(CPackingFolder* pFolder)
{
	if ( !pFolder || !pFolder->m_pstrFolderName )
		return false;

	char strDataFilePath[256];	

	if( !stricmp( pFolder->m_pstrFolderName, "root" ) )
	{
		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
	}
	else
	{
		sprintf( strDataFilePath, "%s\\%s", pFolder->m_pstrFolderName, Packing_Data_FileName );
	}

	FILE* fpData = fopen(strDataFilePath, "rb");
	if ( !fpData )
		return false;
	fseek(fpData, 0, SEEK_END);
	long lDataFileSize = ftell(fpData);
	fclose(fpData);

	if ( lDataFileSize != pFolder->m_lDataFileSize && (lDataFileSize > pFolder->m_lDataFileSize) )
	{
		HANDLE hDataFile = CreateFile(strDataFilePath, 
			//FILE_ALL_ACCESS, 
			GENERIC_READ | GENERIC_WRITE,
			0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if ( hDataFile != INVALID_HANDLE_VALUE )
		{	
			LARGE_INTEGER llMove;
			llMove.HighPart = 0;
			llMove.LowPart = pFolder->m_lDataFileSize;
			
			//SetFilePointerEx(hDataFile, llMove, NULL, FILE_BEGIN); // win98은 이 함수를 지원하지 않는다 -_-
			
			DWORD dwPtrLow, dwError;
			dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);			
			if ( dwPtrLow == INVALID_SET_FILE_POINTER &&
				(dwError = GetLastError()) != NO_ERROR )
			{
				// try once again
				dwPtrLow = ::SetFilePointer(hDataFile, llMove.LowPart, &llMove.HighPart, FILE_BEGIN);				
				if ( dwPtrLow == INVALID_SET_FILE_POINTER &&
					(dwError = GetLastError()) != NO_ERROR )
				{
					ASSERT(0);
				}
			}

			SetEndOfFile(hDataFile);
			CloseHandle(hDataFile);

			return true;
		}
	}

	return false;
}
//@}

//@{ 2006/04/16 burumal
bool AuPackingManager::CompleteCurrentFolderProcess(char* pstrFolder, int lPackedFileCount)
{
	if ( !pstrFolder )
		return false;

	//bool	bEmptyDat;
	bool	bEmptyRef;	

	char	strReferenceFilePath[256];	
	char	strBackupReferenceFilePath[256];

	char	strDataFilePath[255];
	char	strBackupDataFilePath[255];

	int		nFinalValidDataSize = 0;

	bEmptyRef = false;

	if(  !stricmp( pstrFolder, "root" ) )
	{	
		sprintf( strReferenceFilePath, "%s", Packing_Reference_FileName );
		sprintf( strBackupReferenceFilePath, "%s", Packing_Reference_Backup_FileName );

		sprintf( strDataFilePath, "%s", Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s", Packing_Data_Backup_FileName );
	}
	else
	{	
		sprintf( strReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_FileName );		
		sprintf( strBackupReferenceFilePath, "%s\\%s", pstrFolder, Packing_Reference_Backup_FileName );

		sprintf( strDataFilePath, "%s\\%s", pstrFolder, Packing_Data_FileName );
		sprintf( strBackupDataFilePath, "%s\\%s", pstrFolder, Packing_Data_Backup_FileName );
	}	

	FILE* fpRef = fopen( strReferenceFilePath, "rb" );	
	if ( !fpRef )
		return false;

	//파일크기를 얻어낸다.	
	fseek( fpRef, 0, SEEK_END );

	if( ftell( fpRef ) <= 0  )
	{
		fseek(fpRef, 0, SEEK_END);

		if ( ftell(fpRef) <= 0 )
			bEmptyRef = true;
	}

	fclose( fpRef );

	if( bEmptyRef == true )
	{		
		DeleteFile( strReferenceFilePath );
	}
	else
	{
		//최종적으로 fpRef에 lPackedFileCount를 수정해준다.
		fpRef = fopen( strReferenceFilePath, "rb+" );

		if( fpRef )
		{
			fwrite( &lPackedFileCount, 1, sizeof(int), fpRef );

			fseek(fpRef, 0, SEEK_END);
			int nRefFileSize = ftell(fpRef);
			fseek(fpRef, 0, SEEK_SET);

			char* pRefBuffer = new char[nRefFileSize];
			ASSERT(pRefBuffer);
			char* pDelBuffer = pRefBuffer;

			fread(pRefBuffer, nRefFileSize, 1, fpRef);
			fclose( fpRef );

			//최종 유효데이터의 size를 알아낸다			

			int lRefFileCount;
			int lFolderNameSize;

			//파일이 몇개냐?
			memcpy( &lRefFileCount, pRefBuffer, sizeof(int) );
			pRefBuffer += sizeof(int);

			//폴더 이름크기
			memcpy( &lFolderNameSize, pRefBuffer, sizeof(int) );
			pRefBuffer += sizeof(int);

			//폴더 이름			
			pRefBuffer += lFolderNameSize;

			for( int lCounter = 0; lCounter < lRefFileCount; lCounter++ )
			{				
				long				lFileNameSize;
				long				lStartPos, lFileSize;

				//파일이름 크기 복사
				memcpy( &lFileNameSize, pRefBuffer, sizeof(int) );
				pRefBuffer += sizeof(int);

				//파일이름
				pRefBuffer += lFileNameSize;

				//시작위치 복사
				memcpy( &lStartPos, pRefBuffer, sizeof(int) );
				pRefBuffer += sizeof(int);

				//파일크기 복사
				memcpy( &lFileSize, pRefBuffer, sizeof(int) );
				pRefBuffer += sizeof(int);

				if ( (lStartPos + lFileSize) > nFinalValidDataSize )
					nFinalValidDataSize = lStartPos + lFileSize;
			}

			if ( pDelBuffer )
				delete [] pDelBuffer;

			//ASSERT(nFinalValidDataSize > 0);
		}


		//암호화한다.
		fpRef = fopen( strReferenceFilePath, "rb" );
		if( fpRef )
		{
			long				lFileSize;
			char				*pstrEncryptBuffer;

			//파일 크기를 얻는다.
			fseek( fpRef, 0, SEEK_END );
			lFileSize = ftell(fpRef);

			//파일크기를 얻었으니 다시 앞으로~
			fseek( fpRef, 0, SEEK_SET ); 

			pstrEncryptBuffer = new char[lFileSize];

			fread( pstrEncryptBuffer, 1, lFileSize, fpRef );

			fclose( fpRef );

			//Key는 MD5_HASH_KEY_STRING이다.
			if( m_cMD5Encrypt.EncryptString( "1111", pstrEncryptBuffer, lFileSize ) )
			{
				fpRef = fopen( strReferenceFilePath, "wb" );

				if( fpRef )
				{
					fwrite( pstrEncryptBuffer, 1, lFileSize, fpRef );

					fclose( fpRef );
				}
			}

			delete [] pstrEncryptBuffer;
		}
	}

	//백업 파일은 삭제한다.
	DeleteFile( strBackupReferenceFilePath );


	//@{ 2006/06/13 burumal
	// 패치도중 Reference.dat , Data.dat 파일이 삭제되는 버그가 종종 발생
	// 이 현상의 원인을 이 부분으로 보고 resizing 작업을 막아둠.
	/*
	// Data.Bak resize
	if ( nFinalValidDataSize > 0 )
		ResizeFileSize(strBackupDataFilePath, nFinalValidDataSize);
	*/


	// Data.Bak 를 Data.Dat 로 이름을 변경한다
	int nTry;
	for ( nTry = 0; nTry < 3; nTry++ )
		if ( MoveFile( strBackupDataFilePath, strDataFilePath ) != FALSE )
			break;

	return true;
}
//@}