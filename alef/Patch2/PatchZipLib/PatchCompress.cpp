
/* zlib 를 사용하는 예 */
//#include <afxwin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "zlib.h"
#include "PatchCompress.h"

FILE* CCompress::OpenFileMakeDir( char *pstrFileName, char *pstrOption )
{
	FILE			*fpTemp;

	fpTemp = fopen( pstrFileName, pstrOption );

	if( fpTemp == NULL )
	{
		int		iErrorCode;

		iErrorCode = GetLastError();
		
		//ErrorCode가 0x03이면 폴더가 없는경우!! 폴더를 만들어준다.
		if( iErrorCode == 0x03 )
		{
			int				iCurrentPos;
			int				iDPathSize;
			char			strDir[256];

			iCurrentPos = 0;
			iDPathSize = strlen( pstrFileName );
			memset( strDir, 0, 256 );

			for( int i=0; i<iDPathSize; i++ )
			{
				strDir[iCurrentPos] = pstrFileName[i];
				iCurrentPos++;

				if( pstrFileName[i] == '\\' )
				{
					CreateDirectory( strDir, NULL );
					
					//파일을 복사한다.
					fpTemp =fopen( pstrFileName, pstrOption );

					if( fpTemp != NULL )
					{
						break;
					}
					else
					{
						if( GetLastError() == 0x03 )
						{
							continue;
						}
						else
						{
							fpTemp = NULL;
							break;
						}
					}
				}
			}			
		}
		else
		{
			fpTemp = NULL;
		}
	}

	return fpTemp;
}

