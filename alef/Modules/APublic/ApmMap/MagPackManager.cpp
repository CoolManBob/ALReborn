// MagPackManager.cpp: implementation of the CMagPackManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"
#include <stdio.h>
#include "aplib.h"
#include "MagPackManager.h"
#include "MagDebug.h"
#include "ApMemoryTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL PackFiles(char *outputfilename, AuList< CMagPackHeaderInfo * > *plistFiles ,
			   int (__cdecl *filecallback) (unsigned int, unsigned int) ,
			   int (__cdecl *statecallback) (unsigned int, unsigned int))
{
	char	tmpfilename[ 256 ];
	if( outputfilename == NULL || plistFiles == NULL )
	{
		TRACE( "::PackFiles() Argument incorrect!" );
		return FALSE;
	}
	BYTE	*pworking = NULL;

	// memory allocation for working
	pworking = new BYTE[ 640 * 1024 ];
	if( !pworking )
	{
		TRACE( "::PackFiles() Memory allocation error!" );
		return FALSE;
	}

	// Sequence
	// 1, Compress them each other
	// 2, Put that files into output file , and delete the remains

	int	nfilecount = plistFiles->GetCount();
	if( nfilecount <= 0 )
	{
		TRACE( "::PackFiles() The List does not have anything!" );
		return FALSE;
	}

	char	pworkfilename[ 256 ];
	HANDLE	pSourceFile = NULL;
	HANDLE	pPackedFile = NULL;
	AuNode< CMagPackHeaderInfo * >	*pPos = plistFiles->GetHeadNode();
	CMagPackHeaderInfo	*pfilename;

	DWORD	packedlength;
	DWORD	numofread;
	UINT	nSourceFileSize;
	BYTE	*pSourceBuffer = NULL;
	BYTE	*pPackedBuffer = NULL;
	int		count = 0;

	for(count = 0 ; count < nfilecount ; ++count )
	{
		// Compress them each other!
		pfilename = ( CMagPackHeaderInfo * ) plistFiles->GetNext( pPos );
		wsprintf( pworkfilename , MAGPACKTMPFILENAME , count );
		pSourceFile = CreateFile(
			pfilename->filename,
			GENERIC_READ,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,0);
		pPackedFile	= CreateFile( 
			pworkfilename ,
			GENERIC_WRITE,
			0,
			0,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,0);

		if( pSourceFile == INVALID_HANDLE_VALUE)
		{
			TRACE( "::PackFiles() Some error found in opening file %s!" , pfilename );
			if( pworking )
				delete [] pworking;
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}
		else if( pPackedFile == INVALID_HANDLE_VALUE )
		{
			TRACE( "::PackFiles() Some error found in opening temporary file %s!" , pfilename );
			if( pworking )
				delete [] pworking;
			CloseHandle( pSourceFile );
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		// Get the source file size
		nSourceFileSize = GetFileSize( pSourceFile , NULL );

		if( nSourceFileSize <= 0 )
		{			
			TRACE( "::PackFiles() Source file size error (%s)" , pfilename->filename );
			if( pworking ) delete [] pworking;
			CloseHandle( pSourceFile );
			CloseHandle( pPackedFile );
			for( count = 0 ; count < nfilecount ; count++ )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		// Get Memory
		pSourceBuffer= new BYTE[ nSourceFileSize ];
		pPackedBuffer= new BYTE[ ( ( nSourceFileSize * 9 ) / 8 ) + 16 ];

		if( !pSourceBuffer || !pPackedBuffer)
		{
			TRACE( "::PackFiles() Reading Buffer Allocation Error (%s)" , pfilename->filename );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			if( pworking ) delete [] pworking;
			CloseHandle( pSourceFile );
			CloseHandle( pPackedFile );
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		// Read the Source File

		if( 
			!ReadFile( 
				pSourceFile		,
				pSourceBuffer	,
				nSourceFileSize	,
				&numofread		,
				NULL			)
			||
			numofread != nSourceFileSize 
			)
		{
			TRACE( "::PackFiles() File Reading Error (%s)" , pfilename->filename );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			if( pworking ) delete [] pworking;
			CloseHandle( pSourceFile );
			CloseHandle( pPackedFile );
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		// Pack the buffer
		if( filecallback ) filecallback( (UINT) count + 1 , nSourceFileSize );
		packedlength = aP_pack(
			pSourceBuffer,
			pPackedBuffer,
			nSourceFileSize,
			pworking,
			statecallback );


		// Write binary info to file
		UINT	xorreturned = XORReturn( nSourceFileSize );
		WriteFile(
			pPackedFile,
			&xorreturned, // &nSourceFileSize, 암호화
			sizeof(unsigned int),
			&numofread,
			NULL);
		WriteFile(
			pPackedFile,
			pPackedBuffer,
			packedlength,
			&numofread,
			NULL);
		
		if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
		if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
		CloseHandle( pSourceFile );
		CloseHandle( pPackedFile );
		pSourceFile = INVALID_HANDLE_VALUE;
		pPackedFile = INVALID_HANDLE_VALUE;

		// Store the packed size
		pfilename->size = packedlength + sizeof( UINT ) ;
	}
	// Pack Each file Completed

	// Prepare the MPF Format header
	// Header Txt ( 50 )
	// Filenamesize( 1 ) Filename ( Filenamesize ) PackedSize ( 4 ) * File count
	BYTE	headerBuffer[ MAGPACKFILEHEADERBUFFERSIZE ];
	int		bufferWrited = 0;
	char	drive[ 256 ] , dir[ 256 ] , fname[ 256 ] , ext[ 256 ];
	memset( ( void * ) headerBuffer , 0 , MAGPACKFILEHEADERBUFFERSIZE * sizeof ( BYTE ) );
	strncpy( ( char * ) headerBuffer , MAGPACKFILEHEADERTXT , MAGPACKFILEHEADERBUFFERSIZE );
	bufferWrited += MAGPACKFILEHEADERSIZE;

	pPos = plistFiles->GetHeadNode();
	for(int count = 0 ; count < nfilecount ; ++count )
	{
		if( bufferWrited > MAGPACKFILEHEADERBUFFERSIZE - 256 - 5 )
		{
			TRACE( "::PackFiles() MPF Header size overflow!" );
			if( pworking )
				delete [] pworking;
			if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
			if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			for( count = 0 ; count < nfilecount ; count++ )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}
		pfilename = ( CMagPackHeaderInfo * ) plistFiles->GetNext( pPos );
		// Get File name from path name
		_splitpath( pfilename->filename , drive , dir , fname , ext );
		wsprintf( pfilename->filename , "%s%s" , fname , ext );
		headerBuffer[ bufferWrited ] = (BYTE) strlen( pfilename->filename );
		++bufferWrited ;
		strcpy( ( char * ) ( headerBuffer + bufferWrited ) , pfilename->filename );
		bufferWrited += (int)strlen( pfilename->filename );
		*( UINT * ) ( headerBuffer + bufferWrited ) = XORReturn( pfilename->size ) ;// 암호화
		bufferWrited += sizeof( UINT );
	}

	++bufferWrited  ; // For End Null

	// Now open the output file
	HANDLE	pOutputFile;
	pOutputFile	= CreateFile( 
		outputfilename ,
		GENERIC_WRITE,
		0,
		0,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,0);

	if( pOutputFile == INVALID_HANDLE_VALUE)
	{
		TRACE( "::PackFiles() Some error found in opening file %s!" , outputfilename );
		if( pworking )
			delete [] pworking;
		if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
		if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
		if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
		if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
		for( count = 0 ; count < nfilecount ; count++ )
		{
			wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
			DeleteFile( tmpfilename );
		}
		return FALSE;
	}

	// Write the MPF Header
	WriteFile(
		pOutputFile,
		headerBuffer,
		bufferWrited,
		&numofread,
		NULL);

	// Write the binary with parity of all the file
	pPos = plistFiles->GetHeadNode();
	for( count = 0 ; count < nfilecount ; ++count )
	{
		pfilename = ( CMagPackHeaderInfo * ) plistFiles->GetNext( pPos );
		wsprintf( pworkfilename , MAGPACKTMPFILENAME , count );
		pPackedFile = CreateFile(
			pworkfilename,
			GENERIC_READ,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,0);
		if( pPackedFile == INVALID_HANDLE_VALUE )
		{
			TRACE( "::PackFiles() Some error found in opening temporary file %s!" , pworkfilename );
			if( pworking )
				delete [] pworking;
			if( pOutputFile != INVALID_HANDLE_VALUE ) CloseHandle( pOutputFile );
			if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
			if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		UINT	nPackedFileSize = GetFileSize( pPackedFile , NULL );

		if( nPackedFileSize <= 0 )
		{			
			TRACE( "::PackFiles() tmp file size error (%s)" , pworkfilename );
			if( pworking )
				delete [] pworking;
			if( pOutputFile != INVALID_HANDLE_VALUE ) CloseHandle( pOutputFile );
			if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
			if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		// Get Memory
		pSourceBuffer= new BYTE[ nPackedFileSize ];

		if( !pSourceBuffer )
		{
			TRACE( "::PackFiles() Reading Buffer Allocation Error (%s)" , pworkfilename );
			if( pworking )
				delete [] pworking;
			if( pOutputFile != INVALID_HANDLE_VALUE ) CloseHandle( pOutputFile );
			if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
			if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

		if( 
			!ReadFile( 
				pPackedFile,
				pSourceBuffer,
				nPackedFileSize,
				&numofread,
				NULL)
			||
			numofread != nPackedFileSize 
			)
		{
			TRACE( "::PackFiles() File Reading Error (%s)" , pworkfilename );
			if( pworking )
				delete [] pworking;
			if( pOutputFile != INVALID_HANDLE_VALUE ) CloseHandle( pOutputFile );
			if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
			if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
			if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
			if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
			for( count = 0 ; count < nfilecount ; ++count )
			{
				wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
				DeleteFile( tmpfilename );
			}
			return FALSE;
		}

//		UINT	parity = 0xffffffff ; // GetParity( pSourceBuffer , nSourceFileSize );
		UINT	parity = GetParity( pSourceBuffer + 4 , pfilename->size );// XORReturn( *( UINT * ) pSourceBuffer ) );

		WriteFile(
			pOutputFile,
			pSourceBuffer,
			nPackedFileSize,
			&numofread,
			NULL);
		WriteFile(
			pOutputFile,
			&parity,
			sizeof(unsigned int),
			&numofread,
			NULL);

		CloseHandle( pPackedFile );
		pPackedFile = INVALID_HANDLE_VALUE;
		delete [] pSourceBuffer;
		pSourceBuffer = NULL;
	}

	// Close the File

	// Clean Up
	if( pworking )
		delete [] pworking;
	if( pOutputFile != INVALID_HANDLE_VALUE ) CloseHandle( pOutputFile );
	if( pSourceFile != INVALID_HANDLE_VALUE ) CloseHandle( pSourceFile );
	if( pPackedFile != INVALID_HANDLE_VALUE ) CloseHandle( pPackedFile );
	if( pSourceBuffer ) { delete [] pSourceBuffer; pSourceBuffer = NULL; }
	if( pPackedBuffer ) { delete [] pPackedBuffer; pPackedBuffer = NULL; }
	pPos = plistFiles->GetHeadNode();
	for( count = 0 ; count < nfilecount ; ++count )
	{
		wsprintf( tmpfilename , MAGPACKTMPFILENAME , count );
		DeleteFile( tmpfilename );
	}
	return TRUE;
}

BOOL PackFilesBinary( char * outputfilename , AuList< CMagPackHeaderBinaryInfo * > *plistFiles ,
			   int (__cdecl *filecallback ) (unsigned int, unsigned int) ,
			   int (__cdecl *statecallback ) (unsigned int, unsigned int))
// Compress , CMagPackHeaderInfo's Binary Infomation..
{
	if( outputfilename == NULL || plistFiles == NULL )
	{
		TRACE( "::PackFiles() Argument incorrect!" );
		return FALSE;
	}
	BYTE	*pworking = NULL;

	// memory allocation for working
	pworking = new BYTE[ 640 * 1024 ];
	if( !pworking )
	{
		TRACE( "::PackFiles() Memory allocation error!" );
		return FALSE;
	}

	// Sequence
	// 1, Compress them each other
	// 2, Put that files into output file , and delete the remains

	int	nfilecount = plistFiles->GetCount();
	if( nfilecount <= 0 )
	{
		TRACE( "::PackFiles() The List does not have anything!" );
		return FALSE;
	}

	AuNode< CMagPackHeaderBinaryInfo * >	*pPos		= plistFiles->GetHeadNode();
	CMagPackHeaderBinaryInfo				*pFileInfo	;

	DWORD	packedlength;
	DWORD	numofread;
	UINT	nSourceFileSize;
	BYTE	*pSourceBuffer = NULL;
	BYTE	*pPackedBuffer = NULL;

	for( int count = 0 ; count < nfilecount ; ++count )
	{
		// Compress them each other!
		pFileInfo = ( CMagPackHeaderBinaryInfo * ) plistFiles->GetNext( pPos );

		// Get the source file size
		nSourceFileSize = pFileInfo->nSourceSize;

		// Get Memory
		VERIFY( pPackedBuffer	= new BYTE[ ( ( nSourceFileSize * 9 ) / 8 ) + 16 ] );

		// Pack the buffer
		if( filecallback ) filecallback( (UINT) count + 1 , nSourceFileSize );
		packedlength = aP_pack(
			pFileInfo->pSourceBinary	,
			pPackedBuffer				,
			nSourceFileSize				,
			pworking					,
			statecallback				);

		// Store the packed size

		pFileInfo->pPackedData	= pPackedBuffer	;
		pFileInfo->size			= packedlength	+ sizeof ( UINT );
	}
	// Pack Each file Completed

	// Prepare the MPF Format header
	// Header Txt ( 50 )
	// Filenamesize( 1 ) Filename ( Filenamesize ) PackedSize ( 4 ) * File count
	BYTE	headerBuffer[ MAGPACKFILEHEADERBUFFERSIZE ];
	int		bufferWrited = 0;
	char	drive[ 256 ] , dir[ 256 ] , fname[ 256 ] , ext[ 256 ];
	memset( ( void * ) headerBuffer , 0 , MAGPACKFILEHEADERBUFFERSIZE * sizeof ( BYTE ) );
	strncpy( ( char * ) headerBuffer , MAGPACKFILEHEADERTXT , MAGPACKFILEHEADERBUFFERSIZE );
	bufferWrited += MAGPACKFILEHEADERSIZE;

	pPos = plistFiles->GetHeadNode();
	int		count = 0;
	for( count = 0 ; count < nfilecount ; ++count )
	{
		if( bufferWrited > MAGPACKFILEHEADERBUFFERSIZE - 256 - 5 )
		{
			TRACE( "::PackFiles() MPF Header size overflow!" );
			if( pworking )
				delete [] pworking;
			return FALSE;
		}
		pFileInfo = ( CMagPackHeaderBinaryInfo * ) plistFiles->GetNext( pPos );
		// Get File name from path name
		_splitpath( pFileInfo->filename , drive , dir , fname , ext );
		wsprintf( pFileInfo->filename , "%s%s" , fname , ext );
		headerBuffer[ bufferWrited ] = (BYTE) strlen( pFileInfo->filename );
		++bufferWrited ;
		strcpy( ( char * ) ( headerBuffer + bufferWrited ) , pFileInfo->filename );
		bufferWrited += (int)strlen( pFileInfo->filename );
		*( UINT * ) ( headerBuffer + bufferWrited ) = XORReturn( pFileInfo->size  ) ;// 암호화
		bufferWrited += sizeof( UINT );
	}

	++bufferWrited  ; // For End Null

	// Now open the output file
	HANDLE	pOutputFile;
	pOutputFile	= CreateFile( 
		outputfilename			,
		GENERIC_WRITE			,
		0						,
		0						,
		CREATE_ALWAYS			,
		FILE_ATTRIBUTE_NORMAL	,
		0						);

	if( pOutputFile == INVALID_HANDLE_VALUE)
	{
		TRACE( "::PackFiles() Some error found in opening file %s!" , outputfilename );
		if( pworking )
			delete [] pworking;
		return FALSE;
	}

	// Write the MPF Header
	WriteFile(
		pOutputFile		,
		headerBuffer	,
		bufferWrited	,
		&numofread		,
		NULL			);

	// Write the binary with parity of all the file
	pPos = plistFiles->GetHeadNode();
	for( count = 0 ; count < nfilecount ; ++count )
	{
		pFileInfo = ( CMagPackHeaderBinaryInfo * ) plistFiles->GetNext( pPos );

		UINT	nPackedFileSize	= pFileInfo->size ;
		UINT	parity			= GetParity( pFileInfo->pPackedData , pFileInfo->size );
		UINT	xorreturned		= XORReturn( pFileInfo->nSourceSize );

		// Size..
		WriteFile(
			pOutputFile			,
			&xorreturned		,
			sizeof(unsigned int),
			&numofread			,
			NULL				);
		// Binary..
		WriteFile(
			pOutputFile				,
			pFileInfo->pPackedData	,
			nPackedFileSize	- 4		,
			&numofread				,
			NULL					);
		// Parity..
		WriteFile(
			pOutputFile			,
			&parity				,
			sizeof(unsigned int),
			&numofread			,
			NULL				);

		delete [] pFileInfo->pPackedData;
		pFileInfo->pPackedData = NULL;
	}

	// Close the File

	// Clean Up
	if( pworking )
		delete [] pworking;
	if( pOutputFile != INVALID_HANDLE_VALUE ) CloseHandle( pOutputFile );
	return TRUE;
}