#include "AuIPBlockingManager.h"
#include "ApMemoryTracker.h"

AuIPBlockingManager::AuIPBlockingManager()
{
	m_csBlockIP.InitializeObject( sizeof(AuIPBlockingData *), AUIPBLOCKING_MAX_IP_COUNT );
	m_csPCRoomIP.InitializeObject( sizeof(AuIPBlockingData *), AUIPBLOCKING_MAX_IP_COUNT );
}

AuIPBlockingManager::~AuIPBlockingManager()
{
	//할당된 메모리를 전부 날린다.
	AuIPBlockingData		**ppcsData;

	INT32				nIndex;

	nIndex = 0;

	while( 1 )
	{
		ppcsData = (AuIPBlockingData **)m_csBlockIP.GetObjectSequence( &nIndex );

		if( ppcsData != NULL )
		{
			if( *ppcsData )
				delete *ppcsData;
		}
		else
		{
			break;
		}
	}

	while( 1 )
	{
		ppcsData = (AuIPBlockingData **)m_csPCRoomIP.GetObjectSequence( &nIndex );

		if( ppcsData != NULL )
		{
			if( *ppcsData )
				delete *ppcsData;
		}
		else
		{
			break;
		}
	}
}

BOOL AuIPBlockingManager::AddIP( ApAdmin *pcsApAdmin, char *pstrIP, unsigned long lMin, unsigned long lMax )
{
	BOOL			bResult;

	bResult = FALSE;

	AuIPBlockingData		*pcsData;

	pcsData = new AuIPBlockingData;

	pcsData->m_lMinIP = lMin;
	pcsData->m_lMaxIP = lMax;

	if( pcsApAdmin->AddObject( (void **)&pcsData, pstrIP ) )
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL AuIPBlockingManager::SearchIPFromBlock( char *pstrIP )
{
	AuIPBlockingData		**ppcsData;

	BOOL			bResult;
	int				iIndex;

	bResult = FALSE;
	iIndex = 0;

	ppcsData = (AuIPBlockingData **)m_csBlockIP.GetObject( pstrIP );

	if( ppcsData )
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL AuIPBlockingManager::SearchIPFromPCRoom(char *pstrIP )
{
	AuIPBlockingData		**ppcsData;

	BOOL			bResult;
	int				iIndex;

	bResult = FALSE;
	iIndex = 0;

	ppcsData = (AuIPBlockingData **)m_csPCRoomIP.GetObject( pstrIP );

	if( ppcsData )
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL AuIPBlockingManager::LoadBlockIPFromFile( char *pstrFileName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, TRUE ) )
	{
		int					iRow;
		unsigned long		ulMinIP;
		unsigned long		ulMaxIP;

		char				*pstrMinIP;
		char				*pstrMaxIP;

		iRow = m_csExcelTxtLib.GetRow();

		for( int iCounter=0; iCounter<iRow; iCounter++ )
		{
			pstrMinIP = m_csExcelTxtLib.GetData( 0, iCounter );
			pstrMaxIP = m_csExcelTxtLib.GetData( 1, iCounter );

			if( pstrMinIP && pstrMinIP )
			{
				IN_ADDR		inMinAddr, inMaxAddr;
				BOOL		bIPLoadData;
				char		strIP[80];

				inMinAddr.S_un.S_addr = inet_addr( pstrMinIP );
				inMaxAddr.S_un.S_addr = inet_addr( pstrMaxIP );

				bIPLoadData = TRUE;

				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b1 && (unsigned long)inMinAddr.S_un.S_un_b.s_b1 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b2 && (unsigned long)inMinAddr.S_un.S_un_b.s_b2 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b3 && (unsigned long)inMinAddr.S_un.S_un_b.s_b3 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b4 && (unsigned long)inMinAddr.S_un.S_un_b.s_b4 <= 0 )
					bIPLoadData = FALSE;

				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b1 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b1 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b2 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b2 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b3 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b3 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b4 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b4 <= 0 )
					bIPLoadData = FALSE;

				if( (unsigned long)inMinAddr.S_un.S_un_b.s_b1 == 255 && 
					(unsigned long)inMinAddr.S_un.S_un_b.s_b2 == 255 && 
					(unsigned long)inMinAddr.S_un.S_un_b.s_b3 == 255 && 
					(unsigned long)inMinAddr.S_un.S_un_b.s_b4 == 255 )
				{
					bIPLoadData = FALSE;
				}

				if( (unsigned long)inMaxAddr.S_un.S_un_b.s_b1 == 255 && 
					(unsigned long)inMaxAddr.S_un.S_un_b.s_b2 == 255 && 
					(unsigned long)inMaxAddr.S_un.S_un_b.s_b3 == 255 && 
					(unsigned long)inMaxAddr.S_un.S_un_b.s_b4 == 255 )
				{
					bIPLoadData = FALSE;
				}

				if( bIPLoadData == FALSE )
				{
					printf( "[IP Load Error] MinIP:%s, MaxIP :%s\n", pstrMinIP, pstrMaxIP );
					printf( "Press any key to continue.\n", pstrMinIP, pstrMaxIP );
					getchar();
					continue;
				}

				ulMinIP = 0;
				ulMaxIP = 0;

				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b1) << 24;
				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b2) << 16;
				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b3) << 8;
				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b4);

				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b1) << 24;
				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b2) << 16;
				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b3) << 8;
				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b4);

				for( unsigned long lIP = (unsigned long)inMinAddr.S_un.S_un_b.s_b4; lIP <= (unsigned long)inMaxAddr.S_un.S_un_b.s_b4; lIP++ )
				{
					sprintf( strIP, "%d.%d.%d.%d", (unsigned long)inMinAddr.S_un.S_un_b.s_b1,
													(unsigned long)inMinAddr.S_un.S_un_b.s_b2,
													(unsigned long)inMinAddr.S_un.S_un_b.s_b3,
													lIP
													);

					if( AddIP( &m_csBlockIP, strIP, ulMinIP, ulMaxIP ) )
					{
						bResult = TRUE;
					}
				}
			}
		}
	}
	else
	{
		printf( "[Error] %s Load Error!!\n", pstrFileName );
		bResult = FALSE;
	}

	return bResult;
}

