// ksBmpLoader.cpp: implementation of the CksBmpLoader class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "bitmapLoader.h"

#include "AgcuEffUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CksBmpLoader::m_Count=0;

CksBmpLoader::CksBmpLoader()
{
	++m_Count;
	m_bSuccessfulRead=0;
	m_bRGBOrder		= 1;
	m_fp			= NULL;
	m_lpchFileName	= NULL;
	m_lpbyteBuff	= NULL;
	
}
CksBmpLoader::CksBmpLoader(LPCSTR fileName)
{
	++m_Count;
	m_bSuccessfulRead=0;
	m_bRGBOrder		= 1;
	m_fp			= NULL;
	m_lpchFileName	= NULL;
	m_lpbyteBuff	= NULL;

	DWORD namelength= (DWORD)strlen(fileName);
	m_lpchFileName	= new char[namelength+1];
	strcpy(m_lpchFileName, fileName);
	m_bSuccessfulRead=LoadData();
}

CksBmpLoader::~CksBmpLoader()
{
	DEF_SAFEDELETEARRAY(m_lpbyteBuff);
	DEF_SAFEDELETEARRAY(m_lpchFileName);
	--m_Count;
}

BOOL CksBmpLoader::Load(LPCSTR fname)
{
	DWORD namelength= (DWORD)strlen(fname);
	m_lpchFileName	= new char[namelength+1];
	strcpy(m_lpchFileName, fname);

	m_bSuccessfulRead=LoadData();

	return m_bSuccessfulRead;
}

BOOL CksBmpLoader::LoadData()
{
	int i=0; // loop index
	int ir=0; // for testing of fread, fclose's return value
	int item=0; // for num of reading item (fread)

	m_fp	= fopen(m_lpchFileName,"rb");
	if(!m_fp){
		//ERRTOWND("fopen is failed @ CksBmpLoader::LoadData()");
		return FALSE;
	}

	//bmpfileheader
	item=1;
	int fhSize	= (int)sizeof(m_bmpfh);
	int ihSize	= (int)sizeof(m_bmpih);

	ir=(int)fread(&m_bmpfh,sizeof(m_bmpfh),item,m_fp);
	if(ir<item){
		//ERRTOWND("fread is failed @ CksBmpLoader::LoadData()");
		ir=fclose(m_fp);
		if(ir==EOF){
			//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");
		}
		m_fp=NULL;
		return FALSE;
	}
	if(m_bmpfh.bfType != 0x4D42){
		ir=fclose(m_fp);
		if(ir==EOF){

			//ERRTOWND("this isn't bmp @ CksBmpLoader::LoadData()");

		}
		m_fp=NULL;
		return FALSE;
	}
	//bmpinfoheader
	ir=(int)fread(&m_bmpih,sizeof(m_bmpih),item,m_fp);

	if(ir<item){

		//ERRTOWND("fread is failed @ CksBmpLoader::LoadData()");

		ir=fclose(m_fp);
		if(ir==EOF){
			//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");
		}
		m_fp=NULL;
		return FALSE;
	}

	//data
	int widthByte;
	if(m_bmpih.biBitCount==8){
		widthByte=m_bmpih.biWidth;
	}else if(m_bmpih.biBitCount==24){
		widthByte=m_bmpih.biWidth*3;
	}else{

		//ERRTOWND("this bmp's bpp is not 8 nor 24! @ CksBmpLoader::LoadData()");

		ir=fclose(m_fp);
		if(ir==EOF){
			//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");
		}
		m_fp=NULL;
		return FALSE;
	}

	int absHeight			= abs(m_bmpih.biHeight);
	int imageByte			= widthByte * absHeight;
	int imaginaryWidthByte	= ((m_bmpih.biWidth*m_bmpih.biBitCount+31)&~31)>>3; // 넓이를 4의 배수 바이트로
	int imaginaryImage		= imaginaryWidthByte * absHeight;
	int offset				= imaginaryWidthByte - widthByte;
	//data
	/*
	if(m_bmpih.biBitCount==8){
		m_bmpih.biWidth=imaginaryWidthByte;
	}else if(m_bmpih.biBitCount==24){
		m_bmpih.biWidth=imaginaryWidthByte/3;
	}
	*/

	m_lpbyteBuff		= new BYTE[imageByte];

	//rgb order test and set
	if( (DWORD)(imaginaryWidthByte*absHeight+54)== m_bmpfh.bfSize){
		m_bRGBOrder=1;
	}else{
		m_bRGBOrder=0;
	}
	ir=fseek(m_fp,-(int)(imaginaryImage),SEEK_END);
	if(ir!=0){

			//ERRTOWND("fseek is failed @ CksBmpLoader::LoadData()");

			ir=fclose(m_fp);
			if(ir==EOF){

				//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");

			}
			m_fp=NULL;
			DEF_SAFEDELETEARRAY(m_lpbyteBuff);
			return FALSE;
	}
	//read
/*	
	ir=fread(&m_lpbyteBuff[0],sizeof(BYTE),(imaginaryImage),m_fp);
	if(ir<imaginaryImage){

		ERRTOWND("fread is failed @ CksBmpLoader::LoadData()");

		ir=fclose(m_fp);
		if(ir==EOF){

			ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");

		}
		m_fp=NULL;
		DEF_SAFEDELETEARRAY(m_lpbyteBuff);
		return FALSE;
	}
*/
	for(i=0; i<absHeight; ++i){
		ir=(int)fread(&m_lpbyteBuff[i*widthByte],sizeof(BYTE),(widthByte),m_fp);
		if(ir<item){

			//ERRTOWND("fread is failed @ CksBmpLoader::LoadData()");

			ir=fclose(m_fp);
			if(ir==EOF){

				//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");

			}
			m_fp=NULL;
			DEF_SAFEDELETEARRAY(m_lpbyteBuff);
			return FALSE;
		}
		ir=fseek(m_fp,offset,SEEK_CUR);
		if(ir!=0){
				
			//ERRTOWND("fseek is failed @ CksBmpLoader::LoadData()");
				
			ir=fclose(m_fp);				
			if(ir==EOF){
					
				//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");
				
			}
			m_fp=NULL;
			DEF_SAFEDELETEARRAY(m_lpbyteBuff);
			return FALSE;
		}
	}//for(i=0; i<m_bmpih.biHeight; ++i)

	ir=fclose(m_fp);
	if(ir==EOF){

		//ERRTOWND("fclose is failed @ CksBmpLoader::LoadData()");

		m_fp=NULL;
		DEF_SAFEDELETEARRAY(m_lpbyteBuff);
		return FALSE;
	}
	return TRUE;
}


