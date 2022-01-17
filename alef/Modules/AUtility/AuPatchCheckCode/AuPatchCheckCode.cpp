#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "AuPatchCheckCode.h"

AuPatchCheckCode::AuPatchCheckCode()
{
	m_piData = NULL;
}

AuPatchCheckCode::~AuPatchCheckCode()
{
	if( m_piData != NULL )
		delete [] m_piData;
}

bool AuPatchCheckCode::GenerateCode()
{
	bool			bResult;

	bResult = false;

	srand( (unsigned)time(NULL) );

	if( m_piData != NULL )
	{
		delete m_piData;
	}

	m_piData = new unsigned int[MAX_LOGIN_PATCH_CODE];

	if( m_piData != NULL )
	{
		unsigned int	ulCodeHigh;
		unsigned int	ulCodeLow;
		unsigned int	ulCode;

		for( int i=0; i<MAX_LOGIN_PATCH_CODE; i++ )
		{
			while(1)
			{
				ulCodeHigh = rand();

				if( ulCodeHigh != 0 )
					break;
			}

			while(1)
			{
				ulCodeLow = rand();

				if( ulCodeLow != 0 )
					break;
			}

			ulCode = (ulCodeHigh<<16) + ulCodeLow;

			m_piData[i] = ulCode;
		}

		bResult = true;
	}

	return bResult;
}

bool AuPatchCheckCode::LoadPatchCode( char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	if( m_piData != NULL )
	{
		delete m_piData;
	}

	m_piData = new unsigned int[MAX_LOGIN_PATCH_CODE];

	if( m_piData != NULL )
	{
		FILE			*file;

		if( (file = fopen( pstrFileName, "rb" )) != NULL )
		{
			unsigned int	iFileSize;

			iFileSize = MAX_LOGIN_PATCH_CODE*sizeof(unsigned int);

			fread( m_piData, iFileSize, 1, file );

			bResult = true;

			fclose( file );
		}
	}

	return bResult;
}

bool AuPatchCheckCode::SavePatchCode( char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	if( GenerateCode() == true )
	{
		if( m_piData != NULL )
		{
			FILE			*file;

			if( (file = fopen( pstrFileName, "wb" )) != NULL )
			{
				unsigned int	iFileSize;

				iFileSize = MAX_LOGIN_PATCH_CODE*sizeof(unsigned int);

				fwrite( m_piData, iFileSize, 1, file );

				bResult = true;

				fclose( file );
			}		
		}
	}

	return bResult;
}

unsigned int AuPatchCheckCode::GetCode( int iVer )
{
	unsigned int		uiReturnData;

	uiReturnData = 0;

	if( m_piData != NULL )
	{
		if( iVer == 0 )
		{
			uiReturnData = 0;
		}
		else
		{
			uiReturnData = m_piData[iVer%MAX_LOGIN_PATCH_CODE];
		}
	}

	return uiReturnData;
}