void CCompress::compressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName )
{
	z_stream z;                     /* 라이브러리와 교환하기 위한 구조체 */
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	char dest_filename[256];
	char strCurrentDir[256];

	unsigned int count, flush, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//현재 디렉토리를 저장한다.
	GetCurrentDirectory( 256, strCurrentDir );

	//현재 디렉토리를 지정된 디렉토리(psrtDestDir)로 지정한다.
	SetCurrentDirectory( pstrDestDir );

	//파일 네임 설정
	sprintf( dest_filename, "%s\\%s.jzp", pstrDestDir, pstrDestFileName );

	m_fpSourceFile = fopen(src_filename, "rb");
	if(m_fpSourceFile == NULL)
	{
		printf("%s open failed\n", src_filename);
		exit(1);
	}

	m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );
	if(m_fpDestFile == NULL)
	{
		printf("%s open failed\n", dest_filename);
		exit(1);
	}

	//압축을 하기 위한 환경 설정
	INBUFSIZE = filelength(fileno(m_fpSourceFile));
	OUTBUFSIZE = INBUFSIZE+1024;
	inbuf = (unsigned char *)malloc(INBUFSIZE);
	outbuf = (unsigned char *)malloc(OUTBUFSIZE);
	
    /* 모든 메모리 메니지먼트를 라이브러리에 맡긴다 */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 초기화 */
    /* 제2 인수는 압축의도 합. 0~9 의 범위의 정수로, 0 은 무압축 */
    /* Z_DEFAULT_COMPRESSION (= 6)가 표준 */
    if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK) {
        fprintf(stderr, "deflateInit: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

    z.avail_in = 0;             /* 입력 버퍼중의 데이터의 바이트수 */
    z.next_out = outbuf;        /* 출력 포인터 */
    z.avail_out = OUTBUFSIZE;    /* 출력 버퍼의 사이즈 */

    /* 통상은 deflate()의 제2 인수는 Z_NO_FLUSH 로 해 호출한다 */
    flush = Z_NO_FLUSH;

    while (1) {
        if (z.avail_in == 0) {  /* 입력이 다하면 */
            z.next_in = inbuf;  /* 입력 포인터를 입력 버퍼의 선두에 */
            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* 데이터를 읽어들인다 */

            /* 입력이 마지막에 되면(자) deflate()의 제2 인수는 Z_FINISH 로 한다 */
            if (z.avail_in < INBUFSIZE) flush = Z_FINISH;
        }
        status = deflate(&z, flush); /* 압축한다 */
        if (status == Z_STREAM_END) break; /* 완료 */
        if (status != Z_OK) {   /* 에러 */
            fprintf(stderr, "deflate: %s\n", (z.msg) ?  z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* 출력 버퍼가 다하면 */
            /* 정리해 써낸다 */
            if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* 출력 버퍼 잔량을 바탕으로 되돌린다 */
            z.avail_out = OUTBUFSIZE; /* 출력 포인터를 바탕으로 되돌린다 */
        }
    }

    /* 나머지를 토해낸다 */
    if ((count = OUTBUFSIZE - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* 뒤처리 */
    if (deflateEnd(&z) != Z_OK) {
        fprintf(stderr, "deflateEnd: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

	free(inbuf);
	free(outbuf);
	fclose(m_fpSourceFile);
	fclose(m_fpDestFile);

	//원래 디렉토리로 돌린다.
	SetCurrentDirectory( strCurrentDir );
}

void CCompress::decompressFile(const char* src_filename, char *pstrDestDir, char *pstrDestFileName )
{
	z_stream z;
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	char			dest_filename[256];
	char			strCurrentDir[256];

	unsigned int count, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//현재 디렉토리를 저장한다.
	GetCurrentDirectory( 256, strCurrentDir );

	//현재 디렉토리를 지정된 디렉토리(psrtDestDir)로 지정한다.
	SetCurrentDirectory( pstrDestDir );

	//파일 네임 설정
	int				iDestPathSize;

//	sprintf( dest_filename, "%s%s", pstrDestDir, pstrDestFileName );
	sprintf( dest_filename, "%s\\%s", pstrDestDir, pstrDestFileName );

	//뒤에붙은 jzp를 지운다.
	iDestPathSize = strlen(dest_filename);
	memset( &dest_filename[iDestPathSize-4], 0, 3 );

	m_fpSourceFile = fopen(src_filename, "rb");
	if(m_fpSourceFile == NULL)
	{
		printf("%s open failed\n", src_filename);
		exit(1);
	}

	m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );
	if(m_fpDestFile == NULL)
	{
		printf("%s open failed\n", dest_filename);
		exit(1);
	}

	//압축을 하기 위한 환경 설정
	INBUFSIZE = filelength(fileno(m_fpSourceFile));
	OUTBUFSIZE = INBUFSIZE;
	inbuf = (unsigned char *)malloc(INBUFSIZE);
	outbuf = (unsigned char *)malloc(OUTBUFSIZE);
	
    /* 모든 메모리 메니지먼트를 라이브러리에 맡긴다 */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 초기화 */
    z.next_in = Z_NULL;
    z.avail_in = 0;
    if (inflateInit(&z) != Z_OK) {
        fprintf(stderr, "inflateInit: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

    z.next_out = outbuf;        /* 출력 포인터 */
    z.avail_out = OUTBUFSIZE;    /* 출력 버퍼 잔량 */
    status = Z_OK;

    while (status != Z_STREAM_END) {
        if (z.avail_in == 0) {  /* 입력 잔량이 제로가 되면 */
            z.next_in = inbuf;  /* 입력 포인터를 바탕으로 되돌린다 */
            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* 데이터를 읽는다 */
        }
        status = inflate(&z, Z_NO_FLUSH); /* 전개 */
        if (status == Z_STREAM_END) break; /* 완료 */
        if (status != Z_OK) {   /* 에러 */
            fprintf(stderr, "inflate: %s\n", (z.msg) ?  z.msg : "???");
            exit(1);
        }
        if (z.avail_out == 0) { /* 출력 버퍼가 다하면 */
            /* 정리해 써낸다 */
            if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
                fprintf(stderr, "Write error\n");
                exit(1);
            }
            z.next_out = outbuf; /* 출력 포인터를 바탕으로 되돌린다 */
            z.avail_out = OUTBUFSIZE; /* 출력 버퍼 잔량을 바탕으로 되돌린다 */
        }
    }

    /* 나머지를 토해낸다 */
    if ((count = OUTBUFSIZE - z.avail_out) != 0) {
        if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
            fprintf(stderr, "Write error\n");
            exit(1);
        }
    }

    /* 뒤처리 */
    if (inflateEnd(&z) != Z_OK) {
        fprintf(stderr, "inflateEnd: %s\n", (z.msg) ?  z.msg : "???");
        exit(1);
    }

	fclose( m_fpDestFile );
	free( outbuf );

	//원래 디렉토리로 돌린다.
	SetCurrentDirectory( strCurrentDir );
}

void CCompress::decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char *pstrDestDir, char *pstrDestFileName )
{
	z_stream z;
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	char			dest_filename[256];
	char			strCurrentDir[256];
	int				iCurrentPos = 0;

	unsigned int count, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//현재 디렉토리를 저장한다.
	GetCurrentDirectory( 256, strCurrentDir );

	//현재 디렉토리를 지정된 디렉토리(psrtDestDir)로 지정한다.
	SetCurrentDirectory( pstrDestDir );

	//파일 네임 설정
	int				iDestPathSize;

	sprintf( dest_filename, "%s\\%s", pstrDestDir, pstrDestFileName );

	//뒤에붙은 jzp를 지운다.
	iDestPathSize = strlen(dest_filename);
	memset( &dest_filename[iDestPathSize-4], 0, 3 );

	//만약 복사할곳에 파일이 Read-only 속성을 가졌을지도 모르니 Normal로 만들어서 처리한다.
	SetFileAttributes( pstrDestFileName, FILE_ATTRIBUTE_NORMAL );

	m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );

	if(m_fpDestFile != NULL)
	{
		//압축을 하기 위한 환경 설정
		INBUFSIZE = iCompressBlockSize;
		OUTBUFSIZE = INBUFSIZE;
		inbuf = (unsigned char *)pstrCompressBlock;
		outbuf = (unsigned char *)malloc(OUTBUFSIZE);
		
		/* 모든 메모리 메니지먼트를 라이브러리에 맡긴다 */
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;

		/* 초기화 */
		z.next_in = Z_NULL;
		z.avail_in = 0;
		if (inflateInit(&z) != Z_OK) {
			fprintf(stderr, "inflateInit: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		z.next_out = outbuf;        /* 출력 포인터 */
		z.avail_out = OUTBUFSIZE;    /* 출력 버퍼 잔량 */
		status = Z_OK;

		while (status != Z_STREAM_END) {
			if (z.avail_in == 0) {  /* 입력 잔량이 제로가 되면 */
				z.next_in = inbuf;  /* 입력 포인터를 바탕으로 되돌린다 */
	//            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* 데이터를 읽는다 */
				z.avail_in = INBUFSIZE;
			}
			status = inflate(&z, Z_NO_FLUSH); /* 전개 */
			if (status == Z_STREAM_END) break; /* 완료 */
			if (status != Z_OK) {   /* 에러 */
				fprintf(stderr, "inflate: %s\n", (z.msg) ?  z.msg : "???");
				exit(1);
			}
			if (z.avail_out == 0) { /* 출력 버퍼가 다하면 */
				/* 정리해 써낸다 */
				if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
					fprintf(stderr, "Write error\n");
					exit(1);
				}
				z.next_out = outbuf; /* 출력 포인터를 바탕으로 되돌린다 */
				z.avail_out = OUTBUFSIZE; /* 출력 버퍼 잔량을 바탕으로 되돌린다 */
			}
		}

		/* 나머지를 토해낸다 */
		if ((count = OUTBUFSIZE - z.avail_out) != 0) {
			if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
				fprintf(stderr, "Write error\n");
				exit(1);
			}
		}

		/* 뒤처리 */
		if (inflateEnd(&z) != Z_OK) {
			fprintf(stderr, "inflateEnd: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		//
		fclose( m_fpDestFile );
		free( outbuf );
	}
	else
	{
		//이유를 알려준다.
	}

	//원래 디렉토리로 돌린다.
	SetCurrentDirectory( strCurrentDir );
}

//@{ 2006/04/10 burumal
int CCompress::decompressMemory(const char* pstrCompressBlock, int iCompressBlockSize, char** ppDestMem)
{
	z_stream z;
	unsigned char* inbuf = NULL;
	unsigned char* outbuf = NULL;

	//char			dest_filename[256];
	//char			strCurrentDir[256];
	//int			iCurrentPos = 0;

	unsigned int count, status;
	unsigned long INBUFSIZE;
	unsigned long OUTBUFSIZE;

	//@{ 2006/04/11 burumal
	bool bUseOutputMemPool = false;
	//@}

	if ( !pstrCompressBlock || !ppDestMem )
		return 0;

	*ppDestMem = NULL;

	long lMemAllocSize = 0;

	//현재 디렉토리를 저장한다.
	//GetCurrentDirectory( 256, strCurrentDir );

	//현재 디렉토리를 지정된 디렉토리(psrtDestDir)로 지정한다.
	//SetCurrentDirectory( pstrDestDir );

	//파일 네임 설정
	int				iDestPathSize;

	//sprintf( dest_filename, "%s\\%s", pstrDestDir, pstrDestFileName );

	//뒤에붙은 jzp를 지운다.
	//iDestPathSize = strlen(dest_filename);
	//memset( &dest_filename[iDestPathSize-4], 0, 3 );

	//만약 복사할곳에 파일이 Read-only 속성을 가졌을지도 모르니 Normal로 만들어서 처리한다.
	//SetFileAttributes( pstrDestFileName, FILE_ATTRIBUTE_NORMAL );

	//m_fpDestFile = OpenFileMakeDir( pstrDestFileName, "wb" );

	//if(m_fpDestFile != NULL)
	{
		//압축을 풀기 위한 환경 설정
		INBUFSIZE = iCompressBlockSize;
		OUTBUFSIZE = INBUFSIZE;
		inbuf = (unsigned char *) pstrCompressBlock;
		
		//@{ 2006/04/11 burumal
		//outbuf = (unsigned char *) malloc(OUTBUFSIZE);
		if ( m_pMemPool[1] )
		{
			if ( m_lMemPoolSize[1] < OUTBUFSIZE )
				AllocMemPool(OUTBUFSIZE, 1);

			if ( m_lMemPoolSize[1] >= OUTBUFSIZE )
			{
				outbuf = (unsigned char*) m_pMemPool[1];
				bUseOutputMemPool = true;
			}
		}
		else
		{
			outbuf = (unsigned char *) malloc(OUTBUFSIZE);
		}

		if ( outbuf == NULL )
			exit(1);
		//@}

		/* 모든 메모리 메니지먼트를 라이브러리에 맡긴다 */
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;

		/* 초기화 */
		z.next_in = Z_NULL;
		z.avail_in = 0;
		if (inflateInit(&z) != Z_OK) {
			fprintf(stderr, "inflateInit: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		z.next_out = outbuf;        /* 출력 포인터 */
		z.avail_out = OUTBUFSIZE;    /* 출력 버퍼 잔량 */
		status = Z_OK;

		while (status != Z_STREAM_END) 
		{
			if (z.avail_in == 0) {  /* 입력 잔량이 제로가 되면 */
				z.next_in = inbuf;  /* 입력 포인터를 바탕으로 되돌린다 */
				//            z.avail_in = fread(inbuf, 1, INBUFSIZE, m_fpSourceFile); /* 데이터를 읽는다 */
				z.avail_in = INBUFSIZE;
			}
			status = inflate(&z, Z_NO_FLUSH); /* 전개 */
			if (status == Z_STREAM_END) break; /* 완료 */
			if (status != Z_OK) {   /* 에러 */
				fprintf(stderr, "inflate: %s\n", (z.msg) ?  z.msg : "???");
				exit(1);
			}
			if (z.avail_out == 0) { /* 출력 버퍼가 다하면 */

				/* 정리해 써낸다 */
				//if (fwrite(outbuf, 1, OUTBUFSIZE, m_fpDestFile) != OUTBUFSIZE) {
					//fprintf(stderr, "Write error\n");
					//exit(1);
				//}				

				if ( m_pMemPool[0] )
				{
					// memory pool을 사용하는 경우
					lMemAllocSize += OUTBUFSIZE;

					if ( lMemAllocSize > m_lMemPoolSize[0] )
					{
						RaiseMemPool(lMemAllocSize - m_lMemPoolSize[0]);
						ASSERT(m_lMemPoolSize[0] >= lMemAllocSize);
						if ( m_lMemPoolSize[0] < lMemAllocSize )
							exit(1);
					}
					
					*ppDestMem = m_pMemPool[0];
					memcpy(*ppDestMem + lMemAllocSize - OUTBUFSIZE, outbuf, OUTBUFSIZE);
				}
				else
				{
					exit(1);
				}

				z.next_out = outbuf; /* 출력 포인터를 바탕으로 되돌린다 */
				z.avail_out = OUTBUFSIZE; /* 출력 버퍼 잔량을 바탕으로 되돌린다 */
			}
		}

		/* 나머지를 토해낸다 */
		if ((count = OUTBUFSIZE - z.avail_out) != 0) {

			//if (fwrite(outbuf, 1, count, m_fpDestFile) != count) {
				//fprintf(stderr, "Write error\n");
				//exit(1);
			//}

			if ( m_pMemPool[0] )
			{
				// memory pool을 사용하는 경우
				lMemAllocSize += count;

				if ( lMemAllocSize > m_lMemPoolSize[0] )
				{
					RaiseMemPool(lMemAllocSize - m_lMemPoolSize[0]);
					ASSERT(m_lMemPoolSize[0] >= lMemAllocSize);
					if ( m_lMemPoolSize[0] < lMemAllocSize )
						exit(1);
				}

				*ppDestMem = m_pMemPool[0];
				memcpy(*ppDestMem + lMemAllocSize - count, outbuf, count);
			}
			else
			{
				exit(1);
			}
		}

		/* 뒤처리 */
		if (inflateEnd(&z) != Z_OK) {
			fprintf(stderr, "inflateEnd: %s\n", (z.msg) ?  z.msg : "???");
			exit(1);
		}

		//
		//fclose( m_fpDestFile );

		//@{ 2006/04/11 burumal
		//free( outbuf );
		if ( bUseOutputMemPool == false )
			free(outbuf);
		//@}
	}
	//else
	{
		//이유를 알려준다.
	}

	//원래 디렉토리로 돌린다.
	//SetCurrentDirectory( strCurrentDir );

	if ( lMemAllocSize > 0 && (*ppDestMem != NULL) )
		return lMemAllocSize;

	return 0;
}