BOOL CksBmpLoader::Save(LPCSTR fname, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* pRgbQuad, LPBYTE pBuff)
{
	FILE*	fp	= fopen(fname, "wb");
	if( !fp ){
		//ERRTOWND("fopen faield @ CksBmpLoader::Save(fname)");
		return FALSE;
	}

	if( !Save(fp, pbfh, pbih, pRgbQuad, pBuff) ){
		DEF_SAFEFCLOSE( fp );
		//ERRTOWND("Save(fp) @ CksBmpLoader::Save(fname)");
		return FALSE;
	}

	return TRUE;
}

BOOL CksBmpLoader::Save(FILE* fp, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* pRgbQuad, LPBYTE pBuff)
{
	int		 i		= 0;// loop index
	size_t	 ir		= 0	// test variable
			,item	= 1;// num of write item
	//BMP FORMAT TEST
	if( pbfh->bfType != 0x4D42 ){
		//ERRTOWND("this isn't bmpformat @ CksBmpLoader::Save(fp)");
		return FALSE;
	}

	//BPP TEST
	int widthByte;
	if(pbih->biBitCount==8){
		widthByte=pbih->biWidth;
	}else if(pbih->biBitCount==24){
		widthByte=pbih->biWidth*3;
	}else{
		//ERRTOWND("this bmp's bpp is not 8 nor 24! @ CksBmpLoader::Save(fp)");
		return FALSE;
	}

	//HEADER
	ir	= fwrite( pbfh, sizeof(BITMAPFILEHEADER), item, fp );	if( ir < item ) goto ERR_FWRITE;
	ir	= fwrite( pbih, sizeof(BITMAPINFOHEADER), item, fp );	if( ir < item ) goto ERR_FWRITE;
	
	

	if( pbih->biBitCount	== 8 ){
		if(pbih->biClrUsed){//갯수만큼
			ir	= fwrite(pRgbQuad, sizeof(RGBQUAD), pbih->biClrUsed, fp );	if( ir < pbih->biClrUsed )	goto ERR_FWRITE;
		}else{				//256개
			ir	= fwrite(pRgbQuad, sizeof(RGBQUAD), 256, fp );				if( ir < 256 )				goto ERR_FWRITE;
		}
	}

	//data

	if( true )
	{
		int absHeight			= abs(pbih->biHeight);
		int imageByte			= widthByte * absHeight;
		int imaginaryWidthByte	= ((pbih->biWidth*pbih->biBitCount+31)&~31)>>3; // 넓이를 4의 배수 바이트로
		int imaginaryImage		= imaginaryWidthByte * absHeight;
		int offset				= imaginaryWidthByte - widthByte;

		LPBYTE	pTempBuff	= new BYTE[imaginaryWidthByte];
		memset(pTempBuff, 0, imaginaryWidthByte);
		for( i=0; i<absHeight; ++i ){
			memcpy( pTempBuff, pBuff, widthByte );
			ir	= fwrite( pTempBuff, imaginaryWidthByte, 1, fp );	if( ir < 1 )	goto ERR_FWRITE;
			pTempBuff+=widthByte;
		}

		return TRUE;
	}

ERR_FWRITE:
	//ERRTOWND( "fwrite @ CksBmpLoader::Save(fp)");
	return FALSE;
}