BOOL AuIPBlockingManager::LoadPCRoomIPFromFile( char *pstrFileName )
{
	BOOL			bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, TRUE ) )
	{
		int					iRow;
		unsigned long		ulMinIP;
		unsigned long		ulMaxIP;

		char				*pstrMinIP;
		char				*pstrMaxIP;

		iRow = m_csExcelTxtLib.GetRow();

		for( int iCounter=0; iCounter<iRow; iCounter++ )
		{
			pstrMinIP = m_csExcelTxtLib.GetData( 0, iCounter );
			pstrMaxIP = m_csExcelTxtLib.GetData( 1, iCounter );

			if( pstrMinIP && pstrMaxIP )
			{
				IN_ADDR		inMinAddr, inMaxAddr;
				BOOL		bIPLoadData;
				char		strIP[80];

				inMinAddr.S_un.S_addr = inet_addr( pstrMinIP );
				inMaxAddr.S_un.S_addr = inet_addr( pstrMaxIP );

				bIPLoadData = TRUE;

				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b1 && (unsigned long)inMinAddr.S_un.S_un_b.s_b1 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b2 && (unsigned long)inMinAddr.S_un.S_un_b.s_b2 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b3 && (unsigned long)inMinAddr.S_un.S_un_b.s_b3 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMinAddr.S_un.S_un_b.s_b4 && (unsigned long)inMinAddr.S_un.S_un_b.s_b4 <= 0 )
					bIPLoadData = FALSE;

				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b1 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b1 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b2 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b2 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b3 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b3 <= 0 )
					bIPLoadData = FALSE;
				if( 255 < (unsigned long)inMaxAddr.S_un.S_un_b.s_b4 && (unsigned long)inMaxAddr.S_un.S_un_b.s_b4 <= 0 )
					bIPLoadData = FALSE;

				if( (unsigned long)inMinAddr.S_un.S_un_b.s_b1 == 255 && 
					(unsigned long)inMinAddr.S_un.S_un_b.s_b2 == 255 && 
					(unsigned long)inMinAddr.S_un.S_un_b.s_b3 == 255 && 
					(unsigned long)inMinAddr.S_un.S_un_b.s_b4 == 255 )
				{
					bIPLoadData = FALSE;
				}

				if( (unsigned long)inMaxAddr.S_un.S_un_b.s_b1 == 255 && 
					(unsigned long)inMaxAddr.S_un.S_un_b.s_b2 == 255 && 
					(unsigned long)inMaxAddr.S_un.S_un_b.s_b3 == 255 && 
					(unsigned long)inMaxAddr.S_un.S_un_b.s_b4 == 255 )
				{
					bIPLoadData = FALSE;
				}

				if( bIPLoadData == FALSE )
				{
					printf( "[IP Load Error] MinIP:%s, MaxIP :%s\n", pstrMinIP, pstrMaxIP );
					printf( "Press any key to continue.\n", pstrMinIP, pstrMaxIP );
					getchar();
					continue;
				}

				ulMinIP = 0;
				ulMaxIP = 0;

				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b1) << 24;
				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b2) << 16;
				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b3) << 8;
				ulMinIP += ((unsigned long)inMinAddr.S_un.S_un_b.s_b4);

				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b1) << 24;
				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b2) << 16;
				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b3) << 8;
				ulMaxIP += ((unsigned long)inMaxAddr.S_un.S_un_b.s_b4);

				for( unsigned long lIP = (unsigned long)inMinAddr.S_un.S_un_b.s_b4; lIP <= (unsigned long)inMaxAddr.S_un.S_un_b.s_b4; lIP++ )
				{
					sprintf( strIP, "%d.%d.%d.%d", (unsigned long)inMinAddr.S_un.S_un_b.s_b1,
													(unsigned long)inMinAddr.S_un.S_un_b.s_b2,
													(unsigned long)inMinAddr.S_un.S_un_b.s_b3,
													lIP
													);

					if( AddIP( &m_csPCRoomIP, strIP, ulMinIP, ulMaxIP ) )
					{
						bResult = TRUE;
					}
				}
			}
		}
	}
	else
	{
		printf( "[Error] %s Load Error!!\n", pstrFileName );
		bResult = FALSE;
	}

	return bResult;
}
